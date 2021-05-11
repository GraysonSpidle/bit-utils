/* BitUtils.h
* Author: Grayson Spidle
* 
* This file defines a generic class called BitUtils which can be used to manipulate the bits in 
* memory blocks as if they were arrays (I mean, they essentially are just bit arrays). I made this
* as a programming exercise so I could learn how to use the C api and learn how memory management
* works at a low level. I'm not claiming this is cutting edge stuff and I'm sure it has massive
* inefficiencies. I tried to treat it as if it were a piece of professional work just as practice
* for when I actually do programming professionally.
* 
* Targeted the C++17 language standard.
* 
* You might have to change some things if you aren't using Visual C++. I tried to do some preprocessor
* stuff to make it more flexible, but I haven't tested this on Linux.
*/

#ifndef BITUTILS2_H
#define BITUTILS2_H

#include <cstdint>
#include <string.h>
#include <climits>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <iostream>
#include <sstream>
#include <functional>
#include <type_traits>

#ifdef CHAR_BIT
constexpr const std::size_t CHAR_SIZE = CHAR_BIT;
#else
#ifdef __CHAR_BIT__
constexpr const std::size_t CHAR_SIZE = __CHAR_BIT__;
#endif // __CHAR_BIT__
#endif // CHAR_BIT

/// <summary>
/// A class comprised mainly of static functions used for manipulating memory blocks as if they
/// were arrays. Since we can't explicitly allocate bits, we have to settle for just manipulating
/// the bytes that house them. This is reflected in some of the terminology and syntax defined.
/// </summary>
/// <param name="_n">the number of bits in the "array". Must be > 0.
/// This does not have to be a log of 2. Violations result in undefined behavior.</param>
/// <param name="start_bit">the beginning bound of the "array". Use this to specify a range of bits
/// the class is allowed to modify. Must be less than _n and greater than end_bit.
/// Violations result in undefined behavior.</param>
/// <param name="end_bit">the end bound of the "array". Use this to specify a range of bits the
/// class is allowed to modify. Must be less than or equal to _n and greater than begin_bit. Violations result in undefined
/// behavior.</param>
template <
	const std::size_t _n,
	const std::size_t start_bit = 0,
	const std::size_t end_bit = _n,
	std::enable_if_t < (_n > 0) , bool > = true,
	std::enable_if_t < (start_bit < _n) , bool > = true,
	std::enable_if_t < (end_bit <= _n) , bool > = true,
	std::enable_if_t < (start_bit < end_bit) , bool > = true
>
class BitUtils {
#define _BITUTILS_IS_LITTLE_ENDIAN (1 << 1) > 1
public:
	constexpr static const std::size_t n = end_bit - start_bit; // The number of bits we're working with.
	constexpr static const std::size_t size = (_n <= CHAR_SIZE ? 1 : (_n / CHAR_SIZE) + bool(_n % CHAR_SIZE)); // The number of bytes that would be allocated.
	constexpr static const std::size_t start_bit = start_bit; // The index of the bit to start on (inclusive).
	constexpr static const std::size_t end_bit = end_bit; // The index of the bit to end on (exclusive).

	// ========== TYPE DEFS ==========


	/* A typedef that sets bounds for the BitUtils.
	This guarantees 2 things:
	* the resulting type's n value will be a log of 2.
	* the safe versions of functions (where available) are forcibly used.
	*/
	template <
		const std::size_t start,
		const std::size_t end,
		std::enable_if_t < (start < n) , bool > = true,
		std::enable_if_t < (end <= n) , bool > = true,
		std::enable_if_t < (start < end) , bool > = true
	>
	using bound = BitUtils<size * CHAR_SIZE, start, end>;

	/* A typedef that removes all bounds for the BitUtils. 
	It guarantees 2 things:
	* the resulting type's n value will be a log of 2.
	* the safe versions of functions (where available) are not forcibly used.
	*/
	using unbound = BitUtils<size * CHAR_SIZE>;

	/* A typedef that, given a type, makes an unbounded BitUtils class that accomodates for its size. */
	template <
		class Type
	>
	using of = BitUtils<sizeof(Type) * CHAR_SIZE>;
	

	// ========== FUNCTIONS ==========

