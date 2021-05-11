/* BitUtils.h
* Author: Grayson Spidle
* 
* This file defines the BitUtils namespace which houses functions for manipulating bits in memory blocks
* as if they were arrays. I made this as a programming exercise so I could learn how to use the C api
* and learn how memory management works at a lower level. I'm not claiming this is cutting edge stuff
* and I'm sure it has massive inefficiencies. I tried to treat it as if it were a piece of professional
* work just as practice for when I actually do programming professionally.
* 
* Targeted the C++11 language standard.
* 
* You might have to change some things if you aren't using Visual C++. I tried to do some preprocessor
* stuff to make it more flexible, but I haven't tested this on Linux.
*/

#ifndef BITUTILS_H
#define BITUTILS_H

#include <cstdint>
#include <string.h>
#include <climits>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <iostream>
#include <sstream>
#include <functional>

namespace BitUtils {
	/* Calculates the size (in bytes) of a memory block that is size n (in bits)
	*
	Parameters
	* n: the size of the memory block in bits. This does not have to be a log of 2.
	*
	Returns the size of the memory block in bytes.
	*/
	std::size_t size(const std::size_t n);

	/* Allocates a memory block on the heap that guarantees that it is at least of size n (in bits).
	* The memory block is comprised entirely of 0s.
	* You will have to use free() on the memory block when you're done, if you don't want memory leaks.
	*
	Parameters
	* n: the size of the memory block in bits. This does not have to be a log of 2.
	*
	Returns a pointer to the memory block.
	*/
	void* create(const std::size_t n);

	/* Gets the selected bit's state.
	*
	Parameters
	* arr_ptr: the pointer to the memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	* i: the index of the bit you want to get.
	*
	Returns true if the bit is set and false if it isn't.
	*/
	bool get(const void* const src,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit,
		const std::size_t i
	);

	bool get(const void* const src,
		const std::size_t n,
		const std::size_t i) { return get(src, n, 0, n, i); }

	/* Flips the selected bit from true to false or vice versa.
	*
	Parameters
	* arr_ptr: the pointer to the memory block.
	* n: the size of the memory block or BitArray in bits. Doesn't have to be a log of 2.
	* i: the index of the bit you want to flip.
	*/
	void flip(void* const src,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit,
		const std::size_t i);

	void flip(void* const src,
		const std::size_t n,
		const std::size_t i) { flip(src, n, 0, n, i); }

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
	void set(void* const src,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit,
		const std::size_t i,
		const bool b);

	void set(void* const src,
		const std::size_t n,
		const std::size_t i,
		const bool b) { set(src, n, 0, n, i, b); }

	/* Fills all of the memory block with 1s or 0s.
	*
	Parameters
	* arr_ptr: the pointer to the memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	* b: the state you want to set all the bits to.
	*/
	void fill(void* const src,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit,
		const bool b);

	void fill(void* const src,
		const std::size_t n,
		const bool b) { fill(src, n, 0, n, b); }

	/* Safely fills the memory block with 1s or 0s. Safely means that if you have a memory block with a size of 2 bytes, and you fill 15 bits with 1, then it'll only fill those 15 bits.
	*
	Parameters
	* arr_ptr: the pointer to the memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	* b: the state you want to set all the bits to.
	*/
	void fill_s(void* const src,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit,
		const bool b);

	void fill_s(void* const src,
		const std::size_t n,
		const bool b) { fill_s(src, n, 0, n, b); }

	/* Does the & bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left & right
	*
	Parameters
	* left: the pointer to the left memory block.
	* right: the pointer to the right memory block.
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* n: the size of all 3 memory blocks in bits. Doesn't have to be a log of 2.
	*/
	void bitwise_and(const void* const left,
		const std::size_t left_n,
		const std::size_t left_start_bit,
		const std::size_t left_end_bit,
		const void* const right,
		const std::size_t right_n,
		const std::size_t right_start_bit,
		const std::size_t right_end_bit,
		void* const dst,
		const std::size_t dst_n,
		const std::size_t dst_start_bit,
		const std::size_t dst_end_bit);

