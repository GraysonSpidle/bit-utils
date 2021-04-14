/* BitUtils.h
* Honestly this was just a programming exercise for me. I'm in no way claiming this is cutting edge stuff.
* 
* Note: IF YOU GET A HEAP CORRUPTION ERROR THAT MEANS YOU PROBABLY FUCKED UP THE SIZES/INDICES.
* 
* Author: Grayson Spidle
*/

#ifndef BITUTILS_H
#define BITUTILS_H

#include <cstdint>
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
#undef __STDC_WANT_LIB_EXT1__
#include <climits>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <string>
#include <sstream>
#include <functional>

#include <iostream>

#ifdef _WIN64
#define _BITUTILS_ENVIRONMENT 64
#else
#define _BITUTILS_ENVIRONMENT 32
#endif

// Gets a memory pointer to the page (in reference to "paging memory") given the bit index
#define _BITUTILS_GET_PAGE(arr_ptr, i) ((char*)(arr_ptr) + (std::size_t)((i) / CHAR_BIT))

// Preprocessor macro. Checks if the current machine is big endian or little endian. 
#define _BITUTILS_IS_LITTLE_ENDIAN (1 << 1) > 1

namespace BitUtils {
	/* Calculates the size (in bytes) of a memory block that is size n (in bits)
	* 
	Parameters
	* n: the size of the memory block in bits. This does not have to be a log of 2.
	* 
	Returns the size of the memory block in bytes.
	*/
	std::size_t size(std::size_t n);

	/* Allocates a memory block on the heap that guarantees that it is at least of size n (in bits).
	* The memory block is comprised entirely of 0s.
	* You will have to use free() on the memory block when you're done, if you don't want memory leaks.
	* 
	Parameters
	* n: the size of the memory block in bits. This does not have to be a log of 2.
	* 
	Returns a pointer to the memory block.
	*/
	void* create(std::size_t n);

	/* Gets the selected bit's state.
	* 
	Parameters
	* arr_ptr: the pointer to the memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	* i: the index of the bit you want to get.
	* 
	Returns true if the bit is set and false if it isn't.
	*/
	bool get(const void* const arr_ptr, std::size_t n, std::size_t i);

	/* Flips the selected bit from true to false or vice versa.
	* 
	Parameters
	* arr_ptr: the pointer to the memory block.
	* n: the size of the memory block or BitArray in bits. Doesn't have to be a log of 2.
	* i: the index of the bit you want to flip.
	*/
	void flip(void* const arr_ptr, std::size_t n, std::size_t i);

	/* Sets the selected bit to reflect the given boolean.
	* 
	Parameters
	* arr_ptr: the pointer to the memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	* i: the index of the bit you want to set.
	* b: the state you want to set the bit to.
	* 
	It is worth noting that setting a bit to false is slower than setting it to true. The flip() function is faster than this.
	*/
	void set(void* const arr_ptr, std::size_t n, std::size_t i, bool b);

	/* Fills all of the memory block with 1s or 0s. 
	* 
	Parameters
	* arr_ptr: the pointer to the memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	* b: the state you want to set all the bits to.
	*/
	void fill(void* const arr_ptr, std::size_t n, bool b);

	/* Safely fills the memory block with 1s or 0s. Safely means that if you have a memory block with a size of 2 bytes, and you fill 15 bits with 1, then it'll only fill those 15 bits.
	*
	Parameters
	* arr_ptr: the pointer to the memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	* b: the state you want to set all the bits to.
	*/
	void fill_s(void* const arr_ptr, std::size_t n, bool b);

	/* Does the & bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left & right
	* 
	Parameters
	* left: the pointer to the left memory block.
	* right: the pointer to the right memory block.
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* n: the size of all 3 memory blocks in bits. Doesn't have to be a log of 2.
	*/
	void bitwise_and(const void* const left, const void* const right, void* const dst, std::size_t n);

