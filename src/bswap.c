#include "incs.h"

#include <stdint.h>
#include <string.h>

#include "bswap.h"

#ifndef bswap32
uint32_t bswap32(uint32_t n)
{
  union {
    UC d[4];
    uint32_t n;} u;

  u.n=n;
  R ((uint32_t)(u.d[0])<<24)+((uint32_t)(u.d[1])<<16)+
    ((uint32_t)(u.d[2])<< 8)+ (uint32_t)(u.d[3]);
}

uint64_t bswap64(uint64_t n)
{
  union {
    UC d[8];
    uint64_t n;} u;

  u.n=n;
  R ((uint64_t)(u.d[0])<<56)+((uint64_t)(u.d[1])<<48)+
    ((uint64_t)(u.d[2])<<40)+((uint64_t)(u.d[3])<<32)+
    ((uint64_t)(u.d[4])<<24)+((uint64_t)(u.d[5])<<16)+
    ((uint64_t)(u.d[6])<< 8)+ (uint64_t)(u.d[7]);
}
#endif

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