	void bitwise_and(const void* const left,
		const std::size_t left_n,
		const void* const right,
		const std::size_t right_n,
		void* const dst,
		const std::size_t dst_n) { bitwise_and(left, left_n, 0, left_n, right, right_n, 0, right_n, dst, dst_n, 0, dst_n); }

	/* Does the | bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left | right
	*
	Parameters
	* left: the pointer to the left memory block.
	* right: the pointer to the right memory block.
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* n: the size of all 3 memory blocks in bits. Doesn't have to be a log of 2.
	*/
	void bitwise_or(const void* const left,
		const std::size_t left_n,
		const std::size_t left_start_bit,
		const std::size_t left_end_bit,
		const void* const right,
		const std::size_t right_n,
		const std::size_t right_start_bit,
		const std::size_t right_end_bit,
		void* const dst,
		const std::size_t dst_n,
		const std::size_t dst_start_bit,
		const std::size_t dst_end_bit);

	void bitwise_or(const void* const left,
		const std::size_t left_n,
		const void* const right,
		const std::size_t right_n,
		void* const dst,
		const std::size_t dst_n) { bitwise_or(left, left_n, 0, left_n, right, right_n, 0, right_n, dst, dst_n, 0, dst_n); }

	/* Does the ^ bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left ^ right
	*
	Parameters
	* left: the pointer to the left memory block.
	* right: the pointer to the right memory block.
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* n: the size of all 3 memory blocks in bits. Doesn't have to be a log of 2.
	*/
	void bitwise_xor(const void* const left,
		const std::size_t left_n,
		const std::size_t left_start_bit,
		const std::size_t left_end_bit,
		const void* const right,
		const std::size_t right_n,
		const std::size_t right_start_bit,
		const std::size_t right_end_bit,
		void* const dst,
		const std::size_t dst_n,
		const std::size_t dst_start_bit,
		const std::size_t dst_end_bit);

	void bitwise_xor(const void* const left,
		const std::size_t left_n,
		const void* const right,
		const std::size_t right_n,
		void* const dst,
		const std::size_t dst_n) { bitwise_xor(left, left_n, 0, left_n, right, right_n, 0, right_n, dst, dst_n, 0, dst_n); }

	/* Does the ~ bitwise operation on a memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = ~arr
	*
	Parameters
	* arr_ptr: the pointer to the memory block.
	* dst: the pointer to the destination memory block. This can be the same as arr_ptr.
	* n: the size of all 2 memory blocks in bits. Doesn't have to be a log of 2.
	*/
	void bitwise_not(const void* const src,
		const std::size_t src_n,
		const std::size_t src_start_bit,
		const std::size_t src_end_bit,
		void* const dst,
		const std::size_t dst_n,
		const std::size_t dst_start_bit,
		const std::size_t dst_end_bit);

	void bitwise_not(const void* const src,
		const std::size_t src_n,
		void* const dst,
		const std::size_t dst_n) { bitwise_not(src, src_n, 0, src_n, dst, dst_n, 0, dst_n); }

	void bitwise_not(void* const src,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit) { bitwise_not(src, n, start_bit, end_bit, src, n, start_bit, end_bit); }

	void bitwise_not(void* const src,
		const std::size_t n) { bitwise_not(src, n, 0, n); }

	/* Evaluates the memory block as a bool.
	This is similar to casting an int to a bool
	*
	Parameters
	* arr_ptr: the pointer to the memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	Returns false if all the bits are 0 or returns true otherwise.
	*/
	bool bool_op(const void* const src,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit);

	bool bool_op(const void* const src,
		const std::size_t n) { return bool_op(src, n, 0, n); }

	/* Safely evaluates the memory block as a bool. Safely means that it only evaluates the bits specified by n.
	This is similar to casting an int to a bool
	*
	Parameters
	* arr_ptr: the pointer to the memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	Returns false if the specified bits are all 0 or returns true otherwise.
	*/
	bool bool_op_s(const void* const src,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit);

	bool bool_op_s(const void* const src,
		const std::size_t n) { return bool_op_s(src, n, 0, n); }