	/* Does the | bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left | right
	*
	Parameters
	* left: the pointer to the left memory block.
	* right: the pointer to the right memory block.
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* n: the size of all 3 memory blocks in bits. Doesn't have to be a log of 2.
	*/
	void bitwise_or(const void* const left, const void* const right, void* const dst, std::size_t n);

	/* Does the ^ bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left ^ right
	*
	Parameters
	* left: the pointer to the left memory block.
	* right: the pointer to the right memory block.
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* n: the size of all 3 memory blocks in bits. Doesn't have to be a log of 2.
	*/
	void bitwise_xor(const void* const left, const void* const right, void* const dst, std::size_t n);

	/* Does the ~ bitwise operation on a memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = ~arr
	*
	Parameters
	* arr_ptr: the pointer to the memory block.
	* dst: the pointer to the destination memory block. This can be the same as arr_ptr.
	* n: the size of all 2 memory blocks in bits. Doesn't have to be a log of 2.
	*/
	void bitwise_not(const void* const arr_ptr, void* const dst, std::size_t n);

	/* Evaluates the memory block as a bool.
	This is similar to casting an int to a bool
	*
	Parameters
	* arr_ptr: the pointer to the memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	Returns false if all the bits are 0 or returns true otherwise.
	*/
	bool bool_op(const void* const arr_ptr, std::size_t n);

	/* Safely evaluates the memory block as a bool. Safely means that it only evaluates the bits specified by n.
	This is similar to casting an int to a bool
	*
	Parameters
	* arr_ptr: the pointer to the memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	Returns false if the specified bits are all 0 or returns true otherwise.
	*/
	bool bool_op_s(const void* const arr_ptr, std::size_t n);

	/* Copies memory from one memory block to another.
	* 
	Parameters
	* dst: pointer to the desination memory block.
	* dst_n: the size (in bits) of the destination memory block.
	* src: pointer to the source memory block.
	* src_n: the size (in bits) of the source memory block.
	*/
	void copy(void* const dst, std::size_t dst_n, const void* const src, std::size_t src_n);

	/* Performs the == operation on two memory blocks.
	* 
	Parameters
	* left: pointer to the left memory block.
	* left_n: the size (in bits) of the left memory block. This does not have to be the same as right_n.
	* right: pointer to the right memory block.
	* right_n: the size (in bits) of the right memory block. This does not have to be the same as left_n.
	* 
	Returns true if all pertinent bits in both blocks are the same or false otherwise.
	*/
	bool equals(const void* const left, const std::size_t left_n, const void* const right, const std::size_t right_n);

	/* Puts a string representation of the binary of the memory block into the supplied buffer.
	Bit 0 will always be the left most number regardless if the machine is big or little endian.
	* 
	Parameters
	* arr_ptr: pointer to the memory block.
	* arr_n: the size (in bits) of the memory block. Does nothing if this is 0.
	* buf: the char buffer to put the string into. This should be arr_n + 1.
	* 
	Note that this does null terminate the string by any means necessary (ie it will overwrite a bit so it can null terminate the string).
	*/
	void str(const void* const arr_ptr, std::size_t arr_n, char* const buf);

	/* Puts a wide string representation of the binary of the memory block into the supplied buffer.
	Bit 0 will always be the left most number regardless if the machine is big or little endian.
	*
	Parameters
	* arr_ptr: pointer to the memory block.
	* arr_n: the size (in bits) of the memory block. Does nothing if this is 0.
	* buf: the wide char buffer to put the string into. This should be arr_n + 1.
	* buf_n: the size (in wide chars) of the buffer. Does nothing if this is 0.
	*
	Note that this does null terminate the string by any means necessary (ie it will overwrite a bit so it can null terminate the string).
	*/
	void wstr(const void* const arr, std::size_t arr_n, wchar_t* const buf, std::size_t buf_n);

