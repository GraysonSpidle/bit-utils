/* BitUtils.h
* Author: Grayson Spidle
* 
* This file defines the BitUtils namespace which houses functions for manipulating bits in memory blocks
* as if they were arrays. I made this as a programming exercise so I could learn how to use the C api
* and learn how memory management works at a lower level. I'm not claiming this is cutting edge stuff
* and I'm sure it has massive inefficiencies.
* 
* This file also defines a generic class called BitUtils which can be used to manipulate the bits in 
* memory blocks as if they were arrays (I mean, they essentially are just bit arrays).
* 
* I tried to treat it as if it were a piece of professional work just as practice for when I actually do programming professionally.
* 
* The bare minimum language standard for this file is C++11
* 
* You might have to change some things if you aren't using Visual C++. I tried to do some preprocessor
* stuff to make it more flexible, but I'm not perfect.
* 
* If you are using Visual C++, then you'll need to add /Zc:__cplusplus to your command line arguments for compilation.
*/

#ifndef __BITUTILS_H__
#define __BITUTILS_H__

#include <cstdlib>
#include <cstdint>
#include <string.h>
#include <climits>
#include <cstdio>
#include <cmath>

#include <iostream>
#include <sstream>
#include <functional>
#include <type_traits>

#define _BITUTILS_IS_LITTLE_ENDIAN (1 << 1) > 1

#if __cplusplus >= 201100 // C++11
/* The namespace for the BitUtils library.
*
Guarantees for every function/class inside this namespace:
* In functions/classes where you can specify a start_bit and end_bit, the start_bit is ALWAYS inclusive and end_bit is ALWAYS exclusive.
* In functions/classes where you specify an n value (which is all of them) the n value does not have to be a log of 2, because it will automatically be rounded up to the nearest log of 2.
* In function/classes that have a non-const parameter, it 100% means that they WILL modify the parameter in some way. This extends to const* and *const parameters.
* 
All C++11 functions can throw any of these:
* std::invalid_argument: for when you have invalid bounds or (in the case of from_str()) you have an unrecognized char (ie not a 1 or 0).
* std::out_of_range: for when you have valid bounds, but you tried to read/write a bit that is outside those bounds.
* 
For the C++17 class, you will likely run into funky errors. I've found that if you get errors in regards to SFINAE stuff, then your logic is probably incorrect some where (ie you're putting in an incorrect number for the bounds).
*
For all you performance min-maxers, it is worth noting that for the C++11 functions, the amount of parameters you specify does matter.
* Functions that require a start_bit and end_bit for each block of memory act differently than ones that don't.
* In other words, if you don't need to specify bounds, then don't.
*/
namespace BitUtils {
#if defined(CHAR_BIT)
	constexpr const std::size_t CHAR_SIZE = CHAR_BIT;
#elif defined(__CHAR_BIT__)
	constexpr const std::size_t CHAR_SIZE = __CHAR_BIT__;
#else
	constexpr const std::size_t CHAR_SIZE = 8;
#endif

	/* Calculates the size (in bytes) of a memory block that is of size n (in bits).
	*
	Parameters
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	*
	Returns the size of the memory block in bytes.
	*/
	inline std::size_t size(const std::size_t n);

	inline std::size_t size(const std::size_t start_bit, const std::size_t end_bit);

	/* Allocates a memory block on the heap and guarantees that it is at least of size n (in bits).
	* The memory block is comprised entirely of 0s (calloc).
	* You will have to use free() on the memory block when you're done, if you don't want memory leaks.
	*
	Parameters
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	*
	Returns a pointer to the memory block.
	*/
	void* create(const std::size_t n);

	/* Gets the selected bit's state.
	*
	Parameters
	* block: the pointer to the memory block.
	* start_bit: the starting bit of your bounds (inclusive).
	* end_bit: the ending bit of your bounds (exclusive).
	* i: the local index of the bit you want to get.
	*
	Returns true if the bit is set, and false if it isn't.
	*/
	bool get(const void* const block,
		const std::size_t start_bit,
		const std::size_t end_bit,
		const std::size_t i
	);

