#include "incs.h"

#include <stdint.h>
#include <string.h>

#include "bswap.h"

#if defined(__GNUC__) && !defined(__clang__)
#include <bits/byteswap.h>
#define bswap32 __bswap_32
#define bswap64 __bswap_64
#endif

#ifdef _MSVC
#include <stdlib.h>
#define bswap32 _byteswap_ulong
#define bswap64 _byteswap_uint64
#endif

#ifdef __APPLE__
#include <libkern/_OSByteOrder.h>
#define bswap32 _OSSwapInt32
#define bswap64 _OSSwapInt64
#endif

#ifndef bswap32
uint32_t bswap32(uint32_t n)
{
  C d[4];

  memcpy(d,&n,4);
  R (d[0]<<24)+(d[1]<<16)+(d[2]<< 8)+d[3];
}

uint64_t bswap64(uint64_t n)
{
  C d[8];

  memcpy(d,&n,8);
  R ((uint64_t)(d[0])<<56)+((uint64_t)(d[1])<<48)+
    ((uint64_t)(d[2])<<40)+((uint64_t)(d[3])<<32)+
    (d[4]<<24)+(d[5]<<16)+(d[6]<< 8)+d[7];
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