	/* Puts a string representation of the binary of the memory block into the supplied array.
	Bit 0 will always be the left most number regardless if the machine is big or little endian.
	* 
	Template Args
	* n: the size (in bits) of the memory block.
	*
	Parameters
	* arr_ptr: pointer to the memory block.
	* buf: the char array to put the string into.
	*/
	template <const std::size_t n>
	void str(const void* const arr, char buf[n]) {
		if (!n)
			return;
		for (std::size_t i = 0; i < n; i++) {
			buf[i] = BitUtils::get(arr, n, i) ? '1' : '0';
		}
	}

	/* Puts a wide string representation of the binary of the memory block into the supplied array.
	Bit 0 will always be the left most number regardless if the machine is big or little endian.
	*
	Template Args
	* n: the size (in bits) of the memory block.
	*
	Parameters
	* arr_ptr: pointer to the memory block.
	* buf: the wide char array to put the string into.
	*/
	template <const std::size_t n>
	void wstr(const void* const arr, wchar_t buf[n]) {
		if (!n)
			return;
		for (std::size_t i = 0; i < n; i++) {
			buf[i] = BitUtils::get(arr, n, i) ? L'1' : L'0';
		}
	}

	/* Puts a string representation of the binary of the memory block into the supplied output stream.
	Bit 0 will always be the left most number regardless if the machine is big or little endian.
	*
	Parameters
	* arr_ptr: pointer to the memory block.
	* arr_n: the size (in bits) of the memory block. Does nothing if this is 0.
	* os: the output stream to put the string into.
	*/
	void str(const void* const arr_ptr, std::size_t arr_n, std::ostream& os);

	/* Puts a wide string representation of the binary of the memory block into the supplied wide output stream.
	Bit 0 will always be the left most number regardless if the machine is big or little endian.
	*
	Parameters
	* arr_ptr: pointer to the memory block.
	* arr_n: the size (in bits) of the memory block. Does nothing if this is 0.
	* wos: the wide output stream to put the string into.
	*/
	void wstr(const void* const arr_ptr, std::size_t arr_n, std::wostream& wos);

	/* For each function that iterates over pages in a memory block. Pages (in reference to "memory paging") are defined by the user.
	For scenarios where it is impossible to evenly divide up the memory block into pages, it will just repeat some of the memory at the end.
	* 
	Template Args
	* PageType: an arithmetic type that will be used for the pages.
	* 
	Parameters
	* begin: pointer to the beginning of the memory block.
	* end: pointer to the end of the memory block.
	* func: pointer to a function that will be used on every page.
	* 
	Note that a reverse iteration can be achieved if begin > end.
	*/
	template <class PageType, typename = std::enable_if_t < std::is_arithmetic_v<PageType>, bool > >
	void for_each_page(void* const begin, void* const end, void (*func)(PageType* const pP)) {
		bool reverse = begin > end;
		for (PageType* it = (PageType*)(reverse ? end : begin); (reverse ? it > begin : it < end); it += (reverse ? -1 : 1) * sizeof(PageType)) {
			(*func)(it);
		}
		if (reverse)
			(*func)((PageType*)begin);
	}

	/* For each function that iterates over pages in a memory block. Pages (in reference to "memory paging") are defined by the user.
	For scenarios where it is impossible to evenly divide up the memory block into pages, it will just repeat some of the memory at the end.
	*
	Template Args
	* PageType: an arithmetic type that will be used for the pages.
	*
	Parameters
	* begin: pointer to the beginning of the memory block.
	* end: pointer to the end of the memory block.
	* func: reference to a function that will be used on every page.
	*
	Note that a reverse iteration can be achieved if begin > end.
	*/
	template <class PageType, typename = std::enable_if_t < std::is_arithmetic_v<PageType>, bool > >
	void for_each_page(void* const begin, void* const end, std::function<void(PageType*)>& func) {
		bool reverse = begin > end;
		for (PageType* it = (PageType*)(reverse ? end : begin); (reverse ? it > begin : it < end); it += (reverse ? -1 : 1) * sizeof(PageType)) {
			func(it);
		}
		if (reverse)
			func((PageType*)begin);
	}

