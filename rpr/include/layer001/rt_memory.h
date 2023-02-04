#ifndef RT_MEMORY_H
#define RT_MEMORY_H

#include "layer000/rt_types.h"

/**
 * @file
 * Memory utility functions.
 *
 * <p>
 * To compare/copy/move/set memory, use macros.<br>
 * If the CRT is used, then CRT/intrinsics will be used.<br>
 * If CRT is not used then implementations from rpr_mem.h/intrinsics will be used.<br>
 * Intrinsics are much faster than CRT/rpr_mem.h (They are "inlined" and the size is known).
 * </p>
 */

#ifdef RT_DEFINE_GCC

#define RT_MEMORY_SWAP_BYTES16(VAR) __builtin_bswap16(VAR)
#define RT_MEMORY_SWAP_BYTES32(VAR) __builtin_bswap32(VAR)

#else

#define RT_MEMORY_SWAP_BYTES16(VAR) _byteswap_ushort(VAR)
#define RT_MEMORY_SWAP_BYTES32(VAR) _byteswap_ulong(VAR)

#endif

/* Equivalence gate. True in case of boolean equality. More like NXOR, logical complement of the Exclusive OR (XOR) gate. */
#define RT_MEMORY_XNOR(BOOL1, BOOL2) (!(BOOL1) == !(BOOL2))

/* Find the offset of given member in given structure. */
#ifdef RT_DEFINE_VC
/* __builtin_offsetof does not exist in VC 2005. It may exist in VC 2019. */
#define RT_MEMORY_OFFSET_OF(STRUCT, MEMBER) ((size_t)((char *)&((STRUCT *)0)->MEMBER - (char *)0))
#else
#define RT_MEMORY_OFFSET_OF(STRUCT, MEMBER) __builtin_offsetof(STRUCT, MEMBER)
#endif

/* Find the address of the "container" of given member. */
#define RT_MEMORY_CONTAINER_OF(POINTER, STRUCT, MEMBER) ((STRUCT*)((char*)(POINTER) - RT_MEMORY_OFFSET_OF(STRUCT, MEMBER)))

/* UINT % sizeof(rt_un). */
/* Notice that most compilers should automatically perform this optimization as sizeof(rt_un) is known at runtime. */
/* This macro only applies to positive integers. */
#ifdef RT_DEFINE_32
#define RT_MEMORY_MODULO_RT_UN_SIZE(UINT) (UINT & 3)
#else
#define RT_MEMORY_MODULO_RT_UN_SIZE(UINT) (UINT & 7)
#endif

/* C modulo operator % is very often dramatically slow because it is an integer division. */
/* On the other hand, modulo power of two is very fast because it is only a bitwise operation. */
/* As a result, you should try to always use following macro instead of % operator. */
#define RT_MEMORY_MODULO_POWER_OF_TWO(UINT, POWER_OF_TWO) (UINT & (POWER_OF_TWO - 1))

/* Can be used to check that a given value is power of two to be able to use RT_MEMORY_MODULO_POWER_OF_TWO later on. */
#define RT_MEMORY_IS_POWER_OF_TWO(UINT) (UINT && ((UINT & (UINT - 1)) == 0))

/* Count the number of zero bits before the first set bit. */
/* It is equivalent to the position of the first set bit.  */
#ifdef RT_DEFINE_GCC
#define RT_MEMORY_BIT_SCAN_FORWARD(CHUNK_SIZE) __builtin_ctzl(CHUNK_SIZE)
#else
rt_n32 rt_memory_bit_scan_forward(rt_un x);
#define RT_MEMORY_BIT_SCAN_FORWARD(CHUNK_SIZE) rt_memory_bit_scan_forward(CHUNK_SIZE)
#endif

/* Compute the count of chunks given <tt>SIZE</tt> (The elements count) and the size of a chunk. */
/* CHUNK_SIZE must be a power of 2. */
#define RT_MEMORY_GET_CHUNKS_COUNT(SIZE, CHUNK_SIZE) ((SIZE >> RT_MEMORY_BIT_SCAN_FORWARD(CHUNK_SIZE)) + (RT_MEMORY_MODULO_POWER_OF_TWO(SIZE, CHUNK_SIZE) ? 1 : 0))