	/* Copies memory from one memory block to another.
	* 
	Parameters
	* dst: pointer to the desination memory block.
	* dst_n: the size (in bits) of the destination memory block.
	* src: pointer to the source memory block.
	* src_n: the size (in bits) of the source memory block.
	*/
	void copy(const void* const src,
		const std::size_t src_n,
		const std::size_t src_start_bit,
		const std::size_t src_end_bit,
		void* const dst,
		const std::size_t dst_n,
		const std::size_t dst_start_bit,
		const std::size_t dst_end_bit);

	void copy(const void* const src,
		void* const dst,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit) { copy(src, n, start_bit, end_bit, dst, n, start_bit, end_bit); }

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
	bool equals(const void* const left,
		const std::size_t left_n,
		const std::size_t left_start_bit,
		const std::size_t left_end_bit,
		const void* const right,
		const std::size_t right_n,
		const std::size_t right_start_bit,
		const std::size_t right_end_bit);

	bool equals(const void* left,
		const void* const right,
		const std::size_t n) { equals(left, n, 0, n, right, n, 0, n); }

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
	void str(const void* const src, 
		const std::size_t src_n,
		const std::size_t src_start_bit,
		const std::size_t src_end_bit,
		char* const buf);

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
	void wstr(const void* const src,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit,
		wchar_t* const buf,
		const std::size_t buf_n);

	/* Puts a string representation of the binary of the memory block into the supplied output stream.
	Bit 0 will always be the left most number regardless if the machine is big or little endian.
	*
	Parameters
	* arr_ptr: pointer to the memory block.
	* arr_n: the size (in bits) of the memory block. Does nothing if this is 0.
	* os: the output stream to put the string into.
	*/
	void str(const void* const src,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit,
		std::ostream& os);

	/* Puts a wide string representation of the binary of the memory block into the supplied wide output stream.
	Bit 0 will always be the left most number regardless if the machine is big or little endian.
	*
	Parameters
	* arr_ptr: pointer to the memory block.
	* arr_n: the size (in bits) of the memory block. Does nothing if this is 0.
	* wos: the wide output stream to put the string into.
	*/
	void wstr(const void* const src,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit,
		std::wostream& wos);

	std::string str(const void* const src,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit
	) {
		std::stringstream ss;
		BitUtils::str(src, n, start_bit, end_bit, ss);
		return ss.str();
	}