	/* For each function that iterates over each byte in a memory block.
	*
	Parameters
	* begin: pointer to the beginning of the memory block.
	* end: pointer to the end of the memory block.
	* func: pointer to a function that will be used on every byte.
	*
	Note that a reverse iteration can be achieved if begin > end.
	*/
	void for_each_byte(void* const begin, void* const end, void (*func)(char* pC));

	/* For each function that iterates over each byte in a memory block.
	*
	Parameters
	* begin: pointer to the beginning of the memory block.
	* end: pointer to the end of the memory block.
	* func: reference to a function that will be used on every byte.
	*
	Note that a reverse iteration can be achieved if begin > end.
	*/
	void for_each_byte(void* const begin, void* const end, std::function<void(char*)>& func);

	/* For each function that iterates over each bit in a memory block.
	*
	Parameters
	* begin: pointer to the beginning of the memory block.
	* end: pointer to the end of the memory block.
	* func: pointer to a function that will be used on every bit.
	*
	Note that a reverse iteration can be achieved if begin > end.
	*/
	void for_each_bit(void* const begin, void* const end, void (*func)(bool b));

	/* For each function that iterates over each bit in a memory block.
	*
	Parameters
	* begin: pointer to the beginning of the memory block.
	* end: pointer to the end of the memory block.
	* func: reference to a function that will be used on every bit.
	*
	Note that a reverse iteration can be achieved if begin > end.
	*/
	void for_each_bit(void* const begin, void* const end, std::function<void(bool)>& func);

	// A class that encapsulates all the other functions in a single object if that's more your speed. I'd prefer the other functions tho.
	template <const std::size_t n, typename = std::enable_if_t < (n > 0), bool >>
	class BitArray {
		void* data;
	public:

		BitArray() {
			data = calloc(BitUtils::size(n), 1);
		}

		BitArray(const BitArray<n>& other)
			: BitArray()
		{
			copy(other);
		}

		~BitArray() {
			free(data);
		}

		void set(std::size_t i, bool b)		{ BitUtils::set(data, n, i, b); }
		bool get(std::size_t i) const		{ return BitUtils::get(data, n, i); }
		void flip(std::size_t i)			{ BitUtils::flip(data, n, i); }
		void fill(bool b)                   { BitUtils::fill(data, n, b); }
		void fill_s(bool b)                 { BitUtils::fill_s(data, n, b); }
		void copy(const BitArray<n>& other) { BitUtils::copy(data, n, *other, n); }

		std::string str() const {
			std::stringstream ss;
			for (std::size_t i = 0; i < n; i += CHAR_BIT) {
				ss.putback(get(i) ? '1' : '0');
			}
			return ss.str();
		}
		std::wstring wstr() const {
			std::wstringstream wss;
			for (std::size_t i = 0; i < n; i += CHAR_BIT) {
				wss.putback(get(i) ? L'1' : L'0');
			}
			return wss.str();
		}

		void for_each_bit(void (*func)(bool b))				 { BitUtils::for_each_bit(data, (char*)data + BitUtils::size(n), func); }
		void for_each_bit(std::function<void(bool)>& func)   { BitUtils::for_each_bit(data, (char*)data + BitUtils::size(n), func); }
		void for_each_byte(void (*func)(char* pC))			 { BitUtils::for_each_byte(data, (char*)data + BitUtils::size(n), func); }
		void for_each_byte(std::function<void(char*)>& func) { BitUtils::for_each_byte(data, (char*)data + BitUtils::size(n), func); }

		template <class PageType, typename = std::enable_if_t < std::is_arithmetic_v<PageType>, bool > >
		void for_each_page(void (*func)(PageType* pP)) { BitUtils::for_each_page(data, (char*)data + BitUtils::size(n), func); }

		template <class PageType, typename = std::enable_if_t < std::is_arithmetic_v<PageType>, bool > >
		void for_each_page(std::function<void(PageType*)>& func) { BitUtils::for_each_page(data, (char*)data + BitUtils::size(n), func); }