	/* Gets the selected bit's state.
	* 
	Parameters
	* block: the pointer to the memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	* i: the index of the bit you want to get.
	*/
	bool get(const void* const block,
		const std::size_t n,
		const std::size_t i);

	/* Flips the selected bit from true to false or vice versa.
	*
	Parameters
	* block: the pointer to the memory block.
	* start_bit: the starting bit of your bounds (inclusive).
	* end_bit: the ending bit of your bounds (exclusive).
	* i: the local index of the bit you want to flip.
	*/
	void flip(void* const block,
		const std::size_t start_bit,
		const std::size_t end_bit,
		const std::size_t i);

	/* Flips the selected bit from true to false or vice versa.
	* 
	Parameters
	* block: the pointer to the memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	* i: the index of the bit you want to flip.
	*/
	void flip(void* const block,
		const std::size_t n,
		const std::size_t i);

	/* Sets the selected bit to reflect the given boolean.
	*
	Parameters
	* block: the pointer to the memory block.
	* start_bit: the starting bit of your bounds (inclusive).
	* end_bit: the ending bit of your bounds (exclusive).
	* i: the local index of the bit you want to set.
	* b: the state you want to set the bit to.
	*
	For you performance min-maxers, it is worth noting that setting a bit to false is slower than setting it to true. In general, the flip() function is faster than this.
	*/
	void set(void* const block,
		const std::size_t start_bit,
		const std::size_t end_bit,
		const std::size_t i,
		const bool b);

	/* Sets the selected bit to reflect the given boolean.
	*
	Parameters
	* block: the pointer to the memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	* i: the local index of the bit you want to set.
	* b: the state you want to set the bit to.
	*
	For you performance min-maxers, it is worth noting that setting a bit to false is slower than setting it to true. In general, the flip() function is faster than this.
	*/
	void set(void* const block,
		const std::size_t n,
		const std::size_t i,
		const bool b);

	/* Fills the memory block with 1s or 0s.
	*
	Parameters
	* block: the pointer to the memory block.
	* start_bit: the starting bit of your bounds (inclusive).
	* end_bit: the ending bit of your bounds (exclusive).
	* b: the state you want to set all the bits to (true = 1 and false = 0).
	*/
	void fill(void* const block,
		const std::size_t start_bit,
		const std::size_t end_bit,
		const bool b);

	/* Fills all of the memory block with 1s or 0s.
	*
	Parameters
	* block: the pointer to the memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2, but it will be rounded up to the nearest log of 2.
	* b: the state you want to set all the bits to (true = 1 and false = 0).
	*/
	void fill(void* const block,
		const std::size_t n,
		const bool b);

	/* Does the & bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left & right
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right or dst.
	* left_start_bit: the starting bit for the left memory block's bounds (inclusive).
	* left_end_bit: the ending bit for the left memory block's bounds (exclusive).
	* right: the pointer to the right memory block. This can be the same as left or dst.
	* right_start_bit: the starting bit for the right memory block's bounds (inclusive).
	* right_end_bit: the ending bit for the right memory block's bounds (exclusive).
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* dst_start_bit: the starting bit for the dst memory block's bounds (inclusive).
	* dst_end_bit: the ending bit for the dst memory block's bounds (exclusive).
	*/
	void bitwise_and(const void* const left,
		const std::size_t left_start_bit,
		const std::size_t left_end_bit,
		const void* const right,
		const std::size_t right_start_bit,
		const std::size_t right_end_bit,
		void* const dst,
		const std::size_t dst_start_bit,
		const std::size_t dst_end_bit);

	void bitwise_and(const void* const left,
		const std::size_t left_start_bit,
		const std::size_t left_end_bit,
		const bool right,
		void* const dst,
		const std::size_t dst_start_bit,
		const std::size_t dst_end_bit);

