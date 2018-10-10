#include <stdint.h>

#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <sys/endian.h>
#endif

#if defined(__linux__) && defined(__GNUC__)
#include <byteswap.h>
#define bswap32 __bswap_32
#define bswap64 __bswap_64
#endif

#ifdef _MSC_VER
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
uint32_t bswap32(uint32_t n);
uint64_t bswap64(uint64_t n);
#endif

I bswapI(I n);
V membswpI(V d,V s,I n,I x);
V membswpF(V d,V s,I n,I x);
