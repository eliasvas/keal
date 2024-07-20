#ifndef MEM_H_
#define MEM_H_

#include "core_inc.h"
#include <string.h>

#if defined(OS_WINDOWS)
    #define M_RESERVE(bytes) VirtualAlloc(NULL, bytes, MEM_RESERVE, PAGE_NOACCESS)
    #define M_COMMIT(reserved_ptr, bytes) VirtualAlloc(reserved_ptr, bytes, MEM_COMMIT, PAGE_READWRITE)
    #define M_ALLOC(bytes) VirtualAlloc(NULL, bytes, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)
    #define M_RELEASE(base, bytes) VirtualFree(base, bytes, MEM_RELEASE)
    #define M_ZERO(p, s) (ZeroMemory(p, s))
#else
    #include <sys/mman.h>
    #include <string.h>
    #define M_RESERVE(bytes) mmap(NULL, bytes, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0)
    #define M_COMMIT(reserved_ptr, bytes) mmap(reserved_ptr, bytes, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)
    #define M_ALLOC(bytes) mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)
    #define M_RELEASE(base, bytes) munmap(base, bytes)
    #define M_ZERO(p, s) memset(p, 0, s)
#endif

#define M_ZERO_STRUCT(p)  M_ZERO((p), sizeof(*(p)))
#define M_ZERO_ARRAY(a)  M_ZERO((a), sizeof(a))


#define ALLOC malloc
#define REALLOC realloc
#define FREE free

#endif