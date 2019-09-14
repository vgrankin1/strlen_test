
#include <iostream>
#include <string>
#include <vector>
#include <chrono>


#include <nmmintrin.h>

inline size_t strlen_algo(const char* str)
{
	size_t length = 0;
	while (*str++)
		length++;
	return length;
}
/*
_SIDD_UBYTE_OPS                // unsigned 8-bit characters
_SIDD_UWORD_OPS                // unsigned 16-bit characters
_SIDD_SBYTE_OPS                // signed 8-bit characters
_SIDD_SWORD_OPS                // signed 16-bit characters
_SIDD_CMP_EQUAL_ANY            // compare equal any
_SIDD_CMP_RANGES               // compare ranges
_SIDD_CMP_EQUAL_EACH           // compare equal each
_SIDD_CMP_EQUAL_ORDERED        // compare equal ordered
_SIDD_NEGATIVE_POLARITY        // negate results
_SIDD_MASKED_NEGATIVE_POLARITY // negate results only before end of string
_SIDD_LEAST_SIGNIFICANT        // index only: return last significant bit
_SIDD_MOST_SIGNIFICANT         // index only: return most significant bit
_SIDD_BIT_MASK                 // mask only: return bit mask
_SIDD_UNIT_MASK                // mask only: return byte/word mask*/
inline size_t strlen_sse4(const char* str)
{
	size_t length = 0;
	int res;
	//align to 16 bytes
	while (((size_t(str+length)) & (sizeof(__m128i) - 1)) != 0)
	{
		if (str[length] == 0)
			return length;
		length++;
	}
	__m128i z128 = _mm_setzero_si128();
	for(; ; length += 16)
	{
		__m128i data = _mm_load_si128((__m128i*)(str + length));
		if ((res = _mm_cmpistri(z128, data, _SIDD_CMP_EQUAL_EACH)) != 16)
			break;
	}
	while (str[length])
		length++;
	return length;
}

#define _DISABLE_ASM_BSF
//https://www.strchr.com/sse2_optimised_strlen
#ifndef WORDS_BIGENDIAN
#if 0
#elif 0
#else
static inline int count_bits_to_0(unsigned int x) // counting trailing zeroes, by Nazo, post: 2009/07/20 03:40
{                                                 // this is current winner for speed
	static const unsigned char table[256] =
	{
		7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
		4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	};
	if ((unsigned char)x)
		return table[(unsigned char)x];
	return table[x >> 8] + 8; // t[x / 256] + 8
}
#endif
#else
#if 0
static inline int count_bits_to_0(unsigned int x)  // counting trailing zeroes
{
	register int i = 0;
	if (!(x & (1 << 15))) i++;
	else return i;
	if (!(x & (1 << 14))) i++;
	else return i;
	if (!(x & (1 << 13))) i++;
	else return i;
	if (!(x & (1 << 12))) i++;
	else return i;
	if (!(x & (1 << 11))) i++;
	else return i;
	if (!(x & (1 << 10))) i++;
	else return i;
	if (!(x & (1 << 9))) i++;
	else return i;
	if (!(x & (1 << 8))) i++;
	else return i;
	if (!(x & (1 << 7))) i++;
	else return i;
	if (!(x & (1 << 6))) i++;
	else return i;
	if (!(x & (1 << 5))) i++;
	else return i;
	if (!(x & (1 << 4))) i++;
	else return i;
	if (!(x & (1 << 3))) i++;
	else return i;
	if (!(x & (1 << 2))) i++;
	else return i;
	if (!(x & (1 << 1))) i++;
	else return i;
	if (!(x & (1 << 0))) i++;
	return i;
}
#else
static inline int count_bits_to_0(unsigned int x)  // counting trailing zeroes
{
	// http://www.hackersdelight.org/: nlz1() shortened for 16-bit mask
	register int n = 0;
	if (x <= 0x000000FFU) { n = n + 8; x = x << 8; }
	if (x <= 0x00000FFFU) { n = n + 4; x = x << 4; }
	if (x <= 0x00003FFFU) { n = n + 2; x = x << 2; }
	if (x <= 0x00007FFFU) { n = n + 1; }
	return n;
}
#endif
#endif
size_t strlen2(const char* str)
{
	register size_t len = 0;
	// align to 16 bytes
	while ((((intptr_t)str) & (sizeof(__m128i) - 1)) != 0)
	{
		if (*str++ == 0)
			return len;
		++len;
	}
	// search for 0
	__m128i xmm0 = _mm_setzero_si128();
	__m128i xmm1;
	int mask = 0;
	for (;;)
	{
		xmm1 = _mm_load_si128((__m128i*)str);
		xmm1 = _mm_cmpeq_epi8(xmm1, xmm0);
		if ((mask = _mm_movemask_epi8(xmm1)) != 0)
		{
			// got 0 somewhere within 16 bytes in xmm1, or within 16 bits in mask
			// find index of first set bit

#ifndef _DISABLE_ASM_BSF // define it to disable ASM
#if (_MSC_VER >= 1300)   // make sure <intrin.h> is included
			unsigned long pos;
			_BitScanForward(&pos, mask);
			len += (size_t)pos;
#elif defined(_MSC_VER)  // earlier MSVC's do not have _BitScanForward, use inline asm
			__asm bsf edx, mask; edx = bsf(mask)
			__asm add edx, len; edx += len
			__asm mov len, edx; len = edx
#elif ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 4))) // modern GCC has built-in __builtin_ctz
			len += __builtin_ctz(mask);
#elif defined(__GNUC__) // older GCC shall use inline asm
			unsigned int pos;
			asm("bsf %1, %0" : "=r" (pos) : "rm" (mask));
			len += (size_t)pos;
#else                    // none of choices exist, use local BSF implementation
			len += count_bits_to_0(mask);
#endif
#else
			len += count_bits_to_0(mask);
#endif

			break;
		}
		str += sizeof(__m128i);
		len += sizeof(__m128i);
	}
	return len;
}


int main()
{
	std::vector<std::string> vstr;
	const int str_num = 1024;
	const int str_size = 1024;

	size_t len_result = 0;

	srand(0);


	for (int i = 0; i < str_num; i++)
	{
		std::string str1;
		for (int j = 0; j < str_size; j++)
		{
			str1.push_back('0' + rand() % 78);
		}
		vstr.push_back(std::move(str1));
	}
	
	/*
	len_result = strlen_sse4("abcdefghij\0klmnopqrstu1234567890");
	len_result = strlen_sse4("123456789101112656g");
	*/


	auto strlen_func = strlen;
	//auto strlen_func = strlen_algo;
	//auto strlen_func = strlen_sse4;
	//auto strlen_func = strlen2;

	auto time_std = std::chrono::steady_clock::now();
	//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	
	for (int k = 0; k < 10*1000; k++)
	{
		for (int i = 0; i < str_num; i++)
		{
			const char* str_for_test = vstr[i].c_str();
			len_result += strlen_func(str_for_test);
			//len_result += strlen(str_for_test);
		}
		for (int i = 0; i < str_num; i++)
		{
			const char* str_for_test = vstr[i].c_str();
			len_result -= strlen_func(str_for_test);
			//len_result -= strlen(str_for_test);
		}
	}
	auto finish = std::chrono::steady_clock::now();
	double elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(finish - time_std).count();

	std::cout << "\n" << len_result;
	std::cout << "\n\nTime: " << elapsed_seconds;

	
	return 0;
}