	/* Does the & bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left & right
	* 
	Parameters
	* left: the pointer to the left memory block. This can be the same as right or dst.
	* right: the pointer to the right memory block. This can be the same as left or dst.
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* n: the size of all 3 memory blocks in bits. Doesn't have to be a log of 2.
	*/
	void bitwise_and(const void* const left,
		const void* const right,
		void* const dst,
		const std::size_t n);

	void bitwise_and(const void* const left,
		const bool right,
		void* const dst,
		const std::size_t n);

	/* Does the & bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left & right
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right or dst.
	* right: the pointer to the right memory block. This can be the same as left or dst.
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* start_bit: the starting bit for all 3 memory blocks' bounds (inclusive).
	* end_bit: the ending bit for all 3 memory blocks' bounds (exclusive).
	*/
	void bitwise_and(const void* const left,
		const void* const right,
		void* const dst,
		const std::size_t start_bit,
		const std::size_t end_bit);

	void bitwise_and(const void* const left,
		const bool right,
		void* const dst,
		const std::size_t start_bit,
		const std::size_t end_bit);

	/* Does the | bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left | right
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right or dst.
	* left_start_bit: the starting bit for the left memory block's bounds (inclusive).
	* left_end_bit: the ending bit for the left memory block's bounds (exclusive).
	* right: the pointer to the right memory block. This can be the same as left or dst.
	* right_start_bit: the starting bit for the right memory block's bounds (inclusive).
	* right_end_bit: the ending bit for the right memory block's bounds (exclusive).
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* dst_start_bit: the starting bit for the dst memory block's bounds (inclusive).
	* dst_end_bit: the ending bit for the dst memory block's bounds (exclusive).
	*/
	void bitwise_or(const void* const left,
		const std::size_t left_start_bit,
		const std::size_t left_end_bit,
		const void* const right,
		const std::size_t right_start_bit,
		const std::size_t right_end_bit,
		void* const dst,
		const std::size_t dst_start_bit,
		const std::size_t dst_end_bit);

	/* Does the | bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left | right
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right or dst.
	* right: the pointer to the right memory block. This can be the same as left or dst.
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* start_bit: the starting bit for all 3 memory blocks' bounds (inclusive).
	* end_bit: the ending bit for all 3 memory blocks' bounds (exclusive).
	*/
	void bitwise_or(const void* const left,
		const void* const right,
		void* const dst,
		const std::size_t start_bit,
		const std::size_t end_bit);

	/* Does the | bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left | right
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right or dst.
	* right: the pointer to the right memory block. This can be the same as left or dst.
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* n: the size of all 3 memory blocks in bits. Doesn't have to be a log of 2.
	*/
	void bitwise_or(const void* const left,
		const void* const right,
		void* const dst,
		const std::size_t n);

	/* Does the ^ bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left ^ right
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right or dst.
	* left_start_bit: the starting bit for the left memory block's bounds (inclusive).
	* left_end_bit: the ending bit for the left memory block's bounds (exclusive).
	* right: the pointer to the right memory block. This can be the same as left or dst.
	* right_start_bit: the starting bit for the right memory block's bounds (inclusive).
	* right_end_bit: the ending bit for the right memory block's bounds (exclusive).
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* dst_start_bit: the starting bit for the dst memory block's bounds (inclusive).
	* dst_end_bit: the ending bit for the dst memory block's bounds (exclusive).
	*/
	void bitwise_xor(const void* const left,
		const std::size_t left_start_bit,
		const std::size_t left_end_bit,
		const void* const right,
		const std::size_t right_start_bit,
		const std::size_t right_end_bit,
		void* const dst,
		const std::size_t dst_start_bit,
		const std::size_t dst_end_bit);

	/* Does the ^ bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left ^ right
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right or dst.
	* right: the pointer to the right memory block. This can be the same as left or dst.
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* start_bit: the starting bit for all 3 memory blocks' bounds (inclusive).
	* end_bit: the ending bit for all 3 memory blocks' bounds (exclusive).
	*/
	void bitwise_xor(const void* const left,
		const void* const right,
		void* const dst,
		const std::size_t start_bit,
		const std::size_t end_bit);

