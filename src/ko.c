/* K object management */

#include "incs.h"

#include "k.h"
#include "kc.h"
#include "km.h"
#include "ko.h"

K _kclone(K x);
K _kcopy(K a);
K kcloneI(K a,cS f,int n){
  if(KONA_DEBUG)O("\n%s:%d kclone(%p)",f,n,a);
  R _kclone(a);}
K kcopyI(K a,cS f,int n){//Shallow copy - list/dict
  if(KONA_DEBUG)O("\n%s:%d kcopy(%p)",f,n,a);
  R _kcopy(a);}
K _kcopy(K x)//Shallow copy - list/dict, deep copy - TYPE_SEVEN, others ci()
{
  U(x);
  if(7==xt)R _kclone(x);
  if(0!=xt && 5!=xt)R ci(x);
  K z=newK(xt,xn),b,e;
  if(5==xt)DO(xn,b=kK(x)[i];kK(z)[i]=(e=newK(0,3));M(e,z);kK(e)[0]=_kcopy(kK(b)[0]);kK(e)[1]=_kcopy(kK(b)[1]);kK(e)[2]=_kcopy(kK(b)[2]);)
  else DO(xn,kK(z)[i]=_kcopy(kK(x)[i]));
  R z;
}

K _kclone(K a)//Deep copy -- eliminate where possible
{
  if(!a) R 0;
  I t=a->t,n=a->n;
  K z= 7==t?Kv():6==t?_n():newK(-5==t?-1:t,n);z->t=t;
  if     (4==ABS(t)) DO(n, kS(z)[i]=kS(a)[i])  //memcpy everywhere is better
  else if(3==ABS(t)) DO(n, kC(z)[i]=kC(a)[i])
  else if(2==ABS(t)) DO(n, kF(z)[i]=kF(a)[i])
  else if(-5==t||1==ABS(t)) DO(n, kI(z)[i]=kI(a)[i])
  else if(0==    t ) DO(n, kK(z)[i]=_kclone(kK(a)[i]))
  else if(5==    t ) DO(n, kK(z)[i]=_kclone(kK(a)[i]))
  else if(7==    t )
  {
    I k=0;

    z->t=a->t;
    I vt=z->n = a->n;
    K kv=0;

    V*v;
    SW(vt)
    {
      CS(1, k=((K)kV(a)[CODE])->n-1;
            M(z,kv=newK(-4,k+1))
            v=(V*)kK(kv);
            //v[k]=0;//superfluous reminder
            DO(k, V w=kW(a)[i];
                  if(VA(w))v[i]=w;  //TODO: is this ok for NAMES? see similar code in capture()
                  else
                  {
                    K r=_kclone(*(K*)w); //oom
                    V q=newE(LS,r); //oom
                    kap((K*) kV(z)+LOCALS,&q);//oom XXX pahihu: this builds LOCALS(z) which is referenced in CODE(z)
                    cd(q);//kap does ci
                    q=EVP(q); //oom free z etc. kap needs checking
                    v[i]=q;
                  }
              )
      )
      CS(2, M(z,kv=newK(-4,3))
            v=(V*)kK(kv);
            memcpy(v,kW(a),sizeof(V));
        )
      CS(3,M(z,kv=_kclone((K)kV(a)[CODE])))
    }
    kV(z)[CODE]=kv;
    kV(z)[DEPTH]=kV(a)[DEPTH];
    kV(z)[CONTeXT]=kV(a)[CONTeXT];
    cd(kV(z)[PARAMS]); kV(z)[PARAMS]=_kclone(kV(a)[PARAMS]); //oom ; fill instead of kclone?
    if(1!=vt){cd(kV(z)[LOCALS]); kV(z)[LOCALS]=_kclone(kV(a)[LOCALS]);} //oom ; fill instead of kclone? XXX pahihu why replace this see above
    kV(z)[CONJ]=_kclone(kV(a)[CONJ]);  //oom
  }

  R z;
}

K collapse(K x) //oom
{
  if(xt==1 && xn==1) R x;
  K z;
  if(1==xn){ z=ci(*kK(x)); cd(x);}
  else z=demote(x);
  R z;
}

K delist(K x){K z=0,t=x; while(t->t==0 && t->n==1){z=*kK(t); t=z;} z=ci(t); cd(x); R z;}

K demote(K a)//Attempt to force unnaturally occurring lists into vectors
{ // change: (0;1;2) ->  0 1 2
  //   keep: (1;0.66667)  //numerics are not reconciled as you might guess
  // change: (1) -> ,1    //doesn't solve parenthetical expressions offhand
  if(!a) R a; //dollar() uses this
  I t=a->t, n=a->n;
  if(0!=t || 1>n) R a;
  I p=kK(a)[0]->t;
  DO(n, if(p!=kK(a)[i]->t){p=0;break;})
  if(!(1<=p && p <= 4))R a;
  K z=newK(-p,n); M(a,z)
  if     (4==p)DO(n,kS(z)[i]=*kS(kK(a)[i])) //use memcpy instead
  else if(3==p)DO(n,kC(z)[i]=*kC(kK(a)[i]))
  else if(2==p)DO(n,kF(z)[i]=*kF(kK(a)[i]))
  else if(1==p)DO(n,kI(z)[i]=*kI(kK(a)[i]))
  cd(a);
  if(z->t==-1 && z->n==1) z->t=1;
  R z;
}
K promote(K a)//Identity on lists. Lists from vectors. Pseudo-enlist on atoms (always 0-lists).
{ //0 1 2 -> (0;1;2)
  I at=a->t;
  if(0==at) R ci(a);
  if(4< at) {K z=newK(0,1); U(z); *kK(z)=ci(a); R z;}
  K z=newK(0,a->n); U(z);
  K x;
  I v=ABS(at);
  if     (4==v) DO(a->n, x=newK(v,1); M(x,z) *kS(x)=kS(a)[i]; kK(z)[i]=x )
  else if(3==v) DO(a->n, x=newK(v,1); M(x,z) *kC(x)=kC(a)[i]; kK(z)[i]=x )
  else if(2==v) DO(a->n, x=newK(v,1); M(x,z) *kF(x)=kF(a)[i]; kK(z)[i]=x )
  else if(1==v) DO(a->n, x=newK(v,1); M(x,z) *kI(x)=kI(a)[i]; kK(z)[i]=x )
  R z;
}
