void init_genrand64(unsigned long long seed);
extern I SEED;
V alloc(size_t sz);
K _dot_t();
extern K KTREE;
extern K KONA_WHO;
extern K KONA_PORT;
extern K KONA_GSET;
extern K KONA_IDX;
extern K KONA_CLIENT;
extern I KONA_APL_DYAD;
extern I KONA_DEBUG;
K _n();
extern F mUsed;
extern F mMax;
extern F mAlloc;
extern F mMap;
extern I fWksp;
extern __thread I fer;
extern I fLoad;
extern S lineA;
extern S lineB;
extern C errmsg[256];
I test();
extern S IFS[3];
extern S IFP[3];
extern S LS;
extern S fnc;
extern V fncp[128];
extern I fnci;
extern I fom;
extern I fam;
extern I nfam;
extern I fll;
extern __thread K cls;
S sp(S k);
extern S d_;
extern K NIL;
void seedPRNG(I s);
N newN();
extern N SYMBOLS;
extern V offsetSSR,offsetWhat,offsetAt,offsetDot,offsetColon,offsetJoin,offset3m;
extern C vc[];
I charpos(S s,C c);
extern V vd[];
extern V adverbs[];
extern V vt_[];
extern V vd_[];
extern V vm_[];
extern V vn_[];
void finally();
I kinit();
extern K KFIXED;
K load(S s);
I args(int n,S *v);
I wipe_tape(I i);
extern S IPC_PORT;
extern S HTTP_PORT;
I attend();
extern fd_set master;
K wd(S s,int n);
K ex(K a);
I lines(FILE *f);
K kap(K *a,V v);
void pdafree(PDA p);
K kerr(cS s);
K show(K a);
I parsedepth(PDA p);
I complete(S a,I n,PDA *q,I *marks);
I appender(S *s,I *n,S t,I k);
I wds(K *a,FILE*f);
I wds_(K *a,FILE *f,I l);
I prompt(I n);
extern I adverb_ct;
extern I vn_ct,vm_ct,vd_ct,vt_ct;

#ifdef __FreeBSD__
extern ssize_t getline(S *lineptr, size_t *n, FILE *f);
#endif

K read_tape(I i,I j,I type);
I line(FILE *f,S *a,I *n,PDA *p);
K* denameS(S dir_string, S t, I create);
I ninit();
extern M0 CP[FD_SETSIZE+1];
K _h();
K _host(K x);
extern S HOST_IFACE;