	/* Does the ^ bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left ^ right
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right or dst.
	* right: the pointer to the right memory block. This can be the same as left or dst.
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* n: the size of all 3 memory blocks in bits. Doesn't have to be a log of 2.
	*/
	void bitwise_xor(const void* const left,
		const void* const right,
		void* const dst,
		const std::size_t n);

	/* Does the ~ bitwise operation on a memory block and puts the result in the destination memory block.
	This is the equivalent of: dst = ~src
	*
	Parameters
	* src: the pointer to the source memory block.
	* src_start_bit: the starting bit for the source memory block's bounds (inclusive).
	* src_end_bit: the ending bit for the source memory block's bounds (exclusive).
	* dst: the pointer to the destination memory block. This can be the same as src.
	* dst_start_bit: the starting bit for the dst memory block's bounds (inclusive).
	* dst_end_bit: the ending bit for the dst memory block's bounds (exclusive).
	*/
	void bitwise_not(const void* const src,
		const std::size_t src_start_bit,
		const std::size_t src_end_bit,
		void* const dst,
		const std::size_t dst_start_bit,
		const std::size_t dst_end_bit);

	/* Does the ~ bitwise operation on a memory block and puts the result in the destination memory block.
	This is the equivalent of: dst = ~src
	*
	Parameters
	* src: the pointer to the source memory block.
	* dst: the pointer to the destination memory block. This can be the same as src.
	* n: the size of both of the memory blocks in bits. Doesn't have to be a log of 2.
	*/
	void bitwise_not(const void* const src,
		void* const dst,
		const std::size_t n);

	/* Does the ~ bitwise operation on a memory block and puts the result in the destination memory block.
	This is the equivalent of: dst = ~src
	*
	Parameters
	* src: the pointer to the source memory block.
	* dst: the pointer to the destination memory block. This can be the same as src.
	* start_bit: the starting bit for both memory blocks' bounds (inclusive).
	* end_bit: the ending bit for both memory blocks' bounds (exclusive).
	*/
	void bitwise_not(const void* const src,
		void* const dst,
		const std::size_t start_bit,
		const std::size_t end_bit);

	/* Does the ~ bitwise operation on a memory block and puts the result in the same memory block.
	This is the equivalent of: block = ~block
	*
	Parameters
	* block: the pointer to the source and destination memory block.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	*/
	void bitwise_not(void* const block,
		const std::size_t start_bit,
		const std::size_t end_bit);

	/* Does the ~ bitwise operation on a memory block and puts the result in the same memory block.
	This is the equivalent of: block = ~block
	*
	Parameters
	* block: the pointer to the source and destination memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	*/
	void bitwise_not(void* const block,
		const std::size_t n);

	/* Evaluates a memory block as a bool.
	This is similar to casting an int to a bool
	*
	Parameters
	* block: the pointer to the memory block.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	Returns false if all the bits are 0 else returns true.
	*/
	bool bool_op(const void* const block,
		const std::size_t start_bit,
		const std::size_t end_bit);

	/* Evaluates a memory block as a bool.
	This is similar to casting an int to a bool
	*
	Parameters
	* block: the pointer to the memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	Returns false if all the bits are 0 else returns true.
	*/
	bool bool_op(const void* const src,
		const std::size_t n);

	/* Copies memory from one memory block to another.
	* 
	Parameters
	* src: the pointer to the source memory block.
	* src_start_bit: the starting bit for the source memory block's bounds (inclusive).
	* src_end_bit: the ending bit for the source memory block's bounds (exclusive).
	* dst: the pointer to the destination memory block. This can be the same as src.
	* dst_start_bit: the starting bit for the dst memory block's bounds (inclusive).
	* dst_end_bit: the ending bit for the dst memory block's bounds (exclusive).
	*/
	void copy(const void* const src,
		const std::size_t src_start_bit,
		const std::size_t src_end_bit,
		void* const dst,
		const std::size_t dst_start_bit,
		const std::size_t dst_end_bit);