	static char* const getPage(void* const arr_ptr, std::size_t i) {
		if constexpr (_n == n) {
			if (i > size * CHAR_SIZE)
				throw std::out_of_range("");
		}
		else {
			if (i >= n)
				throw std::out_of_range("");
		}
		
		if constexpr (size == 1)
			return (char*)arr_ptr;
		else {
			return (char*)arr_ptr + ((i + start_bit) / CHAR_SIZE);
		}
	}
	static const char* const getPage(const void* const arr_ptr, std::size_t i) {
		if constexpr (n == _n) {
			if (i > size * CHAR_SIZE)
				throw std::out_of_range("");
		}
		else {
			if (i >= n)
				throw std::out_of_range("");
		}

		if constexpr (size == 1)
			return (char*)arr_ptr;
		else {
			return (char*)arr_ptr + ((i + start_bit) / CHAR_SIZE);
		}
	}

	/// <summary>
	/// Gets the designated bit.
	/// </summary>
	/// <param name="src">the pointer to the memory block.</param>
	/// <param name="i">the index of the bit to get.</param>
	/// <returns>true if the bit is 1 and false if the bit 0.</returns>
	static bool get(const void* const src, std::size_t i) {
		if constexpr (_BITUTILS_IS_LITTLE_ENDIAN)
			return *getPage(src, i) & ((std::size_t)1 << ((i + start_bit) % CHAR_SIZE));
		else
			return *getPage(src, i) & ((std::size_t)1 >> ((i + start_bit) % CHAR_SIZE));
	}

	/// <summary>
	/// Flips the designated bit (ie from 0 to 1 or from 1 to 0). It is worth noting this is faster than set().
	/// </summary>
	/// <param name="src">the pointer to the memory block.</param>
	/// <param name="i">the index of the bit to flip.</param>
	static void flip(void* const src, std::size_t i) {
		if constexpr (_BITUTILS_IS_LITTLE_ENDIAN)
			*getPage(src, i) ^= ((std::size_t)1 << ((i + start_bit) % CHAR_SIZE));
		else
			*getPage(src, i) ^= ((std::size_t)1 >> ((i + start_bit) % CHAR_SIZE));
	}

	/* Sets the designated bit to reflect the supplied boolean.
	Parameters:
	* src: the pointer to the memory block.
	* i: the index of the bit to set.
	* b: the boolean that reflects what to set the bit to (ie 1 for true and 0 for false).
	*/

	/// <summary>
	/// Sets the designated bit to reflect the supplied boolean.
	/// </summary>
	/// <param name="src">the pointer to the memory block.</param>
	/// <param name="i">the index of the bit to set.</param>
	/// <param name="b">the boolean that reflects what to set the bit to (ie 1 for true and 0 for false).</param>
	static void set(void* const src, std::size_t i, bool b) {
		if constexpr (_BITUTILS_IS_LITTLE_ENDIAN)
			*getPage(src, i) |= ((std::size_t)1 << ((i + start_bit) % CHAR_SIZE));
		else
			*getPage(src, i) |= ((std::size_t)1 >> ((i + start_bit) % CHAR_SIZE));

		// Flipping the bit if user wants it to be false
		if (!b)
			flip(src, i);
	}

	/// <summary>
	/// Dynamically allocates enough bytes to accomodate as designated by n.
	/// This is literally just utilizes calloc()
	/// </summary>
	/// <returns>Returns a void* to the memory block.</returns>
	static void* create() {
		return calloc(size, 1);
	}

	/// <summary>
	/// Fills the memory block to reflect the supplied boolean.
	/// If the BitUtils has bounds, then this function will always default to fill_s().
	/// </summary>
	/// <param name="src">the pointer to the memory block.</param>
	/// <param name="b">the boolean that reflects what to fll the memory block with (ie 1 for true and 0 false).</param>
	static void fill(void* const src, bool b) {
		if constexpr (n != _n)
			fill_s(src, b);
		else if constexpr (n == CHAR_SIZE)
			*((unsigned char*)src) = b ? -1 : 0;
		else
			memset(src, b ? (unsigned char)-1 : 0, size);
	}

	/* Safely fills the memory block to reflect the supplied boolean. "Safely" means that it will only modify the bits specified either by the bounds or by n.
	Parameters:
	* src: the pointer to the memory block.
	* b: the boolean that reflects what to safely fill the memory block with (ie 1 for true and 0 for false).
	*/

