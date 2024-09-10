#ifndef CUSTOM_TYPES_H
#define CUSTOM_TYPES_H

#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

typedef uint8_t   u8;
typedef int8_t    s8;
typedef uint16_t  u16;
typedef int16_t   s16;
typedef uint32_t  u32;
typedef int32_t   s32;
typedef uint64_t  u64;
typedef int64_t   s64;
typedef float     f32;
typedef double    f64;
typedef int32_t   b32;
typedef char      b8;

static s8  S8_MIN  = (s8)0x80;
static s16 S16_MIN = (s16)0x8000;
static s32 S32_MN  = (s32)0x80000000;
static s64 S64_MIN = (s64)0x8000000000000000llu;
static s8  S8_MAX  = (s8) 0x7f;
static s16 S16_MAX = (s16)0x7fff;
static s32 S32_MAX = (s32)0x7fffffff;
static s64 S64_MAX = (s64)0x7fffffffffffffffllu;
static u8  U8_MAX  = 0xff;
static u16 U16_MAX = 0xffff;
static u32 U32_MAX = 0xffffffff;
static u64 U64_MAX = 0xffffffffffffffffllu;

static f64 F64_MAX = 1.7976931348623158e+308;
static f64 F64_MIN_POS = 2.2250738585072014e-308;
static f32 F32_MAX = 3.402823466e+38F;
static f32 F32_MIN_POS = 1.175494351e-38F;
#endif