		void rfor_each_bit(void (*func)(bool b)) { BitUtils::for_each_bit((char*)data + BitUtils::size(n), data, func); }
		void rfor_each_bit(std::function<void(bool)>& func) { BitUtils::for_each_bit((char*)data + BitUtils::size(n), data, func); }
		void rfor_each_byte(void (*func)(char* pC)) { BitUtils::for_each_byte((char*)data + BitUtils::size(n), data, func); }
		void rfor_each_byte(std::function<void(char*)>& func) { BitUtils::for_each_byte((char*)data + BitUtils::size(n), data, func); }

		template <class PageType, typename = std::enable_if_t < std::is_arithmetic_v<PageType>, bool > >
		void rfor_each_page(void (*func)(PageType* pP)) { BitUtils::for_each_page((char*)data + BitUtils::size(n), data, func); }

		template <class PageType, typename = std::enable_if_t < std::is_arithmetic_v<PageType>, bool > >
		void rfor_each_page(std::function<void(PageType*)>& func) { BitUtils::for_each_page((char*)data + BitUtils::size(n), data, func); }

		void* const operator*() { return data; }
		const void* const operator*() const { return data; }

		void operator&=(const BitArray<n>& other) { BitUtils::bitwise_and(data, *other, data, n); }
		BitArray<n> operator&(const BitArray<n>& other) const {
			BitArray<n> output{ *this };
			output &= other;
			return output;
		}

		void operator|=(const BitArray<n>& other) { BitUtils::bitwise_or(data, *other, data, n); }
		BitArray<n> operator|(const BitArray<n>& other) const {
			BitArray<n> output{ *this };
			output |= other;
			return output;
		}

		void operator^=(const BitArray<n>& other) { BitUtils::bitwise_xor(data, *other, data, n); }
		BitArray<n> operator^(const BitArray<n>& other) const {
			BitArray<n> output{ *this };
			output |= other;
			return output;
		}

		BitArray<n> operator~() const {
			BitArray<n> output{ *this };
			BitUtils::bitwise_not(data, *output, n);
			return output;
		}

		bool operator==(const BitArray<n>& other) const { return BitUtils::equals(data, n, *other, n); }
		operator bool() const { return BitUtils::bool_op_s(data, n); }
	};
};


void* BitUtils::create(std::size_t n) {
	return calloc(BitUtils::size(n), 1);
}

std::size_t BitUtils::size(std::size_t n) {
	if (n <= CHAR_BIT)
		return 1;
	return (n / CHAR_BIT) + (n % CHAR_BIT);
}

bool BitUtils::get(const void* const arr_ptr, std::size_t n, std::size_t i) {
#if _BITUTILS_IS_LITTLE_ENDIAN
	return *_BITUTILS_GET_PAGE(arr_ptr, i) & ((std::size_t)1 << (i % CHAR_BIT));
#else
	return *_BITUTILS_GET_PAGE(arr_ptr, i) & ((std::size_t)1 >> (i % CHAR_BIT));
#endif
}

void BitUtils::flip(void* const arr_ptr, std::size_t n, std::size_t i) {
#if _BITUTILS_IS_LITTLE_ENDIAN
	*_BITUTILS_GET_PAGE(arr_ptr, i) ^= ((std::size_t)1 << (i % CHAR_BIT));
#else
	*_BITUTILS_GET_PAGE(arr_ptr, i) ^= ((std::size_t)1 >> (i % CHAR_BIT));
#endif
}

void BitUtils::set(void* const arr_ptr, std::size_t n, std::size_t i, bool b) {
	// Setting the bit to true regardless of its state
#if _BITUTILS_IS_LITTLE_ENDIAN
	* _BITUTILS_GET_PAGE(arr_ptr, i) |= ((std::size_t)1 << (i % CHAR_BIT));
#else
	* _BITUTILS_GET_PAGE(arr_ptr, i) |= ((std::size_t)1 >> (i % CHAR_BIT));
#endif
	// Flipping the bit if user wants it to be false
	if (!b)
		BitUtils::flip(arr_ptr, n % CHAR_BIT, i);
}

