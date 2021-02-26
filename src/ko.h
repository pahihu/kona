K at(K x,K y);
K promote(K a);
K demote(K a);
K collapse(K x);
K delist(K x);
K *EVP(K e);
K kap(K *a,V v);
extern S LS;
I VA(V p);
K kcloneI(K a,const char*f,int n);
#define kclone(a)   kcloneI(a,__FILE__,__LINE__)
K kcopyI(K a,const char*f,int n);
#define kcopy(a)    kcopyI(a,__FILE__,__LINE__)
