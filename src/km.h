I rp2(I v);
K newE(S s,K k);
K newEntry(S s);
extern S d_;
K Kv();
K Kn();
K Kd();
K Ks(S x);
K Kc(C x);
K Kf(F x);
K Ki(I x);
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
K _cdg(K a,S f,I ln);
#define cd(x)	_cdg(x,__FILE__,__LINE__)
#else
K cd(K a);
#endif
I OOM_CD(I g, ...);
I cl2(I v);
#define rc(x)	((x)->_c>>8)
K mrc(K x,I c);
K mstat(void);
void trst();
void elapsed(S m);
extern I lszNode,lszPDA;
V kalloc(I k,I*r);