#ifndef RT_DEFINE_USE_CRT
#ifdef RT_DEFINE_GCC

/* No CRT so we define built-ins. */
int __cdecl memcmp(const void*, const void*, size_t);
void *__cdecl memcpy(void*, const void*, size_t);
void *__cdecl memmove(void*, const void*, size_t);
void *__cdecl memset(void*, int, size_t);

#endif
#else

/* We will use CRT memcmp/memcpy/memmove/memset below. */
int memcmp(const void*, const void*, size_t);
void *memcpy(void*, const void*, size_t);
void *memmove(void*, const void*, size_t);
void *memset(void*, int, size_t);

#endif

/**
 * Compare two memory areas.<br>
 * Return a positive number if the first different byte is greater in <tt>AREA1</tt>
 *
 * <p>
 * Like for memcmp, values inside areas are considered unsigned chars.
 * </p>
 *
 * @return rt_n32 (memcmp returns an int).
 */
#define RT_MEMORY_COMPARE(AREA1, AREA2, SIZE) memcmp(AREA1, AREA2, SIZE)

/**
 * Copy SIZE bytes from SOURCE to DESTINATION.
 *
 * <p>
 * Order of source and destination are the opposite of memcpy.
 * </p>
 *
 * @return <tt>DESTINATION</tt>
 */
#define RT_MEMORY_COPY(SOURCE, DESTINATION, SIZE) memcpy(DESTINATION, SOURCE, SIZE)

/**
 * Copy SIZE bytes from SOURCE to DESTINATION.
 *
 * <p>
 * Act like if a temporary buffer would have been used to avoid overwriting issues.
 * </p>
 *
 * @return <tt>DESTINATION</tt>
 */
#define RT_MEMORY_MOVE(SOURCE, DESTINATION, SIZE) memmove(DESTINATION, SOURCE, SIZE)

/**
 * Set SIZE bytes with VALUE in AREA.
 *
 * @param VALUE Value set to each byte in the area.
 */
#define RT_MEMORY_SET(AREA, VALUE, SIZE) memset(AREA, VALUE, SIZE)

#define RT_MEMORY_ZERO(AREA, SIZE) memset(AREA, 0, SIZE)

rt_n32 rt_memory_compare(const void *area1, const void *area2, rt_un size);
void *rt_memory_copy(const void *source, void *destination, rt_un size);
void *rt_memory_move(const void *source, void *destination, rt_un size);
void *rt_memory_set(void *area, rt_n32 value, rt_un size);

void *rt_memory_zero(void *area, rt_un size);
void rt_memory_swap(void *area1, void *area2, rt_un size);

/**
 * Fill <tt>AREA</tt> with given <tt>VALUE</tt> as character.<br>
 * Can be used to initialize a string with a specific character like space.<br>
 * The size of the character is 8 ou 16 bits depending on the platform.
 */
#ifdef RT_DEFINE_WINDOWS
#define RT_MEMORY_SET_CHAR(AREA, VALUE, SIZE) rt_memory_set_char16(AREA, VALUE, SIZE)
#else
#define RT_MEMORY_SET_CHAR(AREA, VALUE, SIZE) RT_MEMORY_SET(AREA, VALUE, SIZE)
#endif

void *rt_memory_set_char16(void *area, rt_n32 value, rt_un size);

/**
 * Copy from source to destination, swapping bytes.
 *
 * <p>
 * Useful to convert 16 bits words between big endian and little endian.
 * </p>
 *
 * @param size Size in 16 bits words.
 * @return The destination in all cases.
 */
rt_un16 *rt_memory_swap_bytes16(const rt_un16 *source, rt_un16 *destination, rt_un size);

/**
 * Copy from source to destination, swapping bytes.
 *
 * <p>
 * Useful to convert 32 bits words between big endian and little endian.
 * </p>
 *
 * @param size Size in 32 bits words.
 * @return The destination in all cases.
 */
rt_un32 *rt_memory_swap_bytes32(const rt_un32 *source, rt_un32 *destination, rt_un size);

#endif /* RT_MEMORY_H */