void BitUtils::fill(void* const arr_ptr, std::size_t n, bool b) {
	memset(arr_ptr, b ? UCHAR_MAX : 0, BitUtils::size(n));
}

void BitUtils::fill_s(void* const arr_ptr, std::size_t n, bool b) {
	if (n % CHAR_BIT == 0) {
		BitUtils::fill(arr_ptr, n, b);
		return;
	}

	char* page = (char*)arr_ptr;
	if (n > CHAR_BIT) {
		BitUtils::fill(arr_ptr, n - CHAR_BIT, b);
		page = _BITUTILS_GET_PAGE(arr_ptr, BitUtils::size(n) - 1);
	}

	for (std::size_t i = 0; i < n % CHAR_BIT; i++) {
		BitUtils::set(page, n, i, b);
	}
}

void BitUtils::copy(void* const dst, std::size_t dst_n, const void* const src, std::size_t src_n) {
#ifdef __STDC_LIB_EXT1__
	memcpy_s(dst, dst_n * CHAR_BIT, src, src_n * CHAR_BIT);
#else
	if (src == dst)
		return;
	BitUtils::fill(dst, dst_n, 0);
	BitUtils::bitwise_or(dst, src, dst, src_n < dst_n ? src_n : dst_n);
#endif
}

void BitUtils::bitwise_and(const void* const left, const void* const right, void* const dst, std::size_t n) {
	for (std::size_t i = 0; i < n; i += CHAR_BIT) {
		*_BITUTILS_GET_PAGE(dst, i) = *_BITUTILS_GET_PAGE(left, i) & *_BITUTILS_GET_PAGE(right, i);
	}
}

void BitUtils::bitwise_or(const void* const left, const void* const right, void* const dst, std::size_t n) {
	for (std::size_t i = 0; i < n; i += CHAR_BIT) {
		*_BITUTILS_GET_PAGE(dst, i) = *_BITUTILS_GET_PAGE(left, i) | *_BITUTILS_GET_PAGE(right, i);
	}
}

void BitUtils::bitwise_xor(const void* const left, const void* const right, void* const dst, std::size_t n) {
	for (std::size_t i = 0; i < n; i += CHAR_BIT) {
		*_BITUTILS_GET_PAGE(dst, i) = *_BITUTILS_GET_PAGE(left, i) ^ *_BITUTILS_GET_PAGE(right, i);
	}
}

void BitUtils::bitwise_not(const void* const arr_ptr, void* const dst, std::size_t n) {
	if (arr_ptr != dst)
		BitUtils::copy(dst, n, arr_ptr, n);
	for (std::size_t i = 0; i < n; i += CHAR_BIT) {
		*_BITUTILS_GET_PAGE(dst, i) = ~(*_BITUTILS_GET_PAGE(dst, i));
	}
}

bool BitUtils::bool_op(const void* const arr_ptr, std::size_t n) {
	for (std::size_t i = 0; i < n; i += CHAR_BIT) {
		if (*_BITUTILS_GET_PAGE(arr_ptr, i))
			return true;
	}
	return false;
}

bool BitUtils::bool_op_s(const void* const arr_ptr, std::size_t n) {
	if (n % CHAR_BIT) {
		if (BitUtils::bool_op(arr_ptr, (n - CHAR_BIT ? n - CHAR_BIT : CHAR_BIT))) // We're evaluating all the previous bytes as bools (as it is faster)
			return true;
	}

	// Now evaluating each individual bit. Slow, but necessary.
	for (std::size_t i = 0; i < n % CHAR_BIT; i++) {
		if (BitUtils::get(arr_ptr, n, i))
			return true;
	}
	return false;
}

