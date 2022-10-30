#include "layer001/rt_memory.h"

#ifdef RT_DEFINE_VC
rt_n32 rt_memory_bit_scan_forward(rt_un x)
{
	unsigned long result;
#ifdef RT_DEFINE_32
	_BitScanForward(&result, x);
#else
	_BitScanForward64(&result, x);
#endif
	return (rt_n32)result;
}
#endif

rt_n32 rt_memory_compare(const void *area1, const void *area2, rt_un size)
{
	rt_un words_count;
	const rt_un *area1_words;
	const rt_un *area2_words;
	rt_un remainder;
	const rt_uchar8 *area1_uchars;
	const rt_uchar8 *area2_uchars;
	rt_un i;
	rt_n32 ret;

	words_count = size / sizeof(rt_un);
	if (words_count) {
		area1_words = area1;
		area2_words = area2;
		for (i = 0; i < words_count; i++) {
			if (area1_words[i] != area2_words[i]) {
				remainder = size - i * sizeof(rt_un);
				area1_uchars = (rt_uchar8*)&area1_words[i];
				area2_uchars = (rt_uchar8*)&area2_words[i];
				goto handle_remainder;
			}
		}
		remainder = RT_MEMORY_MODULO_RT_UN_SIZE(size);
		area1_uchars = (rt_uchar8*)&area1_words[words_count];
		area2_uchars = (rt_uchar8*)&area2_words[words_count];
	} else {
		remainder = size;
		area1_uchars = area1;
		area2_uchars = area2;
	}

handle_remainder:
	if (size) {
		i = 0;
		while ((i < remainder) && (area1_uchars[i] == area2_uchars[i]))
			i++;
		if (i == remainder)
			i--;
		ret = area1_uchars[i] - area2_uchars[i];
	} else {
		ret = 0;
	}
	return ret;
}

void *rt_memory_copy(const void *source, void *destination, rt_un size)
{
	rt_un words_count;
	const rt_un *source_words;
	rt_un *destination_words;
	rt_un remainder;
	const rt_char8 *source_chars;
	rt_char8 *destination_chars;
	rt_un i;

	words_count = size / sizeof(rt_un);
	if (words_count)	{
		source_words = source;
		destination_words = destination;
		for (i = 0; i < words_count; i++)
			destination_words[i] = source_words[i];
		remainder = RT_MEMORY_MODULO_RT_UN_SIZE(size);
		if (remainder) {
			source_chars = (rt_char8*)&source_words[words_count];
			destination_chars = (rt_char8*)&destination_words[words_count];
			for (i = 0; i < remainder; i++)
				destination_chars[i] = source_chars[i];
		}
	} else {
		source_chars = source;
		destination_chars = destination;
		for (i = 0; i < size; i++)
			destination_chars[i] = source_chars[i];
	}

	return destination;
}

void *rt_memory_move(const void *source, void *destination, rt_un size)
{
	rt_un words_count;
	const rt_un *source_words;
	rt_un *destination_words;
	rt_un remainder;
	const rt_char8 *source_chars;
	rt_char8 *destination_chars;
	rt_un i;

	words_count = size / sizeof(rt_un);
	if (destination > source) {
		if (words_count) {
			source_words = (rt_un*)&((rt_char8*)source)[size];
			destination_words = (rt_un*)&((rt_char8*)destination)[size];
			for (i = 1; i <= words_count; i++)
				destination_words[-(rt_n)i] = source_words[-(rt_n)i];
			remainder = RT_MEMORY_MODULO_RT_UN_SIZE(size);
			source_chars = (rt_char8*)&source_words[-(rt_n)words_count];
			destination_chars = (rt_char8*)&destination_words[-(rt_n)words_count];;
		} else {
			remainder = size;
			source_chars = &((rt_char8*)source)[size];
			destination_chars = &((rt_char8*)destination)[size];
		}
		for (i = 1; i <= remainder; i++)
			destination_chars[-(rt_n)i] = source_chars[-(rt_n)i];
	} else {
		/* No overwriting possible, same code as rt_memory__copy. */
		if (words_count) {
			source_words = source;
			destination_words = destination;
			for (i = 0; i < words_count; i++)
				destination_words[i] = source_words[i];
			remainder = RT_MEMORY_MODULO_RT_UN_SIZE(size);
			if (remainder) {
				source_chars = (rt_char8*)&source_words[words_count];
				destination_chars = (rt_char8*)&destination_words[words_count];
				for (i = 0; i < remainder; i++)
					destination_chars[i] = source_chars[i];
			}
		} else {
			source_chars = source;
			destination_chars = destination;
			for (i = 0; i < size; i++)
				destination_chars[i] = source_chars[i];
		}
	}

	return destination;
}