	/* Copies memory from one memory block to another.
	*
	Parameters
	* src: the pointer to the source memory block.
	* dst: the pointer to the destination memory block. This can be the same as src (it won't do anything though).
	* start_bit: the starting bit for both memory blocks' bounds (inclusive).
	* end_bit: the ending bit for both memory blocks' bounds (exclusive).
	*/
	void copy(const void* const src,
		void* const dst,
		const std::size_t start_bit,
		const std::size_t end_bit);
	
	/* Copies memory from one memory block to another.
	*
	Parameters
	* src: the pointer to the source memory block.
	* dst: the pointer to the destination memory block. This can be the same as src.
	* n: the size of both memory blocks in bits. Doesn't have to be a log of 2.
	*/
	void copy(const void* const src,
		void* const dst,
		const std::size_t n);

	/* Compares two memory blocks as if they were numbers.
	* 
	* Don't rely on the values of the numbers aside from them being either positive, negative, or zero.
	* 
	Parameters
	* left: the pointer to the left memory block. This can be the same as right.
	* left_start_bit: the starting bit for the left memory block's bounds (inclusive).
	* left_end_bit: the ending bit for the left memory block's bounds (exclusive).
	* right: the pointer to the right memory block. This can be the same as left.
	* right_start_bit: the starting bit for the right memory block's bounds (inclusive).
	* right_end_bit: the ending bit for the right memory block's bounds (exclusive).
	Returns:
	* a number < 0 if right has a bit set (to true) that left does not.
	* a number > 0 if left has a bit set (to true) that right does not.
	* 0 if left and right are equal.
	*/
	int compare(const void* const left,
		const std::size_t left_start_bit,
		const std::size_t left_end_bit,
		const void* const right,
		const std::size_t right_start_bit,
		const std::size_t right_end_bit);

	/* Compares two memory blocks as if they were numbers.
	*
	* Don't rely on the values of the numbers aside from them being either positive, negative, or zero.
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right (it'll always return 0 though).
	* right: the pointer to the right memory block. This can be the same as left (it'll always return 0 though).
	* start_bit: the starting bit for both memory blocks' bounds (inclusive).
	* right_end_bit: the ending bit for both memory blocks' bounds (exclusive).
	Returns:
	* a number < 0 if right has a bit set (to true) that left does not.
	* a number > 0 if left has a bit set (to true) that right does not.
	* 0 if left and right are equal.
	*/
	int compare(const void* const left,
		const void* const right,
		const std::size_t start_bit,
		const std::size_t end_bit);

	/* Compares two memory blocks as if they were numbers.
	*
	* Don't rely on the values of the numbers aside from them being either positive, negative, or zero.
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right (it'll always return 0 though).
	* right: the pointer to the right memory block. This can be the same as left (it'll always return 0 though).
	* n: the size of both memory blocks in bits. Doesn't have to be a log of 2.
	Returns:
	* a number < 0 if right has a bit set (to true) that left does not.
	* a number > 0 if left has a bit set (to true) that right does not.
	* 0 if left and right are equal.
	*/
	int compare(const void* const left,
		const void* const right,
		const std::size_t n);

	/* Performs the == operation on two memory blocks.
	* 
	Parameters
	* left: the pointer to the left memory block. This can be the same as right.
	* left_start_bit: the starting bit for the left memory block's bounds (inclusive).
	* left_end_bit: the ending bit for the left memory block's bounds (exclusive).
	* right: the pointer to the right memory block. This can be the same left.
	* right_start_bit: the starting bit for the right memory block's bounds (inclusive).
	* right_end_bit: the ending bit for the right memory block's bounds (exclusive).
	* 
	Returns true if all pertinent bits, in both blocks, are the same else returns false.
	*/
	bool equals(const void* const left,
		const std::size_t left_start_bit,
		const std::size_t left_end_bit,
		const void* const right,
		const std::size_t right_start_bit,
		const std::size_t right_end_bit);

