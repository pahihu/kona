/* strings & string interning */

#include "incs.h"

#include "k.h"
#include "ks.h"
#include "vg.h"
#include "km.h"
#include <time.h>

#if defined(__LITTLE_ENDIAN__) || defined(__ORDER_LITTLE_ENDIAN__)
#warning Using bswap32()
#include "bswap.h"
#define BIG(x)	((StrChunk)bswap32(x))
#else
#define BIG(x)	(x)
#endif

typedef int32_t StrChunk;

/* 181009AP see http://mgronhol.github.io/fast-strcmp/, but modified */
Z I fastcmp(S p0,S p1,I n){
  int cb=0;
  int nb=n/sizeof(StrChunk);
  int o=n-nb*sizeof(StrChunk);
  StrChunk *lp0=(StrChunk*)p0,v0;
  StrChunk *lp1=(StrChunk*)p1,v1;
  while(cb<nb){
    v0=lp0[cb];v1=lp1[cb];
    if(v0^v1){
      // fprintf(stderr,"DBG: v0:%x(%x) v1:%x(%x)\n",v0,BIG(v0),v1,BIG(v1));
      R BIG(v0)-BIG(v1);
    }
    cb++;
  }
  if(o){
    StrChunk msk=((StrChunk)1<<(8*sizeof(StrChunk)-1))>>(8*o);
    v0=msk&BIG(lp0[cb]);v1=msk&BIG(lp1[cb]);
    R v0-v1;
  }
  R 0;
}

Z I scn(S a,I na,S b,I nb){
  I r;
  if(na<nb){
    r=fastcmp(a,b,na);
    if(!r)r=-1;
  }else if(nb<na){
    r=fastcmp(a,b,nb);
    if(!r)r=1;
  }else r=fastcmp(a,b,na);
  // fprintf(stderr,"DBG: %s(%lld) %s(%lld) r:%lld\n",a,na,b,nb,r);
  R r<0?-1:r>0/*?1:0*/;
}

Z I SEQ(S a,I na,S b,I nb){
  if(na!=nb)R 1;
  R fastcmp(a,b,na);
}

Z K ht=0;
Z I ns=0,sd=0;
Z uI maxS=0;
Z N hg(K h,uI hk,S k,I nk,uI*p)
{
  uI s=0;
  I n=h->n;N *d=(N*)kV(h);uI u=hk&(n-1);
  while(d[u]){
    if(!SEQ(k,nk,d[u]->k,d[u]->nk)){*p=u;if(s>maxS)maxS=s;R d[u];}
    if(++u==n)u=0;
    s++;
  }*p=u;if(s>maxS)maxS=s;R 0;
}

Z I hLoad;

Z void hins(N d,uI hint){
  uI p=0;
  // fprintf(stderr,"DBG: hins\n");
  if(!ht){ht=newH(-4,8191);hLoad=2*ht->n/3;}
  if(ns>hLoad){
    I m=1<<(1+cl2(ns));
    if(m!=ht->n){
      K ho=ht;ht=newH(-4,ns);
      // fprintf(stderr,"DBG: ns=%lld ho->n=%lld maxS=%llu ht->n=%lld\n",ns,ho->n,maxS,ht->n);
      maxS=0;
      DO(ho->n,N s=kV(ho)[i];if(s){if(!hg(ht,SV(s->k,SLOT_H),s->k,s->nk,&p))kV(ht)[p]=s;});
      cd(ho);
      hLoad=2*ht->n/3;
      hint=0;
    }
  }
  p=hint;if(!p)hg(ht,SV(d->k,SLOT_H),d->k,d->nk,&p);
  kV(ht)[p]=d;
}

Z N hget(S s,I ns,uI *hc,uI *hint){
  // fprintf(stderr,"DBG: hget\n");
  *hint=0;
  if(!ht)R 0;
  *hc=fnv1a((UC*)s,ns);
  if(!hg(ht,*hc,s,ns,hint))R 0;
  R kV(ht)[*hint];
}

#define SCHUNK (16<<20)
Z S SP;uI nSP=0;
Z S salloc(I k){
  if(nSP<k){SP=alloc(nSP=SCHUNK);U(SP);SP+=SCHUNK;}
  nSP-=k;SP-=k;R SP;
}

size_t sizeofSym(size_t k){I nb=(k+1+sizeof(StrChunk)-1)/sizeof(StrChunk);R NSLOTS*sizeof(Slot)+nb*sizeof(StrChunk);}
// Z S sdup(S s){R strdupn(s,strlen(s));} //using this because "strdup" uses [used] dynamically linked malloc which fails with our static free
Z S sdupI(S s,I k,uI hc){
  I nb=(k+1+sizeof(StrChunk)-1)/sizeof(StrChunk);
  //I r;UC *d=kalloc(NSLOTS*sizeof(Slot)+nb*sizeof(StrChunk),&r);U(d);
  S d=salloc(NSLOTS*sizeof(Slot)+nb*sizeof(StrChunk));U(d);
  ns++;sd=1;d+=NSLOTS*sizeof(Slot);((StrChunk*)d)[nb-1]=0;
  d[k]=0;memcpy(d,s,k);SV(d,SLOT_H)=hc;R (S)d;}