	/// <summary>
	/// Fills the bounded memory block to reflect the supplied boolean.
	/// </summary>
	/// <param name="src">the pointer to the memory block.</param>
	/// <param name="b">the boolean that reflects what to fill the memory block with (ie 1 for true and 0 for false).</param>
	static void fill_s(void* const src, bool b) {
		if constexpr (_n != n) {
			if constexpr (n < CHAR_SIZE) {
				char* page = getPage(src, start_bit);
				using PageUtils = BitUtils<n, start_bit % CHAR_SIZE, (CHAR_SIZE <= n ? CHAR_SIZE : (end_bit % CHAR_SIZE)) >;

				for (std::size_t i = 0; i < PageUtils::n; i++) {
					PageUtils::set(page, i, b);
				}
			}
			else {
				char* page;
				if constexpr (start_bit) {
					page = getPage(src, start_bit);
					using StartPageUtils = BitUtils<CHAR_SIZE, start_bit % CHAR_SIZE>;

					for (std::size_t i = 0; i < StartPageUtils::n; i++) {
						StartPageUtils::set(page, i, b);
					}
				}

				page = getPage(src, start_bit);
				if constexpr (end_bit < _n) {
					for (std::size_t i = 1; i < size - 1; i++) {
						BitUtils<CHAR_SIZE>::fill(page + i, b);
					}
					page = getPage(src, end_bit - 1);
					using EndPageUtils = BitUtils<CHAR_SIZE, 0, end_bit % CHAR_SIZE>;

					for (std::size_t i = 0; i < EndPageUtils::n; i++) {
						EndPageUtils::set(page, i, b);
					}
				}
				else {
					for (std::size_t i = 1; i < size; i++) {
						BitUtils<CHAR_SIZE>::fill(page + i, b);
					}
				}
			}
		}
		else {
			if constexpr ((size * CHAR_SIZE) == n) {
				fill(src, b);
			}
			else {
				for (std::size_t i = 0; i < n; i++) {
					set(src, i, b);
				}
			}
		}
	}


	/* Copies the memory block into another.
	Template Parameters:
	* BitUtils_src: the BitUtils class to use for the src variable. Defaults to the current BitUtils class.
	* BitUtils_dst: the BitUtils class to use for the dst variable. Defaults to the current BitUtils class.
	Parameters:
	* src: the pointer to the source memory block. If this equals dst, this function does nothing.
	* dst: the pointer to the destination memory block. If this equals src, this function does nothing.
	*/

	/// <summary>
	/// Copies the memory block into another.
	/// </summary>
	/// <typeparam name="BitUtils_src">the BitUtils class to use for src. Defaults to the current BitUtils class.</typeparam>
	/// <typeparam name="BitUtils_dst">the BitUtils class to use for dst. Defaults to the current BitUtils class.</typeparam>
	/// <param name="src">the pointer to the source memory block. If this equals dst, this function does nothing.</param>
	/// <param name="dst">the pointer to the destination memory block. If the equals src, this function does nothing.</param>
	template <
		class BitUtils_src = BitUtils,
		class BitUtils_dst = BitUtils,
		std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_src> , bool > = true,
		std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_dst> , bool > = true
	>
	static void copy(const void* const src, void* const dst) {
		if (src == dst)
			return;
		if constexpr (std::is_same_v<BitUtils, BitUtils_dst> && std::is_same_v<BitUtils, BitUtils_src>) {
#ifdef __STDC_LIB_EXT1__
			memcpy_s(dst, size, src, size);
			return;
#else
#ifdef __GNUG__
			memcpy(dst, src, size);
			return;
#endif // __GNUG__
#endif // __STDC_LIB_EXT1__
		}
		BitUtils_dst::fill(dst, 0);
		bitwise_or<BitUtils_src, BitUtils_dst, BitUtils_dst>(src, dst, dst);
	}