	/* Performs the == operation on two memory blocks.
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right.
	* right: the pointer to the right memory block. This can be the same as left.
	* start_bit: the starting bit for both memory blocks' bounds (inclusive).
	* end_bit: the ending bit for both memory blocks' bounds (exclusive).
	*
	Returns true if all pertinent bits, in both blocks, are the same else returns false.
	*/
	bool equals(const void* const left,
		const void* const right,
		const std::size_t start_bit,
		const std::size_t end_bit);

	/* Performs the == operation on two memory blocks.
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right.
	* right: the pointer to the right memory block. This can be the same as left.
	* n: the size of both memory blocks in bits. Doesn't have to be a log of 2.
	* 
	Returns true if all pertinent bits, in both blocks, are the same else returns false.
	*/
	bool equals(const void* left,
		const void* const right,
		const std::size_t n);

	/* Performs an operation similar to bitshift left. It puts the results in the same memory block.
	* This function will ALWAYS shift bits towards the little endian. If you're on a little endian machine (if you don't know, then you probably are), then this is technically backwards.
	*
	Parameters
	* block: the pointer to the memory block.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	*/
	void shift_left(void* const block,
		const std::size_t start_bit,
		const std::size_t end_bit,
		const std::size_t by);

	/* Performs an operation similar to bitshift left. It puts the results in the same memory block.
	* This function will ALWAYS shift bits towards the little endian. If you're on a little endian machine (if you don't know, then you probably are), then this is technically backwards.
	*
	Parameters
	* block: the pointer to the memory block.
	* n: the size of the memory blocks in bits. Doesn't have to be a log of 2.
	*/
	void shift_left(void* const block,
		const std::size_t n,
		const std::size_t by);

	/* Performs an operation similar to bitshift right. It puts the results in the same memory block.
	* This function will ALWAYS shift bits towards the big endian. If you're on a little endian machine (if you don't know, then you probably are), then this is technically backwards.
	*
	Parameters
	* block: the pointer to the memory block.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	*/
	void shift_right(void* const block,
		const std::size_t start_bit,
		const std::size_t end_bit,
		const std::size_t by);

	/* Performs an operation similar to bitshift right. It puts the results in the same memory block.
	* This function will ALWAYS shift bits towards the big endian. If you're on a little endian machine (if you don't know, then you probably are), then this is technically backwards.
	*
	Parameters
	* block: the pointer to the memory block.
	* n: the size of the memory blocks in bits. Doesn't have to be a log of 2.
	*/
	void shift_right(void* const block,
		const std::size_t n,
		const std::size_t by);

	/* Performs a standard all operation on the specified bits in the block.
	* 
	Parameters
	* block: the pointer to the memory block.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	*/
	bool all(const void* const block,
		const std::size_t start_bit,
		const std::size_t end_bit);

	/* Performs a standard all operation on the specified bits in the block.
	*
	Parameters
	* block: the pointer to the memory block.
	* n: the size of the memory blocks in bits. Doesn't have to be a log of 2.
	*/
	bool all(const void* const block,
		const std::size_t n);

	/* Puts a string representation of the binary of the memory block into the supplied buffer.
	Bit 0 will always be the left most number regardless if the machine is big or little endian.
	* 
	Parameters
	* src: pointer to the memory block.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	* buf: the char buffer to put the string into. This should be of size n + 1 (for the null termination char).
	* 
	Note that this does null terminate the string by any means necessary (ie it will overwrite a bit so it can null terminate the string).
	*/
	void str(const void* const src, 
		const std::size_t start_bit,
		const std::size_t end_bit,
		char* const buf);

	/* Puts a wide string representation of the binary of the memory block into the supplied buffer.
	Bit 0 will always be the left most number regardless if the machine is big or little endian.
	*
	Parameters
	* src: pointer to the memory block.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	* buf: the wide char buffer to put the string into. This should be of size n + 1 (for the null termination char).
	*
	Note that this does null terminate the string by any means necessary (ie it will overwrite a bit so it can null terminate the string).
	*/
	void wstr(const void* const src,
		const std::size_t start_bit,
		const std::size_t end_bit,
		wchar_t* const buf);

