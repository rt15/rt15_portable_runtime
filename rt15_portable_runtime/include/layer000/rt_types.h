#ifndef RT_TYPES_H
#define RT_TYPES_H

/* Allows varargs usage which is more a C feature than   */
/* a library feature.                                    */
/* It is a C runtime header included in all cases but    */
/* it should have a very small footprint.                */
/* Defines _W64 that is used below.                      */
#include <stdarg.h>

/* va_copy is not defined under old MS VC versions. */
#if defined(_MSC_VER) && _MSC_VER < 1800 && !defined(va_copy)
#define va_copy(dst, src) ((dst) = (src))
#endif

/* Is debug build? */
#ifdef _DEBUG
#ifdef NDEBUG
#error Both _DEBUG and NDEBUG are defined.
#else
#define RT_DEFINE_DEBUG
#endif
#else
#ifndef NDEBUG
#error Neither _DEBUG or NDEBUG is defined.
#endif
#endif

/* Operating system. */
#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)
#define RT_DEFINE_WINDOWS
#else
#define RT_DEFINE_LINUX
/* CRT is always used on Linux. Beware that this flag cannot be used in library .c files as they are compiled once while they can be used by multiple programs. */
#ifndef RT_DEFINE_USE_CRT
#define RT_DEFINE_USE_CRT
#endif
#endif

/* Compiler. */
#ifdef __GNUC__
#define RT_DEFINE_GCC
#else
#define RT_DEFINE_VC
#endif

/* Targeted architecture. */
/* http://nadeausoftware.com/articles/2012/02/c_c_tip_how_detect_processor_type_using_compiler_predefined_macros */
#ifdef RT_DEFINE_VC
#ifdef _M_X64
#define RT_DEFINE_64
#else
#define RT_DEFINE_32
#endif
#else
#ifdef __x86_64__
#define RT_DEFINE_64
#else
#define RT_DEFINE_32
#endif
#endif

/* Compilers related defines, mostly calling conventions. */
#ifdef RT_DEFINE_GCC
#define RT_CDECL __attribute__((cdecl))
#define RT_STDCALL __attribute__((stdcall))
#else
#define RT_CDECL __cdecl
#define RT_STDCALL __stdcall
#endif
#define RT_EXPORT __declspec(dllexport)

/* Use gcc __va_copy if va_copy is not available.                                    */
/* Unlike va_copy/__va_copy, first parameter is destination, next parameter is source. */
#if defined(RT_DEFINE_GCC) && !defined(va_copy)
#define RT_VA_COPY(SOURCE, DESTINATION) __va_copy(DESTINATION, SOURCE)
#else
#define RT_VA_COPY(SOURCE, DESTINATION) va_copy(DESTINATION, SOURCE)
#endif

/* On Visual C++ and GCC, char is most likely to be signed (But different from "signed char") however there are flags to make it unsigned. */
typedef char rt_char8;
typedef unsigned char rt_uchar8;       /* BYTE.              */
typedef unsigned short rt_un16;        /* WORD.              */
typedef short rt_n16;

/* Use 16 bits characters under windows, 8 bits otherwise. */
/* RT_CHAR is most likely to be "signed (or not) RT_CHAR8" under Linux and "unsigned RT_UN16" under Windows. */
#ifdef RT_DEFINE_WINDOWS
#define _R(x) L ## x
typedef rt_un16 rt_char;
typedef rt_un16 rt_uchar;
#else
#define _R(x) x
typedef rt_char8 rt_char;
typedef rt_uchar8 rt_uchar;
#endif

/* Fixed size types */
typedef int rt_b;                      /* BOOL.              */
typedef int rt_s;                      /* Status.            */
typedef int rt_n32;                    /* INT/INT32/LONG.    */
#if defined(RT_DEFINE_WINDOWS) && defined(RT_DEFINE_64)
typedef unsigned long int rt_un32;     /* DWORD, UINT.       */
#else
typedef unsigned long rt_un32;         /* DWORD, ULONG.      */
#endif

#ifdef RT_DEFINE_GCC
typedef long long rt_n64;              /* Should be off_t as _FILE_OFFSET_BITS should be set to 64 even in 32 bits case. */
typedef unsigned long long rt_un64;
#else
typedef __int64 rt_n64;
typedef unsigned __int64 rt_un64;
#endif

/* 32/64 bits dependent types. The size of RT_N and RT_UN must be the size of a pointer. */
#if defined(RT_DEFINE_WINDOWS) && defined(RT_DEFINE_64)
typedef __int64 rt_n;                  /* long is 32 bits on msvc win 64. */
typedef unsigned __int64 rt_un;
#else
#ifdef RT_DEFINE_GCC
typedef long rt_n;                     /* Should be ssize_t on most systems (int on linux i386, long on x86_64). */
typedef unsigned long rt_un;           /* Should be size_t (Result of sizeof, can be considered primitive) but not on VC 32 bits which is using unsigned int. */
#else
/* _W64 is used to signal to VC compiler that even if the types are 32 bits, they will be 64 bits when targeting 64 bits. */
typedef _W64 long rt_n;
typedef _W64 unsigned long rt_un;
#endif
#endif
typedef void* rt_h;                    /* HANDLE.  */

/* Types equivalences:                             */
/* rt_n = LRESULT, LONG_PTR, LPARAM.               */
/* rt_un = UINT_PTR, ULONG_PTR, DWORD_PTR, WPARAM. */

/* Never use RT_TRUE/RT_OK in comparisons. rt_b and rt_s are zero or non-zero values. */
#define RT_FALSE 0
#define RT_TRUE 1
#define RT_FAILED RT_FALSE
#define RT_OK RT_TRUE

#ifdef __cplusplus
#define RT_NULL 0
#else
#define RT_NULL ((void*)0)
#endif

#ifdef RT_DEFINE_VC

/* Needed for Visual Studio compiler built-in, should be light... */
#include <intrin.h>

#else

/* Needed for size_t, should be light... */
#include <stddef.h>

#endif

#endif /* RT_TYPES_H */
