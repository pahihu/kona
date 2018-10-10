#include "incs.h"

#include <stdint.h>
#include <string.h>

#include "bswap.h"

V membswp32(V d,V s,I n)
{
  uint32_t *q=d,*p=s;
  I i;
  for(i=0;i<n;i=i+4)
    *q++ = bswap32(*p++);
  R d;
}

V membswp64(V d,V s,I n)
{
  uint64_t *q=d,*p=s;
  I i;
  for(i=0;i<n;i=i+8)
    *q++ = bswap64(*p++);
  R d;
}

I bswapI(I n)
{
#ifdef __Kona32__
  R bswap32(n);
#else
  R bswap64(n);
#endif
}

V membswpI(V d,V s,I n,I x)
{
  if(x){
#ifdef __Kona32__
    R membswp32(d,s,n);
#else
    R membswp64(d,s,n);
#endif
  }
  R memcpy(d,s,n);
}

V membswpF(V d,V s,I n,I x)
{
  R x?membswp64(d,s,n):memcpy(d,s,n);
}