	/* Puts a string representation of the binary of the memory block into the supplied output stream (character by character).
	Bit 0 will always be the left most number regardless if the machine is big or little endian.
	*
	Parameters
	* src: pointer to the memory block.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	* os: the output stream to put the string into.
	*/
	void str(const void* const src,
		const std::size_t start_bit,
		const std::size_t end_bit,
		std::ostream& os);

	/* Puts a wide string representation of the binary of the memory block into the supplied wide output stream (character by character).
	Bit 0 will always be the left most number regardless if the machine is big or little endian.
	*
	Parameters
	* src: pointer to the memory block.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	* wos: the wide output stream to put the string into.
	*/
	void wstr(const void* const src,
		const std::size_t start_bit,
		const std::size_t end_bit,
		std::wostream& wos);

	std::string str(const void* const src,
		const std::size_t start_bit,
		const std::size_t end_bit
	);

	std::string str(const void* const src,
		const std::size_t n);

	std::wstring wstr(const void* const src,
		const std::size_t start_bit,
		const std::size_t end_bit
	);

	std::wstring wstr(const void* const src,
		const std::size_t n);

	/* Interprets a string made from str() and puts the data into the memory block.
	* 
	Parameters
	* block: pointer to the memory block.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	* s: reference to the string to interpret.
	*/
	void from_str(void* const block,
		const std::size_t start_bit,
		const std::size_t end_bit,
		const std::string& s);

	/* Interprets a string made from str() and puts the data into the memory block.
	*
	Parameters
	* block: pointer to the memory block.
	* n: the size (in bits) of the memory block. Does nothing if this is 0.
	* s: reference to the string to interpret.
	*/
	void from_str(void* const block,
		const std::size_t n,
		const std::string& s);

	/* Interprets a wstring made from wstr() and puts the data into the memory block.
	*
	Parameters
	* block: pointer to the memory block.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	* s: reference to the wstring to interpret.
	*/
	void from_wstr(void* const block,
		const std::size_t start_bit,
		const std::size_t end_bit,
		const std::wstring& s);

	/* Interprets a wstring made from wstr() and puts the data into the memory block.
	*
	Parameters
	* block: pointer to the memory block.
	* n: the size (in bits) of the memory block. Does nothing if this is 0.
	* s: reference to the wstring to interpret.
	*/
	void from_wstr(void* const block,
		const std::size_t n,
		const std::wstring& s);

	/* For each function that iterates over each byte in a memory block.
	*
	Parameters
	* begin: pointer to the beginning of the memory block.
	* end: pointer to the end of the memory block.
	* func: reference to a function that will be used on every byte. Takes in an unsigned char*
	*
	Note that a reverse iteration can be achieved if begin > end.
	*/
	template < class UnaryFunction >
	void for_each_byte(void* const begin, void* const end, UnaryFunction f);

	/* For each function that iterates over each bit in a memory block.
	*
	Parameters
	* begin: pointer to the beginning of the memory block.
	* end: pointer to the end of the memory block.
	* func: pointer to a function that will be used on every bit. The function takes in a bool.
	*
	Note that a reverse iteration can be achieved if begin > end.
	*/
	template < class UnaryFunction >
	void for_each_bit(const void* const begin, const void* const end, UnaryFunction f);

	/* For each function that iterates over each bit in a bounded memory block.
	*/
	template < class UnaryFunction >
	void for_each_bit(
		const void* const begin,
		const std::size_t start_bit,
		const void* const end,
		const std::size_t end_bit,
		UnaryFunction f
	);
};
#else // C++98 or error
#error If you're using VC++, use the /Zc:__cplusplus command line argument. Or you could just remove the preprocessor stuff. That works too. All relevant preprocessor stuff should end with (where XX is the version number) // C++XX
#endif // C++11

#endif // __BITUTILS_H__