	std::wstring wstr(const void* const src,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit
	) {
		std::wstringstream wss;
		BitUtils::wstr(src, n, start_bit, end_bit, wss);
		return wss.str();
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
	template <
		class PageType,
		typename = std::enable_if < std::is_arithmetic<PageType>::value >::type
	>
	void for_each_page(void* const begin, void* const end, std::function<void(PageType*)>& func) {
		bool reverse = begin > end;
		for (PageType* it = (PageType*)(reverse ? end : begin); (reverse ? it > begin : it < end); it += (reverse ? -1 : 1) * sizeof(PageType)) {
			func(it);
		}
		if (reverse)
			func((PageType*)begin);
	}

	template <
		class PageType,
		typename = std::enable_if < std::is_arithmetic<PageType>::value >::type
	>
	void for_each_page(void* const begin, void* const end, void (*func)(PageType& pP)) {
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
	void for_each_bit(const void* const begin, const void* const end, void (*func)(bool b));

	/* For each function that iterates over each bit in a memory block.
	*
	Parameters
	* begin: pointer to the beginning of the memory block.
	* end: pointer to the end of the memory block.
	* func: reference to a function that will be used on every bit.
	*
	Note that a reverse iteration can be achieved if begin > end.
	*/
	void for_each_bit(const void* const begin, const void* const end, std::function<void(bool)>& func);

	// A class that encapsulates all the other functions in a single object if that's more your speed. I'd prefer the other functions tho.
	template <
		const std::size_t n, 
		const std::size_t start_bit = 0,
		const std::size_t end_bit = n,
		typename = std::enable_if < (n > 0) >::type,
		typename = std::enable_if < (start_bit < end_bit) >::type,
		typename = std::enable_if < (end_bit <= n) >::type
	>
	class BitArray {
		void* data;
	public:

		BitArray() { data = calloc(BitUtils::size(n), 1); }
		template <
			const std::size_t other_n,
			const std::size_t other_start_bit = 0,
			const std::size_t other_end_bit = other_n,
			typename = std::enable_if < (other_n > 0) >::type,
			typename = std::enable_if < (other_start_bit < other_n) >::type,
			typename = std::enable_if < (other_end_bit <= other_n) >::type,
			typename = std::enable_if < (other_start_bit < other_end_bit) >::type,
			typename = std::enable_if < (other_end_bit - other_start_bit) >= n >::type
			>
		BitArray(const BitArray<other_n, other_start_bit, other_end_bit>& other) {
			data = malloc(BitUtils::size(n));
			copy<other_n, other_start_bit, other_end_bit>(other);
		}

		~BitArray() { free(data); }

		void set(std::size_t i, bool b)		{ BitUtils::set(data, n, start_bit, end_bit, i, b); }
		bool get(std::size_t i) const		{ return BitUtils::get(data, n, start_bit, end_bit, i); }
		void flip(std::size_t i)			{ BitUtils::flip(data, n, start_bit, end_bit, i); }
		void fill(bool b)                   { BitUtils::fill(data, n, start_bit, end_bit, b); }
		void fill_s(bool b)                 { BitUtils::fill_s(data, n, start_bit, end_bit, b); }
		
		template <
			const std::size_t other_n,
			const std::size_t other_start_bit = 0,
			const std::size_t other_end_bit = other_n,
			typename = std::enable_if < (other_n > 0) >::type,
			typename = std::enable_if < (other_start_bit < other_n) >::type,
			typename = std::enable_if < (other_end_bit <= other_n) >::type,
			typename = std::enable_if < (other_start_bit < other_end_bit) >::type,
			typename = std::enable_if < (other_end_bit - other_start_bit) >= n >::type
		>
		void copy(const BitArray<other_n, other_start_bit, other_end_bit>& other) {
			BitUtils::copy(data, n, start_bit, end_bit, *other, other_n, other_start_bit, other_end_bit); 
		}

		std::string str() const { return BitUtils::str(data, n, start_bit, end_bit); }
		std::wstring wstr() const { return BitUtils::wstr(data, n, start_bit, end_bit); }

		void for_each_bit(void (*func)(bool b))	const			 { BitUtils::for_each_bit(data, (char*)data + BitUtils::size(n), func); }
		void for_each_bit(std::function<void(bool)>& func) const { BitUtils::for_each_bit(data, (char*)data + BitUtils::size(n), func); }
		void for_each_byte(void (*func)(char* pC))				 { BitUtils::for_each_byte(data, (char*)data + BitUtils::size(n), func); }
		void for_each_byte(std::function<void(char*)>& func)	 { BitUtils::for_each_byte(data, (char*)data + BitUtils::size(n), func); }

		template <class PageType, typename = std::enable_if_t < std::is_arithmetic_v<PageType>, bool > >
		void for_each_page(void (*func)(PageType* pP)) {
			BitUtils::for_each_page(data, (char*)data + BitUtils::size(n), func);
		}

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
		operator bool() const { 
			if (n == (end_bit - start_bit))
				return BitUtils::bool_op(data, n, start_bit, end_bit);
			return BitUtils::bool_op_s(data, n, start_bit, end_bit);
		}

		template <
			const std::size_t other_n,
			const std::size_t other_start_bit = 0,
			const std::size_t other_end_bit = other_n,
			typename = std::enable_if < (other_n > 0) >::type,
			typename = std::enable_if < (other_start_bit < other_n) >::type,
			typename = std::enable_if < (other_end_bit <= other_n) >::type,
			typename = std::enable_if < (other_start_bit < other_end_bit) >::type,
			typename = std::enable_if < (other_end_bit - other_start_bit) >= n >::type
		>
		operator BitArray<other_n, other_start_bit, other_end_bit>() const {
			BitArray<other_n, other_start_bit, other_end_bit> output;
			output.copy(*this);
			return output;
		}
	};
};


#endif // BITUTILS_H