bool BitUtils::equals(const void* const left, const std::size_t left_n, const void* const right, const std::size_t right_n) {
	if (left_n > right_n) {
		for (std::size_t i = right_n; i < left_n; i++) {
			if (BitUtils::get(right, right_n, i))
				return false;
		}
	}
	else if (left_n < right_n) {
		for (std::size_t i = left_n; i < right_n; i++) {
			if (BitUtils::get(left, left_n, i))
				return false;
		}
	}

	for (std::size_t i = 0; i < (left_n < right_n ? left_n : right_n); i++) {
		if (BitUtils::get(left, left_n, i) != BitUtils::get(right, right_n, i))
			return false;
	}
	return true;
}

void BitUtils::str(const void* const arr_ptr, std::size_t arr_n, char* const buf) {
	if (!arr_n || !strlen(buf))
		return;
	for (std::size_t i = 0; i < (arr_n < strlen(buf) ? arr_n : strlen(buf)) - 1; i++) {
		buf[i] = BitUtils::get(arr_ptr, arr_n, i) ? '1' : '0';
	}
	buf[(arr_n < strlen(buf) ? arr_n : strlen(buf))] = '\0';
}

void BitUtils::wstr(const void* const arr, std::size_t arr_n, wchar_t* const buf, std::size_t buf_n) {
	if (!arr_n || !buf_n)
		return;
	for (std::size_t i = 0; i < (arr_n < buf_n ? arr_n : buf_n) - 1; i++) {
		buf[i] = BitUtils::get(arr, arr_n, i) ? L'1' : L'0';
	}
	buf[(arr_n < buf_n ? arr_n : buf_n)] = L'\0';
}

void BitUtils::str(const void* const arr_ptr, std::size_t arr_n, std::ostream& os) {
	if (!arr_n)
		return;
	for (std::size_t i = 0; i < BitUtils::size(arr_n) * CHAR_BIT; i++) {
		os << BitUtils::get(arr_ptr, arr_n, i) ? '1' : '0';
	}
}

void BitUtils::wstr(const void* const arr_ptr, std::size_t arr_n, std::wostream& wos) {
	if (!arr_n)
		return;
	for (std::size_t i = 0; i < arr_n; i++) {
		wos << BitUtils::get(arr_ptr, arr_n, i) ? L'1' : L'0';
	}
}

void BitUtils::for_each_byte(void* const begin, void* const end, void (*func)(char* pC)) {
	BitUtils::for_each_page<char>(begin, end, func);
}

void BitUtils::for_each_byte(void* const begin, void* const end, std::function<void(char*)>& func) {
	BitUtils::for_each_page<char>(begin, end, func);
}

void BitUtils::for_each_bit(void* const begin, void* const end, void (*func)(bool b)) {
	bool reverse = begin > end;

	for (char* it = (char*)(reverse ? end : begin); it != (reverse ? begin : end); it += reverse ? -1 : 1) {
		for (char i = 0; i < CHAR_BIT; i++) {
			(*func)(BitUtils::get(it, 8, i));
		}
	}
	if (reverse) {
		for (char i = 0; i < CHAR_BIT; i++) {
			(*func)(BitUtils::get(begin, 8, i));
		}
	}
}

void BitUtils::for_each_bit(void* const begin, void* const end, std::function<void(bool)>& func) {
	bool reverse = begin > end;

	for (char* it = (char*)(reverse ? end : begin); it != (reverse ? begin : end); it += reverse ? -1 : 1) {
		for (char i = 0; i < CHAR_BIT; i++) {
			func(BitUtils::get(it, 8, i));
		}
	}
	if (reverse) {
		for (char i = 0; i < CHAR_BIT; i++) {
			func(BitUtils::get(begin, 8, i));
		}
	}
}

#undef _BITUTILS_IS_LITTLE_ENDIAN
#undef _BITUTILS_GET_PAGE
#undef _BITUTILS_ENVIRONMENT

#endif // BITUTILS_H