K _ceiling(K a);
K _size(K a);
K _abs(K a);
K _bin(K x,K y);
K _bd(K x);
K _db(K x);
K _jd(K a);
K _f();
K _n();
V alloc(size_t sz);
I test();
K show(K a);
I matchI(K a,K b);
K X(S s);
extern K KTREE;
extern F testtime;
extern I tests;
extern I passed;
extern I test_print;
extern S lineB;
extern I fom;
extern I fbr;
extern __thread K cls;
I tc(S a,S b);
I tp(I x);
S ts(I x);

#if defined(DEBUG)
extern V krec[NKREC];
extern I krecLN[NKREC];
extern S krecF[NKREC];
extern I kreci;
#endif