void *rt_memory_set(void *area, rt_n32 value, rt_un size)
{
	rt_un words_count;
	rt_n *area_words;
	rt_n word;
	rt_un remainder;
	rt_char8 *area_chars;
	rt_un i;

	words_count = size / sizeof(rt_n);
	if (words_count) {
		if (value) {
#ifdef RT_DEFINE_64
			word = 0x0101010101010101 * (rt_uchar8)value;
#else
			word = 0x01010101 * (rt_uchar8)value;
#endif
		} else
			word = 0;

		area_words = area;
		for (i = 0; i < words_count; i++)
			area_words[i] = word;
		remainder = RT_MEMORY_MODULO_RT_UN_SIZE(size);
		if (remainder) {
			area_chars = (rt_char8*)&area_words[words_count];
			for (i = 0; i < remainder; i++)
				area_chars[i] = (rt_char8)value;
		}
	} else {
		area_chars = area;
		for (i = 0; i < size; i++)
			area_chars[i] = (rt_char8)value;
	}

	return area;
}

void *rt_memory_zero(void *area, rt_un size)
{
	rt_un words_count;
	rt_n *area_words;
	rt_un remainder;
	rt_char8 *area_chars;
	rt_un i;

	words_count = size / sizeof(rt_n);
	if (words_count) {
		area_words = area;
		for (i = 0; i < words_count; i++)
			area_words[i] = 0;
		remainder = RT_MEMORY_MODULO_RT_UN_SIZE(size);
		if (remainder) {
			area_chars = (rt_char8*)&area_words[words_count];
			for (i = 0; i < remainder; i++)
				area_chars[i] = 0;
		}
	} else {
		area_chars = area;
		for (i = 0; i < size; i++)
			area_chars[i] = 0;
	}

	return area;
}

void rt_memory_swap(void *area1, void *area2, rt_un size)
{
	rt_un words_count;
	rt_un *area1_words;
	rt_un *area2_words;
	rt_un temp_word;
	rt_un remainder;
	rt_uchar8 *area1_chars;
	rt_uchar8 *area2_chars;
	rt_uchar8 temp_char;
	rt_un i;

	words_count = size / sizeof(rt_un);
	if (words_count) {
		area1_words = area1;
		area2_words = area2;
		for (i = 0; i < words_count; i++) {
			temp_word = area1_words[i];
			area1_words[i] = area2_words[i];
			area2_words[i] = temp_word;
		}
		remainder = RT_MEMORY_MODULO_RT_UN_SIZE(size);
		if (remainder) {
			area1_chars = (rt_uchar8*)&area1_words[words_count];
			area2_chars = (rt_uchar8*)&area2_words[words_count];
			for (i = 0; i < remainder; i++) {
				temp_char = area1_chars[i];
				area1_chars[i] = area2_chars[i];
				area2_chars[i] = temp_char;
			}
		}
	} else {
		area1_chars = area1;
		area2_chars = area2;
		for (i = 0; i < size; i++) {
				temp_char = area1_chars[i];
				area1_chars[i] = area2_chars[i];
				area2_chars[i] = temp_char;
		}
	}
}

void *rt_memory_set_char16(void *area, rt_n32 value, rt_un size)
{
	rt_un words_count;
	rt_n *area_words;
	rt_n word;
	rt_un remainder;
	rt_un16 *area_chars;
	rt_un i;

	words_count = (size * sizeof(rt_un16)) / sizeof(rt_n);
	if (words_count) {
		if (value) {
#ifdef RT_DEFINE_64
			word = 0x0001000100010001 * (rt_un16)value;
#else
			word = 0x00010001 * (rt_un16)value;
#endif
		} else
			word = 0;

		area_words = area;
		for (i = 0; i < words_count; i++)
			area_words[i] = word;
		remainder = RT_MEMORY_MODULO_RT_UN_SIZE(size * sizeof(rt_un16)) / 2;
		if (remainder) {
			area_chars = (rt_un16*)&area_words[words_count];
			for (i = 0; i < remainder; i++)
				area_chars[i] = (rt_un16)value;
		}
	} else {
		area_chars = area;
		for (i = 0; i < size; i++)
			area_chars[i] = (rt_un16)value;
	}

	return area;
}

rt_un16 *rt_memory_swap_bytes16(const rt_un16 *source, rt_un16 *destination, rt_un size)
{
	rt_un i;

	for (i = 0; i < size; i++)
		destination[i] = RT_MEMORY_SWAP_BYTES16(source[i]);

	return destination;
}

rt_un32 *rt_memory_swap_bytes32(const rt_un32 *source, rt_un32 *destination, rt_un size)
{
	rt_un i;

	for (i = 0; i < size; i++)
		destination[i] = RT_MEMORY_SWAP_BYTES32(source[i]);

	return destination;
}
