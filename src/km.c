/* memory management */

#if defined(__linux__)
#define _GNU_SOURCE 1
#endif

#include "incs.h"

#include "k.h"
#include "kc.h"
#include "km.h"


//Notes on memory manager: seems like atoms (and small lists?) are not released
//by K4 (see Skelton's remark: only contiguous arrays greater than 32MB are
//returned to OS). Also: " Why do you think it is memory fragmentation? The
//allocator in kdb+ is designed specifically to avoid that by using fixed size
//buckets."
//
//Setting the minimum pool lane size to the width of a cache line can be a good idea
//This increases the number of bytes in order to improve cache performance
//See: https://github.com/ruby/ruby/pull/495
//As of 2014.01.04 cache line size is often 64 bytes (or 2^6 giving KP_MIN==6)
//There doesn't appear to be a programmatic/compiler way to determine this
//Linux: cat /proc/cpuinfo | grep cache_alignment
//OSX: sysctl -a | grep cache
//Simple tests on Kona confirmed 6 is an improvement over 5
// KP_MIN  //2^x, must be at least ceil(lg(sizeof(V)))
#define KP_MAX 26 //2^x, 26->64MB  //TODO: base on available memory at startup (fixed percent? is 64M/2G a good percent?)
Z V KP[KP_MAX+1]; //KPOOL
I PG; //pagesize:  size_t page_size = (size_t) sysconf (_SC_PAGESIZE);
F mUsed=0.0, mAlloc=0.0, mMap=0.0, mMax=0.0;
// #define GMALLOC

#ifdef DEBUG
// #define MEMDEBUG
// #define CICD
V mMinM=(V)-1;
V mMaxM=(V) 0;
I mMaxL=0;
#define TRAPP {fflush(stdout);*(volatile I*)0=1;}
#else
#define TRAPP
#endif

#ifdef CICD
#define CDBG(x)	KDBG(x)
#else
#define CDBG(x)
#endif

#ifdef MEMDEBUG
#define MEMDBG(x) x
#else
#define MEMDBG(x)
#endif

#if UINTPTR_MAX >= 0xffffffffffffffff //64 bit
#define MAX_OBJECT_LENGTH (((unsigned long long)1) << 45) //for catching obviously incorrect allocations
#else 
#define MAX_OBJECT_LENGTH (II - 1) //for catching obviously incorrect allocations
#endif
Z I kexpander(K *p,I n);
Z K kapn_(K *a,V v,I n);
Z V amem(I k,I r);
V kalloc(I k,I*r);
Z V unpool(I r);

V alloc(size_t sz) {
  V r=malloc(sz);if(!r){fputs("out of memory\n",stderr);exit(1);}
  R r; }

#ifdef __Kona32__
size_t MagicP=((size_t)0xDEADBEEF); // XXX when MagicP is 0 then only 6 failures
size_t FreeP =((size_t)0x8BADF00D);
#else
size_t MagicP=((size_t)0xDEADBEEFCAFEBABE);
size_t FreeP =((size_t)0x8BADF00D8BADF00D);
#endif
//                       1234567890
//	0	O
//	1	I L
//	2	Z
//	3	B
//	4	H
//	5	S
//	6	G
//	7	T
//	8	ATE
//	9	G

#ifdef DEBUG
#define CMR(p) if((p)&&((p)<mMinM||(p)>=mMaxM))TRAPP
Z void CKP(){
  if(PG!=4096)TRAPP;
  DO(KP_MIN,if(KP[i])TRAPP)
  DO(KP_MAX+1,CMR(KP[i]))
}
K ReadK(K x,S f,I ln) {
  if(FreeP==(size_t)x)O("\n%s:%lld reading garbage",f,ln);
  R x;
}
K CheckK(K x)
{
  // O("CheckK: %p\n",x);
  U(x);
  if(FreeP==(size_t)x)R x;
  if(x->_c<256)TRAPP; // refcnt mismatch
  if(rc(x)<0)TRAPP; // negative refcnt
  if(glsz(x)>mMaxL)TRAPP; // lane size greater than max lane size
  size_t m;I k=sz(xt,xn);memcpy(&m,(V)x+k,sizeof(size_t));
  if(m!=MagicP)TRAPP; // MagicP mismatch
  if(!xt || 5==xt)DO(xn,CheckK(x->k[i])); // recurse on list/dict
  R x;
}
#ifdef MEMDEBUG
Z void UnmarkK(K x)
{
  I k=sz(xt,xn);
  memset((V)x+k,0,sizeof(size_t));
}
Z void MarkK(K x)
{
  I k=sz(xt,xn);
  memcpy((V)x+k,&MagicP,sizeof(size_t));
}
#endif
#else
#define CKP(x)
#define CMR(p)
#endif