	/// <summary>
	///	Performs the & operation on every bit between the left and the right memory blocks and puts the result in the destination memory block.
	/// </summary>
	/// <typeparam name="BitUtils_left">the BitUtils class to use for the left variable. Defaults to the current BitUtils class.</typeparam>
	/// <typeparam name="BitUtils_right">the BitUtils class to use for the right variable. Defaults to the current BitUtils class.</typeparam>
	/// <typeparam name="BitUtils_dst">the BitUtils class to use for the dst variable. Defaults to the current BitUtils class.</typeparam>
	/// <param name="left">the pointer to the memory block on the left side of the &. This can be the same as the value for the right or dst parameter.</param>
	/// <param name="right">the pointer to the memory block on the right side of the &. This can be the same as the value for the left or dst parameter.</param>
	/// <param name="dst">the pointer to the memory block that gets the result of the operation. This can be the same as the value for the left or right parameter.</param>
	template <
		class BitUtils_left = BitUtils,
		class BitUtils_right = BitUtils,
		class BitUtils_dst = BitUtils,
		std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_left> , bool > = true,
		std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_right> , bool > = true,
		std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_dst> , bool > = true
	>
	static void bitwise_and(const void* const left, const void* const right, void* const dst) {
		if (left == right) {
			if (left == dst)
				return;
			copy<BitUtils_left, BitUtils_dst>(left, dst);
			return;
		}
		for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
			*BitUtils_dst::getPage(dst, i) = *BitUtils_left::getPage(left, i) & *BitUtils_right::getPage(right, i);
		}
	}

	/// <summary>
	///	Performs the | operation on every bit between the left and the right memory blocks and puts the result in the destination memory block.
	/// </summary>
	/// <typeparam name="BitUtils_left">the BitUtils class to use for the left variable. Defaults to the current BitUtils class.</typeparam>
	/// <typeparam name="BitUtils_right">the BitUtils class to use for the right variable. Defaults to the current BitUtils class.</typeparam>
	/// <typeparam name="BitUtils_dst">the BitUtils class to use for the dst variable. Defaults to the current BitUtils class.</typeparam>
	/// <param name="left">the pointer to the memory block on the left side of the |. This can be the same as the value for the right or dst parameter.</param>
	/// <param name="right">the pointer to the memory block on the right side of the |. This can be the same as the value for the left or dst parameter.</param>
	/// <param name="dst">the pointer to the memory block that gets the result of the operation. This can be the same as the value for the left or right parameter.</param>
	template <
		class BitUtils_left = BitUtils,
		class BitUtils_right = BitUtils,
		class BitUtils_dst = BitUtils,
		std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_left> , bool > = true,
		std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_right> , bool > = true,
		std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_dst> , bool > = true
	>
	static void bitwise_or(const void* const left, const void* const right, void* const dst) {
		if (left == right) {
			bitwise_and<BitUtils_left, BitUtils_right, BitUtils_dst>(left, right, dst);
			return;
		}
		for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
			*BitUtils_dst::getPage(dst, i) = *BitUtils_left::getPage(left, i) | *BitUtils_right::getPage(right, i);
		}
	}

	/// <summary>
	///	Performs the ^ operation on every bit between the left and the right memory blocks and puts the result in the destination memory block.
	/// </summary>
	/// <typeparam name="BitUtils_left">the BitUtils class to use for the left variable. Defaults to the current BitUtils class.</typeparam>
	/// <typeparam name="BitUtils_right">the BitUtils class to use for the right variable. Defaults to the current BitUtils class.</typeparam>
	/// <typeparam name="BitUtils_dst">the BitUtils class to use for the dst variable. Defaults to the current BitUtils class.</typeparam>
	/// <param name="left">the pointer to the memory block on the left side of the ^. This can be the same as the value for the right or dst parameter.</param>
	/// <param name="right">the pointer to the memory block on the right side of the ^. This can be the same as the value for the left or dst parameter.</param>
	/// <param name="dst">the pointer to the memory block that gets the result of the operation. This can be the same as the value for the left or right parameter.</param>
	template <
		class BitUtils_left = BitUtils,
		class BitUtils_right = BitUtils,
		class BitUtils_dst = BitUtils,
		std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_left> , bool > = true,
		std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_right> , bool > = true,
		std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_dst> , bool > = true
	>
	static void bitwise_xor(const void* const left, const void* const right, void* const dst) {
		if (left == right) {
			BitUtils_dst::fill(dst, 0);
			return;
		}
		for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
			*BitUtils_dst::getPage(dst, i) = *BitUtils_left::getPage(left, i) ^ *BitUtils_right::getPage(right, i);
		}
	}

	/// <summary>
	/// Performs the ~ operation on every bit in the src memory block and puts the result in the destinatiion memory block.
	/// </summary>
	/// <typeparam name="BitUtils_src">the BitUtils class to use for the src variable. Defaults to the current BitUtils class.</typeparam>
	/// <typeparam name="BitUtils_dst">the BitUtils class to use for the dst variable. Defaults to the current BitUtils class.</typeparam>
	/// <param name="src">the pointer to the source memory block. This can be the same as the value for the dst parameter.</param>
	/// <param name="dst">the pointer to the destination memory block. This can be the same as the value for the src parameter.</param>
	template <
		class BitUtils_src = BitUtils,
		class BitUtils_dst = BitUtils,
		std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_src> , bool > = true,
		std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_dst> , bool > = true
	>
	static void bitwise_not(const void* const src, void* const dst) {
		if (src != dst)
			copy<BitUtils_src, BitUtils_dst>(src, dst);
		for (std::size_t i = 0; i < size * CHAR_SIZE; i += CHAR_SIZE) {
			*BitUtils_dst::getPage(dst, i) = ~(*BitUtils_dst::getPage(dst, i));
		}
	}

	/// <summary>
	/// Performs the ~ operation on every bit in the src memory block and puts the result in the same memory block.
	/// This is the same as bitwise_not(src, src)
	/// </summary>
	/// <param name="src">the pointer to the source and destination memory block.</param>
	static void bitwise_not(void* const src) {
		bitwise_not(src, src);
	}

	/// <summary>
	/// Performs the bool() operation on the entire memory block (unless BitUtils is bounded, then it defaults to the safe version).
	/// </summary>
	/// <param name="src">the pointer to the source memory block.</param>
	/// <returns>true if any of the bits are 1 and false if all the bits are 0.</returns>
	static bool bool_op(const void* const src) {
		if constexpr (start_bit || end_bit != _n)
			return bool_op_s(src);
		else {
			for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
				if (*getPage(src, i))
					return true;
			}
			return false;
		}
	}

	/// <summary>
	/// Performs the bool() operation on the bounded memory block.
	/// </summary>
	/// <param name="src">the pointer to the source memory block</param>
	/// <returns>true if any of the bits are 1 and false if all the bits are 0.</returns>
	static bool bool_op_s(const void* const src) {
		if constexpr (start_bit == 0 && end_bit == _n)
			return bool_op(src);
		else {
			for (std::size_t i = 0; i < n; i++) {
				if (get(src, i))
					return true;
			}
			return false;
		}
	}

	template <
		class BitUtils_left = BitUtils,
		class BitUtils_right = BitUtils,
		std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_left> , bool > = true,
		std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_right> , bool > = true
	>
	static bool equals(const void* const left, const void* const right) {
		for (std::size_t i = 0; i < size; i++) {
			if (bool(*getPage(left, i *  CHAR_SIZE)) != bool(*getPage(right, i * CHAR_SIZE)))
				return false;
		}
		return true;
	}

	static void str(const void* const arr_ptr, char* const buf) {
		if (!strlen(buf))
			return;
		for (std::size_t i = 0; i < (n < strlen(buf) ? n : strlen(buf)) - 1; i++) {
			buf[i] = get(arr_ptr, i) ? '1' : '0';
		}
		buf[(n < strlen(buf) ? n : strlen(buf))] = '\0';
	}

	static void wstr(const void* const arr, wchar_t* const buf, const std::size_t buf_n) {
		if (!buf_n)
			return;
		for (std::size_t i = 0; i < (n < buf_n ? n : buf_n) - 1; i++) {
			buf[i] = get(arr, i) ? L'1' : L'0';
		}
		buf[(n < buf_n ? n : buf_n)] = L'\0';
	}

	static void str(const void* const arr_ptr, std::ostream& os) {
		std::size_t count;
		if constexpr (n != _n)
			count = n;
		else
			count = size * CHAR_SIZE;

		for (std::size_t i = 0; i < count; i++) {
			os << get(arr_ptr, i) ? '1' : '0';
		}
		
	}

	static void wstr(const void* const arr_ptr, std::wostream& wos) {
		for (std::size_t i = 0; i < n; i++) {
			wos << get(arr_ptr, i) ? L'1' : L'0';
		}
	}

	static std::string str(const void* const arr_ptr) {
		std::stringstream ss;
		str(arr_ptr, ss);
		return ss.str();
	}

	static std::wstring wstr(const void* const arr_ptr) {
		std::wstringstream ss;
		wstr(arr_ptr, ss);
		return ss.str();
	}

	static void for_each_byte(void* const arr_ptr, void (*func)(char* pC)) {
		for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
			func(getPage(arr_ptr, i));
		}
	}

	static void for_each_byte(void* const arr_ptr, const std::function<void(char*)>& func) {
		for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
			func(getPage(arr_ptr, i));
		}
	}

	static void rfor_each_byte(void* const arr_ptr, void (*func)(char* pC)) {
		for (std::size_t i = n; i > CHAR_SIZE; i -= CHAR_SIZE) {
			func(getPage(arr_ptr, i));
		}
		func(getPage(arr_ptr, 0));
	}

	static void rfor_each_byte(void* const arr_ptr, std::function<void(char*)>& func) {
		for (std::size_t i = n; i > CHAR_SIZE; i -= CHAR_SIZE) {
			func(getPage(arr_ptr, i));
		}
		func(getPage(arr_ptr, 0));
	}

	static void for_each_bit(const void* const arr_ptr, void (*func)(bool b)) {
		const char* page;
		for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
			page = getPage(arr_ptr, i);
			for (std::size_t j = 0; j < CHAR_SIZE; j++) {
				func(BitUtils<CHAR_SIZE>::get(page, j));
			}
		}
	}

	static void for_each_bit(const void* const arr_ptr, std::function<void(bool)>& func) {
		const char* page;
		for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
			page = getPage(arr_ptr, i);
			for (std::size_t j = 0; j < CHAR_SIZE; j++) {
				func(BitUtils<CHAR_SIZE>::get(page, j));
			}
		}
	}

	static void rfor_each_bit(const void* const arr_ptr, void (*func)(bool b)) {
		const char* page;
		for (std::size_t i = n; i > CHAR_SIZE; i -= CHAR_SIZE) {
			page = getPage(arr_ptr, i);
			for (std::size_t j = CHAR_SIZE - 1; j > 0; j--) {
				func(BitUtils<CHAR_SIZE>::get(page, j));
			}
			func(BitUtils<CHAR_SIZE>::get(page, 0));
		}

		page = getPage(arr_ptr, 0);
		for (std::size_t j = CHAR_SIZE - 1; j > 0; j--) {
			func(BitUtils<CHAR_SIZE>::get(page, j));
		}
		func(BitUtils<CHAR_SIZE>::get(page, 0));
	}

	static void rfor_each_bit(const void* const arr_ptr, std::function<void(bool)>& func) {
		const char* page;
		for (std::size_t i = n; i > CHAR_SIZE; i -= CHAR_SIZE) {
			page = getPage(arr_ptr, i);
			for (std::size_t j = CHAR_SIZE - 1; j > 0; j--) {
				func(BitUtils<CHAR_SIZE>::get(page, j));
			}
			func(BitUtils<CHAR_SIZE>::get(page, 0));
		}

		page = getPage(arr_ptr, 0);
		for (std::size_t j = CHAR_SIZE - 1; j > 0; j--) {
			func(BitUtils<CHAR_SIZE>::get(page, j));
		}
		func(BitUtils<CHAR_SIZE>::get(page, 0));
	}

	// =============================================

	/// <summary>
	/// Cast operator overload to aid in our SFINAE exploits. This allows us to use the std::is_convertible type trait to
	/// figure out at compile time if another BitUtils class is compatible with this one. This also allows us to effectively cast
	/// classes by casting objects made from the class and just using the static functions.
	/// </summary>
	/// <param name="other_n">this is the value you'd put in the first spot if you were specifying a BitUtils class. This must be greater than 0.</param>
	/// <param name="other_start_bit">this is the value you'd use to specify the begining bound.</param>
	/// <param name="other_end_bit">this is the value you'd use to specify the end bound.</param>
	template <
		const std::size_t other_n,
		const std::size_t other_start_bit = 0,
		const std::size_t other_end_bit = other_n,
		std::enable_if_t < (other_n > 0) , bool > = true,
		std::enable_if_t < (other_start_bit < other_n) , bool > = true,
		std::enable_if_t < (other_end_bit <= other_n) , bool > = true,
		std::enable_if_t < (other_start_bit < other_end_bit) , bool > = true,
		std::enable_if_t < (other_end_bit - other_start_bit) >= n , bool > = true
	>
	operator BitUtils<other_n, other_start_bit, other_end_bit>() const {
		return BitUtils<other_n, other_start_bit, other_end_bit>();
	}
};

#undef _BITUTILS_IS_LITTLE_ENDIAN
#endif // BITUTILS2_H