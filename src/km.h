I rp2(I v);
#ifdef DEBUG
K _newE(S s,K k,S f,I ln);
K _newEntry(S s,S f,I ln);
#define newE(s,k)   _newE(s,k,__FILE__,__LINE__)
#define newEntry(s) _newEntry(s,__FILE__,__LINE__)
#define DBG(x)	x
#define KDBG(x)	if(KONA_DEBUG){x;}
#else
K newE(S s,K k);
K newEntry(S s);
#define DBG(x)
#define KDBG(x)
#endif
extern S d_;
#ifdef DEBUG
K _Kv(S f,I ln);
K _Kn(S f,I ln);
K _Kd(S f,I ln);
K _Ks(S x,S f,I ln);
K _Kc(C x,S f,I ln);
K _Kf(F x,S f,I ln);
K _Ki(I x,S f,I ln);
#define Kv()	_Kv(__FILE__,__LINE__)
#define Kn()	_Kn(__FILE__,__LINE__)
#define Kd()	_Kd(__FILE__,__LINE__)
#define Ks(x)	_Ks(x,__FILE__,__LINE__)
#define Kc(x)	_Kc(x,__FILE__,__LINE__)
#define Kf(x)	_Kf(x,__FILE__,__LINE__)
#define Ki(x)	_Ki(x,__FILE__,__LINE__)
#else
K Kv();
K Kn();
K Kd();
K Ks(S x);
K Kc(C x);
K Kf(F x);
K Ki(I x);
#endif
void pdafree(PDA p);
C bottom(PDA p);
C pop(PDA p);
C peek(PDA p);
I appender(S *s,I *n,S t,I k);
I push(PDA p,C c);
PDA newPDA();
N newN();
K kap(K *a,V v);
K kapn(K *a,V v,I n);
K _n();
extern F testtime;
#ifdef DEBUG
K _newK(I t,I n,S f,I ln);
#define newK(t,n) _newK(t,n,__FILE__,__LINE__)
#else
K newK(I t,I n);
#endif
I bp(I t);
#ifdef DEBUG
K _cig(K a,S f,I ln);
#define ci(x)	_cig(x,__FILE__,__LINE__)
#else
K ci(K a);
#endif
I repool(V v,I r);
I lsz(I k);
I glsz(K x);
I sz(I t,I n);
#if defined(DEBUG)
extern V krec[NKREC];
extern I krecLN[NKREC];
extern S krecF[NKREC];
extern I kreci;
#endif
K show(K a);
extern I tests;
#ifdef DEBUG
K _cdg(K *pA,S f,I ln);
#define cd(x)	_cdg((K*)&(x),__FILE__,__LINE__)
#else
K cd(K a);
#endif
I OOM_CD(I g, ...);
I cl2(I v);
#define rc(x)	((x)?(x)->_c>>8:0)
K mrc(K x,I c);
K mstat(void);
void trst();
void elapsed(S m);
extern I lszNode,lszPDA;
V kalloc(I k,I*r);
#define KP_MIN 5