I OOM_CD(I g, ...) //out-of-memory count-decrement 
{ va_list a; V v,o=(V)-1;
  va_start(a,g);while(o!=(v=va_arg(a,V)))if(!v)g=1; va_end(a);
  P(!g,1)//OK
  va_start(a,g);while(o!=(v=va_arg(a,V)))cd(v); va_end(a);
  R 0;
}
Z K ic(K x){x->_c+=256;R x;}
Z K dc(K x){x->_c-=256;R x;}
I glsz(K x){R 255&(x->_c);}
Z K slsz(K x,I r){x->_c&=~(uI)255;x->_c|=r;MEMDBG(if(r>mMaxL)mMaxL=r;)R x;}
K mrc(K x,I c){I k=sz(xt,xn);I r=lsz(k);x->_c=(c<<8)|r;R x;}
Z I mCDL=0;
Z void H(I n){O("\n");DO(2*n,O(" "))}
#define STAT(x)
//Arthur says he doesn't use malloc or free. Andrei Moutchkine claims smallest unit is vm page (his truss says no malloc + add pages one at a time).
//Arthur not using malloc is probably true. No strdup & related functions in binary's strings. Note: Skelton references "different allocator" not in \w report
//This source would be improved by getting ridding of remaing malloc/calloc/realloc
#ifdef DEBUG
K _cdg(K *pX,S f,I ln){K x=*pX;
#else
K cd(K x){
#endif
  CKP();
  #ifdef DEBUG
  if(x && rc(x) <=0 ) { er(Tried to cd() already freed item) dd(tests) 
    // dd((L)x) dd(rc(x)) dd(x->t) dd(x->n) show(x);
    fprintf(stderr,"x=%p\n",x);
    fprintf(stderr,"x->c=%lld\n",rc(x));
    fprintf(stderr,"x->t=%lld\n",xt);
    fprintf(stderr,"x->n=%lld\n",xn);
    R x;
  }
  #endif 

  P(!x,0)
  P(6==xt,0)
  MEMDBG(CheckK(x);)
  dc(x);
  CDBG(H(mCDL);O("%s:%lld cd(%lld,%p,%lld)",f,ln,xt,x,rc(x));)

  if(x->_c > 255) R x;

  SW(xt)
  {
    CSR(5,)
    CS(0, 
      CDBG(mCDL++;H(mCDL-1);O("--- BEGIN CD %p ---",x);)
      STAT(trst()); DO(xn, cd(x->k[xn-i-1])); STAT(elapsed("cd"))
      CDBG(--mCDL;H(mCDL);O("--- END CD %p ---",x);)
      ) //repool in reverse, attempt to maintain order
  }

  #ifdef DEBUG
  DO(kreci, if(x==krec[i]){krec[i]=0; break; })
  #endif 

  SW(xt)
  {
    CS(7, 
      CDBG(mCDL++;H(mCDL-1);O("--- BEGIN CD %p ---",x);)
      DO(-2+TYPE_SEVEN_SIZE,cd(((V*)x->k)[2+i]))
      CDBG(--mCDL;H(mCDL);O("--- END CD %p ---",x);)
      ) //-4 special trick: don't recurse on V members. assumes sizeof S==K==V.  (don't free CONTeXT or DEPTH)
  }

  #ifdef DEBUG
  if(0)R 0; //for viewing K that have been over-freed
  #endif
  //assumes seven_type x->k is < PG
  I o=((size_t)x)&(PG-1);//file-mapped? 1:
  I r=glsz(x);
  //assert file-maps have sizeof(V)==o and unpooled blocks never do (reasonable)
  //in 32-bit Linux: sizeof(V)==4 but file-maps have o==8
  //in 64-bit Linux: sizeof(V)==8 and file-maps have o==8
  if(o==8 || r>KP_MAX){    //(file-mapped or really big) do not go back into pool.
    I k=sz(xt,xn),s=k+o;
    I res=munmap(((V)x)-o,s); if(res)R UE;
    if(o==8)mMap-=s;
    else if(r>KP_MAX)mAlloc-=s;
    mUsed-=s;
    CKP();
  }
  else repool(x,r);
  MEMDBG(*pX = FreeP;) // clear if reached 0
  R 0;
}

#ifdef DEBUG
K _cig(K x,S f,I ln)
#else
K ci(K x)
#endif
{
  CKP();

  P(!x,0)
  P(6==xt,x)
  MEMDBG(CheckK(x);)
  ic(x);
  CDBG(O("\n%s:%lld ci(%lld,%p,%lld)",f,ln,xt,x,rc(x));)

  #if 0
  SW(xt)
  {
    CSR(5,)
    CS(0, DO(xn, ci(kK(x)[i])))
  }
  #endif

  R x;
}

I bp(I t) {if(-5==t)R sizeof(I);SW(ABS(t)){CSR(1, R sizeof(I)) CSR(2, R sizeof(F)) CSR(3, R sizeof(C)) CD: R sizeof(V); } } //Default 0/+-4/5/6/7  (assumes sizeof(K)==sizeof(S)==...)
I sz(I t,I n){R 3*sizeof(I)+(7==t?TYPE_SEVEN_SIZE:n)*bp(t)+(3==ABS(t));} //not recursive. assert sz() > 0:  Everything gets valid block for simplified munmap/(free)

Z I nearPG(I i){ I k=((size_t)i)&(PG-1);R k?i+PG-k:i;}//up 0,8,...,8,16,16,...

//This is an untested idea for avoiding all that goes on in backing out of memory allocations when an error occurs inside a function before everything is done:
//If you control the memory allocator one possibility is to work in "claimed" (sbreak) but "free" space and build the K data structure there.
//Doing ci() or something similar on it marks the space "used". on error you do nothing and the space remains "free" (mutex)

//Keyword "backingstore" in old k mailing list archives - extra KSWAP beyond regular swap space

#ifdef DEBUG
K _newK(I t, I n,S f,I ln)
#else
K newK(I t, I n)
#endif
{ 
  K z;
  CKP();
  if(n>0 && n>MAX_OBJECT_LENGTH)R ME;//coarse (ignores bytes per type). but sz can overflow
  I k=sz(t,n),r;
  MEMDBG(k+=sizeof(size_t);)
  U(z=kalloc(k,&r))
  //^^ relies on MAP_ANON being zero-filled for 0==t || 5==t (cd() the half-complete), 3==ABS(t) kC(z)[n]=0 (+-3 types emulate c-string)
  ic(slsz(z,r)); z->t=t; z->n=n;
  #ifdef DEBUG
  if(kreci<NKREC){krec[kreci]=z;krecLN[kreci]=ln;krecF[kreci++]=f;};
  CDBG(O("\n%s:%lld newK=%p [%lld,%lld,%lld]",f,ln,z,z->t,z->n,rc(z));)
  #endif
  MEMDBG(MarkK(z);)
  R z;
}

#ifdef GMALLOC
Z V ma(I k)
{
  V a=malloc(k);
  R memset(a,0,k);
}
Z V UNPOOL(I lane){ R ma(1<<lane);}
#else
#define UNPOOL(x) unpool(x)
#endif

Z V kallocI(I k,I r)
{
  CKP();
#ifdef GMALLOC
  R ma(k);
#else
  if(r>KP_MAX)R amem(k,r);// allocate for objects of sz > 2^KP_MAX
  R unpool(r);
#endif
}

V kalloc(I k,I*r) //bytes. assumes k>0
{
  CKP();
  *r=lsz(k);R kallocI(k,*r);
}

Z V amem(I k,I r) {
  K z;I kk=1<<r;I kpg=kk<PG?PG:kk;
  if(MAP_FAILED==(z=mmap(0,kk,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANON,-1,0)))R ME;
  mAlloc+=kpg; //kk<PG?PG:kk;
#ifdef DEBUG
  if((V)z<mMinM)mMinM=z;
  if((V)z+kpg>mMaxM)mMaxM=z+kpg;
#endif
  if(r>KP_MAX){ mUsed+=k;if(mUsed>mMax)mMax=mUsed; }
  CKP();
  R z;
}

Z V unpool(I r)
{
  V*z;
  CKP();
  V*L=((V*)KP)+r;
  CMR(*L);
  I k= ((I)1)<<r;
  if(!*L)
  {
    U(z=amem(k,r))
    if(k<PG)
    { 
      V y=z;
      while(y<(V)z+PG+-k){
        *(V*)y=y+k;
        CMR(y+k);
        y+=k;}
    }//Low lanes subdivide pages. no divide op
    CKP();
    *L=z;
    CKP();
  }
  CKP();
  z=*L;*L=*z;*z=0;
  CKP();
  mUsed+=k; if(mUsed>mMax)mMax=mUsed;
  R z;
}

I cl2(I v) //optimized 64-bit ceil(log_2(I)) 
{
    if(!v)R -1;// no bits set
    I e = 0;
    if(v & (v - 1ULL))e=1; //round up if not a power of two
    #if UINTPTR_MAX >= 0xffffffffffffffff
      if(v & 0xFFFFFFFF00000000ULL){e+=32;v>>=32;} //64-bit or more only
    #endif
    if(v & 0x00000000FFFF0000ULL){e+=16;v>>=16;}
    //short CL2_LUT[1<<16]; DO(1<<16,if(i) CL2_LUT[i]=log2(i));
    //to use lookup table: e+=CL2_LUT[v] and comment out below. 
    if(v & 0x000000000000FF00ULL){e+=8; v>>=8; }
    if(v & 0x00000000000000F0ULL){e+=4; v>>=4; }
    if(v & 0x000000000000000CULL){e+=2; v>>=2; }
    if(v & 0x0000000000000002ULL){e+=1; v>>=1; }
    R e;
}

I lsz(I k){R k<=((I)1)<<KP_MIN?KP_MIN:cl2(k);} //pool lane from size. Ignore everywhere lanes < KP_MIN. MAX() was eliminated as an optimization
I repool(V v,I r)//assert r < KP_MAX 
{
#ifdef GMALLOC
  free(v); R 0;
#else
  I k=((I)1)<<r;
  CKP();
#ifdef DEBUG
  CMR(v);
  CMR(v+k);
#endif
  memset(v,0,k);
  CKP();
#ifdef DEBUG
  CMR(KP[r]);
#endif
  *(V*)v=KP[r];
  CKP();
  KP[r]=v;
  CKP();
  mUsed -= k;
  R 0;
#endif
}
Z I kexpander(K*p,I n) //expand only. 
{
#ifdef GMALLOC
  K a=*p;
  I c=sz(a->t,a->n),d=sz(a->t,n);
  V v=ma(d);
  memcpy(v,a,c);
  *p = v; slsz(*p,lsz(d));
  R 1;
#else
  CKP();
  K a=*p;I r = glsz(a);
  if(r>KP_MAX) //Large anonymous mmapped structure - (simulate mremap)
  {
    I d=sz(a->t,n);
    MEMDBG(d+=sizeof(size_t);)
    if(d<=(1<<r))R 1;
#if 0
    V v;I c=sz(a->t,a->n),d=sz(a->t,n),e=nearPG(c),f=d-e;
    if(f<=0) R 1;
#if defined(__linux__)
    V*w=mremap(a,c,d,MREMAP_MAYMOVE);
    if(MAP_FAILED!=w) {
      mAlloc+=d-c;mUsed+=d-c;if(mUsed>mMax)mMax=mUsed; *p=(K)w;R 1;}
#else  
    F m=f/(F)PG; I n=m, g=1; if(m>n) n++;
    DO(n, if(-1==msync((V)a+e+PG*i,1,MS_ASYNC)) {if(errno!=ENOMEM) {g=0; break;}}
          else {g=0; break;})
    if(g) if(MAP_FAILED!=mmap((V)a+e,f,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANON|MAP_FIXED,-1,0)){mAlloc+=f;mUsed+=f;if(mUsed>mMax)mMax=mUsed;R 1;}  //Add pages to end
#endif
#endif
    r=lsz(d); V v;U(v=amem(d,r))
    I c=sz(a->t,a->n);
    memcpy(v,a,c); *p=v; slsz(*p,r);
    CKP();
    I res=0; // GMALLOC I res=munmap(a,c); 
    free(a);
    if(res) { show(kerr("munmap")); R 0; }
    CKP();
    mAlloc-=c;mUsed-=c;
    R 1; //Couldn't add pages, copy to new space
  }
  I d=sz(a->t,n);
  MEMDBG(d+=sizeof(size_t);)
  //Standard pool object
  if(d<=(1<<r))R 1;
  I s=lsz(d);
  K x=kallocI(d,s); U(x)
  I c=sz(a->t,a->n);
  memcpy(x,a,c);
  CKP();
  *p=x; slsz(*p,s);
  repool(a,r);
  R 1;
#endif
}

Z K kap1_(K *a,V v)//at<=0
{
  K k=*a;
  I t=k->t,m=k->n,p=m+1;
  MEMDBG(CheckK(k);)
  if(!kexpander(&k,p))R 0;
  if(k!=*a)
  {
    #ifdef DEBUG
    DO(kreci, if(*a==krec[i]){krec[i]=0; break; })
    if(kreci<NKREC){krec[kreci]=k;krecLN[kreci]=__LINE__;krecF[kreci++]=__FILE__;}
    #endif
    *a=k;
  }
  MEMDBG(UnmarkK(k);)
  k->n=p;
  MEMDBG(MarkK(k);)
  SW(-t)
  {
    CS(0, kK(k)[m]=ci(((K*)v)[0]));
    CS(1, kI(k)[m]=*(I*)v);
    CS(2, kF(k)[m]=*(F*)v);
    CS(3, kC(k)[m]=*(C*)v;kC(k)[p]=0);
    CS(4, kS(k)[m]=*(S*)v)
    CD:   R 0;
  }
  CKP();
  R k;
}

Z K kapn_(K *a,V v,I n)
{
  if(!a||!n)R 0;
  K k=*a;
  MEMDBG(CheckK(k);)
  I t=k->t,m=k->n,p=m+n;
  if(6==t)
  {
    K z=newK(0,p);U(z)
    K *zv=kK(z);
    *zv++=_n(); DO(n, zv[i]=_n());
    CKP();
    cd(k);
    *a=z;
    R z;
  }
  if(!kexpander(&k,p))R 0;
  if(k!=*a)
  {
    #ifdef DEBUG
    DO(kreci, if(*a==krec[i]){krec[i]=0; break; })
    if(kreci<NKREC){krec[kreci]=k;krecLN[kreci]=__LINE__;krecF[kreci++]=__FILE__;}
    #endif
    *a=k;
  }
  MEMDBG(UnmarkK(k);)
  k->n=p;
  MEMDBG(MarkK(k);)
  CKP();
  SW(ABS(t))
  {
    CSR(0,) CS(5, DO(n, kK(k)[i+m]=ci(((K*)v)[i])));
    CS(1, memcpy(kI(k)+m,v,n*sizeof(I)));
    CS(2, memcpy(kF(k)+m,v,n*sizeof(F)));
    CS(3, strncpy(kC(k)+m,(S)v,n); kC(k)[p]=0);
    CS(4, memcpy(kS(k)+m,v,n*sizeof(S)))
    CD:   R 0;
  }
  CKP();
  if(t>0&&t<5&&p>1)k->t*=-1;
  R *a;
}

extern K kapn(K *a,V v,I n){
  MEMDBG(CheckK(*a);)
  K r=kapn_(a,v,n);
  MEMDBG(CheckK(r);)
  R r;}

extern K kap(K*a,V v){ 
  MEMDBG(CheckK(*a);)
  if(!a)R 0;
  K r=(0<(*a)->t)?kapn_(a,v,1):kap1_(a,v); 
  MEMDBG(CheckK(r);)
  R r;}
//extern K kap(K*a,V v){R kapn_(a,v,1);}

I lszPDA,lszNode;
N newN(){R UNPOOL(lszNode);}
PDA newPDA(){PDA p=UNPOOL(lszPDA);U(p) p->c=alloc(1); if(!p->c){ME;R 0;} R p;}
I push(PDA p, C c){R appender(&p->c,&p->n,&c,1);} 
C    peek(PDA p){I n=p->n; R n?p->c[n-1]:0;}
C     pop(PDA p){R p->n>0?p->c[--(p->n)]:0;}
C  bottom(PDA p){R p->n>0?p->c[0]:0;}
void pdafree(PDA p){free(p->c); repool(p,lszPDA);}

#ifdef DEBUG
K _Ki(I x,S f,I ln){K z=_newK(1,1,f,ln);*kI(z)=x;R z;}
K _Kf(F x,S f,I ln){K z=_newK(2,1,f,ln);*kF(z)=x;R z;}
K _Kc(C x,S f,I ln){K z=_newK(3,1,f,ln);*kC(z)=x;R z;}
K _Ks(S x,S f,I ln){U(x) K z=_newK(4,1,f,ln);*kS(z)=x;R z;}//KDB+ >= 2.4 tries interning [sp()]  by default when generating sym atoms 
K _Kd(    S f,I ln){R   _newK(5,0,f,ln);}
K _Kn(    S f,I ln){R   _newK(6,1,f,ln);}//Should n instead be 0? (Won't affect #:) in k3.2 yes  //In K3.2 _n->n is overridden for error messages. 
K _Kv(    S f,I ln){K z=_newK(7,TYPE_SEVEN_SIZE,f,ln);U(z) z->n=1;kV(z)[CONTeXT]=d_; M(z,kV(z)[PARAMS]=Kd(),kV(z)[LOCALS]=Kd()) R z;} //z->n == 0-wd 1-wordfunc 2-cfunc 3-charfunc 4-:[] 5-if[] 6-while[] 7-do[]
#else
K Ki(I x){K z=newK(1,1);*kI(z)=x;R z;}
K Kf(F x){K z=newK(2,1);*kF(z)=x;R z;}
K Kc(C x){K z=newK(3,1);*kC(z)=x;R z;}
K Ks(S x){U(x) K z=newK(4,1);*kS(z)=x;R z;}//KDB+ >= 2.4 tries interning [sp()]  by default when generating sym atoms 
K Kd(   ){R   newK(5,0);}
K Kn(   ){R   newK(6,1);}//Should n instead be 0? (Won't affect #:) in k3.2 yes  //In K3.2 _n->n is overridden for error messages. 
K Kv(   ){K z=newK(7,TYPE_SEVEN_SIZE);U(z) z->n=1;kV(z)[CONTeXT]=d_; M(z,kV(z)[PARAMS]=Kd(),kV(z)[LOCALS]=Kd()) R z;} //z->n == 0-wd 1-wordfunc 2-cfunc 3-charfunc 4-:[] 5-if[] 6-while[] 7-do[]
#endif

//Optimization: It's better if Kv() doesn't set PARAMS and LOCALS. Only charfuncs should set params

#ifdef DEBUG
K _newEntry(S s,S f,I ln){R _newE(s,_n(),f,ln);}//assumes s came from sp()
K _newE(S s, K k,S f,I ln) //oom
{
  K z=_newK(0,3,f,ln); U(z)
  kK(z)[0]=_Ks(s,f,ln); // be careful -- s must have come from sp()
  kK(z)[1]=k;
  kK(z)[2]=_n();
  M(z,kK(z)[0],kK(z)[2]) //May want to redesign this function (& newEntry) to ci(k==kK(z)[1])
  R z;
}
#else
K newEntry(S s){R newE(s,_n());}//assumes s came from sp()
K newE(S s, K k) //oom
{
  K z=newK(0,3); U(z)
  kK(z)[0]=Ks(s); // be careful -- s must have come from sp()
  kK(z)[1]=k;
  kK(z)[2]=_n();
  M(z,kK(z)[0],kK(z)[2]) //May want to redesign this function (& newEntry) to ci(k==kK(z)[1])
  R z;
}
#endif
I rp2(I v){v--;v|=v>>1;v|=v>>2;v|=v>>4;v|=v>>8;v|=v>>16;if(sizeof(V)>=8)v|=v>>32;v++;R MAX(1,v);}//round up to integer power of 2 (fails on upper 1/4 signed)

K mstat(){K ks=newK(-1,4);M(ks);I*s=kI(ks);s[0]=mUsed;s[1]=mAlloc;s[2]=mMap;s[3]=mMax;R ks;}
