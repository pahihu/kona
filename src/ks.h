S spn(S s,unsigned n);
N newN();
extern N SYMBOLS;
V alloc(size_t sz);
S sp(S k);
S spI(S k,I nk);
I SC(S a,S b);
I SCN(S a,S b,I n);
#define SN(s) strlen(s)
I StoI(S s,I *n);
I strlenn(S s,I k);
S strdupn(S s,I k);
I gradeS();
K mergeGrade(K a,I r);
void setS(unsigned y,Slot z);
Slot wleft(N x,unsigned y,Slot z);
Slot wright(N x,unsigned y,Slot z);
void OS(N x,unsigned y);
size_t sizeofSym(size_t k);