S strdupn (S s,I k) {S d=alloc(k+1);if(!d)R 0;d[k]=0;R memcpy(d,s,k);} // mm/o  (note: this can overallocate)
//I SC0N(S a,S b,I n) {I x=memcmp(a,b,n); R x<0?-1:x>0?1:a[n]?1:0; }// non-standard way to compare aaa\0 vs aaa
I strlenn(S s,I k){S t=memchr(s,'\0',k); R t?t-s:k;}

I StoI(S s,I *n){S t; errno=0; *n=strtol(s,&t,10); R !(errno!=0||t==s||*t!=0);}

I SC(S a,S b){I x=strcmp(a,b); R x<0?-1:x>0/*?1:0*/;}//String Compare: strcmp unfortunately does not draw from {-1,0,1}
I SCN(S a,S b,I n){I x=strncasecmp(a,b,n); R x<0?-1:x>0/*?1:0*/;}
S spI(S k,I nk)//symbol from phrase: string interning, Ks(sp("aaa")). This should be called before introducing any sym to the instance
{ //We are using this to ensure any two 'character-identical' symbols are in fact represented by the same pointer S
  //See Knuth Algorithm 6.2.2T
  #define LINK(n,x) (n)->c[((x)+1)/2] // -1 => 0 , 1 => 1
  if(!k)R 0;//used in glue. used in _2m_4. used in parse. Probably a good argument to keep since it's exposed for libraries via 2: dyadic
  uI hk,hint;N z=hget(k,nk,&hk,&hint);if(z)R z->k;
  N t=SYMBOLS, s=t->c[1],p=s,q=p,r; I a,x;
  if(!s){s=t->c[1]=newN();P(!s,(S)ME);s->k=sdupI(k,nk,hk); if(!s->k){repool(s,lszNode);t->c[1]=0;ME;} s->nk=nk;hins(s,hint);R s->k;} // <-- strdup here and below
  while(q)
  { if(!(a=scn(k,nk,p->k,p->nk))){R p->k;}//In the usual tree put: p->k=k,p->v=v before returning
    if(!(q=LINK(p,a))){q=newN();P(!q,(S)ME);q->k=sdupI(k,nk,hk);if(!q->k){repool(q,lszNode);ME; R 0;} q->nk=nk;hins(q,hint);LINK(p,a)=q;break;}//Usual tree would q->v=v. mmo
    else if(q->b){t=p;s=q;}
    p=q;
  }
  a=0>scn(k,nk,s->k,s->nk)?-1:1;
  r=p=LINK(s,a);
  while(p!=q){x=scn(k,nk,p->k,p->nk); p->b=x;p=LINK(p,x);}
  if(!s->b){s->b=a;R p->k;}
  else if(s->b==-a){s->b=0; R p->k;}
  if(r->b==a){p=r; LINK(s,a)=LINK(r,-a); LINK(r,-a)=s; s->b=r->b=0;}
  else if(r->b==-a)
  { p=LINK(r,-a); LINK(r,-a)=LINK(p,a);
    LINK(p,a)=r; LINK(s,a)=LINK(p,-a); LINK(p,-a)=s;
    if     (p->b== a){s->b=-a; r->b=0;}
    else if(p->b== 0){s->b= 0; r->b=0;}
    else if(p->b==-a){s->b= 0; r->b=a;}
    p->b=0;
  }
  t->c[s==t->c[1]/*?1:0*/]=p;
  R q->k;
}
S sp(S s){R spI(s,strlen(s));}

//S spkC(K a){S u=strdupn(kC(a),a->n),v=sp(u);free(u);R v;}
S spn(S s,unsigned n){unsigned k=0;while(k<n && s[k])k++; /*S u=strdupn(s,k); if(!u)R 0;*/ S v=spI(s,k); /*free(u);*/ R v;} //safer/memory-efficient strdupn
Slot wleft(N x,unsigned y,Slot z)
{
  if(!x)R z;
  z=wleft(x->c[0],y,z);
  if(x->k&&SV(x->k,y)){Slot o=SV(x->k,y);SV(x->k,y)=z;z+=o;}
  R wleft(x->c[1],y,z);
}
Slot wright(N x,unsigned y,Slot z)
{
  if(!x)R z;
  z=wright(x->c[1],y,z);
  if(x->k&&SV(x->k,y)){Slot o=SV(x->k,y);SV(x->k,y)=z;z+=o;}
  R wright(x->c[0],y,z);
}
Z void ssI(N x,unsigned y,Slot z){if(x){DO(2,ssI(x->c[i],y,z));if(x->k)SV(x->k,y)=z;}}
void setS(unsigned y,Slot z){ssI(SYMBOLS,y,z);}
void OS(N x,unsigned y)
{
  if(!x)R;
  OS(x->c[0],y);
  if(x->k&&SV(x->k,y))O("%p: %u\n",x->k,SV(x->k,y));
  OS(x->c[1],y);
}
I SM=0;I smark(void){R ++SM;}
