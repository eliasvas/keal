#ifndef HELPERS_H
#define HELPERS_H

#include "context.h"

#if (OS_WINDOWS)
	#include <windows.h>
#elif (OS_LINUX)
	#include <sys/mman.h>
#endif

#if (ARCH_WASM64)
	#include <emscripten.h>
	#include <emscripten/html5.h>
	#include <sys/mman.h>
#endif

#if (COMPILER_GCC)
	#define thread_loc __thread
#elif (COMPILER_CL) || (COMPILER_CLANG)
	#define thread_loc __declspec(thread)
#else
	#error Cannot define thread_loc
#endif

#define KB(val) ((val)*1024LL)
#define MB(val) ((KB(val))*1024LL)
#define GB(val) ((MB(val))*1024LL)
#define TB(val) ((GB(val))*1024LL)

#define PI 3.1415926535897f
#define align_pow2(val, align) (((val) + ((align) - 1)) & ~(((val) - (val)) + (align) - 1))
#define align2(val) align_pow2(val,2)
#define align4(val) align_pow2(val,4)
#define align8(val) align_pow2(val,8)
#define align16(val) align_pow2(val,16)
#define align32(val) align_pow2(val,32)
#define equalf(a, b, epsilon) (fabs(b - a) <= epsilon)
#define maximum(a, b) ((a) > (b) ? (a) : (b))
#define minimum(a, b) ((a) < (b) ? (a) : (b))
#define step(threshold, value) ((value) < (threshold) ? 0 : 1)
#define clamp(x, a, b)  (maximum(a, minimum(x, b)))
#define is_pow2(x) ((x & (x - 1)) == 0)
#define array_count(a) (sizeof(a) / sizeof((a)[0]))
#define signof(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))

#define each_enumv(type, it) type it = (type)0; it < type##_COUNT; it = (type)(it+1)

#endif