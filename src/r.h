V alloc(size_t sz);
K _kona_exit(K x);
K _dot_t();
double genrand64_real2(void);
I KC(K a,K b);
I FC(F a,F b);
K _m();
K _k();
K _a();
K _u();
K _w();
K _p();
K _s();
K _f();
K _i();
K _v();
extern S d_;
K _d();
K _h();
extern K NIL;
K _T();
extern I k_epoch_offset;
K _t();
K _c();
K _ss(K a,K b);
K demote(K a);
K promote(K a);
K _sm(K a,K b);
K _setenv(K a,K b);
K _lsq(K a,K b);
C bottom(PDA p);
K take_reshape(K a,K b);
void vitter(I *a,I n,I N);
F RF();
K _draw(K a,K b);
K _bin(K x,K y);
K Kf(F x);
K _size(K a);
I stat_sz(S u,I *n);
K _ltime(K a);
K _lt(K a);
K _jd(K a);
K _ic(K a);
K kerr(cS s);
S sp(S k);
K Ks(S x);
K Ki(I x);
K _host(K a);
K _n();
S CSK(K x);
K _getenv(K a);
K _dj(K a);
K rrep(V v,V aft,I *b,I y,I s);
K _db(K x);
K _ci(K a);
K floor_ceil(K a,F(*g)(F));
K _ceiling(K a);
I wrep(K x,V v,I y);
K _bd(K x);
I rep(K x,I y);
I net(K x);
K _abs(K a);
extern V vn_[];
extern S n_s;
K at(K x,K y);
K math(F(*f)(F),K a);
K ci(K a);
F sqr(F x);
K dot(K a,K b);
K X(S s);
#if defined(DEBUG)
extern I kreci;
#endif
K vf_ex(V q,K g);
K cd(K a);
extern K KFIXED;
extern K KONA_ARGS;
extern K KONA_WHO;
extern K KONA_PORT;
extern K KONA_GSET;
extern K KONA_IDX;
extern K KONA_CLIENT;
K kap(K *a,V v);
K _acos(K x);
K _asin(K x);
K _atan(K x);
K _ceil(K x);
K _cos(K x);
K _cosh(K x);
K _exp(K x);
K _floor(K x);
K _log(K x);
K _sin(K x);
K _sinh(K x);
K _sqr(K x);
K _sqrt(K x);
K _tan(K x);
K _tanh(K x);
K _gtime(K x);
K _inv(K x);
K _binl(K x,K y);
K _di(K x,K y);
K _dot(K x,K y);
K _dv(K x,K y);
K _dvl(K x,K y);
K _hat(K x,K y);
K _in(K x,K y);
K _lin(K x,K y);
K _mul(K x,K y);
K _sv(K x,K y);
K _vs(K x,K y);
K _vsx(K x,K y);
K mstat(void);
K _md5(K a);
K _like(K x,K y);
K _rematch(K x,K y);
