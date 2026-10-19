#ifndef PS3_GCC_COMPAT_H
#define PS3_GCC_COMPAT_H

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

/* CellOS has no process environment. */
static inline char *getenv(const char *name)
{
   (void)name;
   return NULL;
}

#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#define isatty(fd) 0

#ifndef __ORDER_LITTLE_ENDIAN__
#define __ORDER_LITTLE_ENDIAN__ 1234
#endif
#ifndef __ORDER_BIG_ENDIAN__
#define __ORDER_BIG_ENDIAN__ 4321
#endif
#ifndef __BYTE_ORDER__
#define __BYTE_ORDER__ __ORDER_BIG_ENDIAN__
#endif

#define fseeko fseek
#define ftello ftell

static inline uint16_t ps3_bswap16(uint16_t x)
{
   return (uint16_t)((x << 8) | (x >> 8));
}

static inline uint32_t ps3_bswap32(uint32_t x)
{
   return ((x & 0xff) << 24) | ((x & 0xff00) << 8)
      | ((x >> 8) & 0xff00) | (x >> 24);
}

static inline uint64_t ps3_bswap64(uint64_t x)
{
   return ((uint64_t)ps3_bswap32((uint32_t)x) << 32)
      | ps3_bswap32((uint32_t)(x >> 32));
}

static inline int ps3_add_overflow(int64_t a, int64_t b, int64_t *r)
{
   *r = (int64_t)((uint64_t)a + (uint64_t)b);
   return ((a ^ *r) & (b ^ *r)) < 0;
}

static inline int ps3_sub_overflow(int64_t a, int64_t b, int64_t *r)
{
   *r = (int64_t)((uint64_t)a - (uint64_t)b);
   return ((a ^ b) & (a ^ *r)) < 0;
}

#define __builtin_bswap16 ps3_bswap16
#define __builtin_bswap32 ps3_bswap32
#define __builtin_bswap64 ps3_bswap64
#define __builtin_add_overflow ps3_add_overflow
#define __builtin_sub_overflow ps3_sub_overflow
#endif
