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
* The bare minimum language standard is C++11 but for the generic class, you'll need C++17.
* 
* You might have to change some things if you aren't using Visual C++. I tried to do some preprocessor
* stuff to make it more flexible, but I haven't tested this on Linux.
* 
* If you are using Visual C++, then you'll need to add /Zc:__cplusplus to your command line arguments for compilation.
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
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	* start_bit: the starting bit of your bounds (inclusive).
	* end_bit: the ending bit of your bounds (exclusive).
	* i: the local index of the bit you want to get.
	*
	Returns true if the bit is set, and false if it isn't.
	*/
	bool get(const void* const block,
		const std::size_t n,
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
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	* start_bit: the starting bit of your bounds (inclusive).
	* end_bit: the ending bit of your bounds (exclusive).
	* i: the local index of the bit you want to flip.
	*/
	void flip(void* const block,
		const std::size_t n,
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
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	* start_bit: the starting bit of your bounds (inclusive).
	* end_bit: the ending bit of your bounds (exclusive).
	* i: the local index of the bit you want to set.
	* b: the state you want to set the bit to.
	*
	For you performance min-maxers, it is worth noting that setting a bit to false is slower than setting it to true. In general, the flip() function is faster than this.
	*/
	void set(void* const block,
		const std::size_t n,
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
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	* start_bit: the starting bit of your bounds (inclusive).
	* end_bit: the ending bit of your bounds (exclusive).
	* b: the state you want to set all the bits to (true = 1 and false = 0).
	*/
	void fill(void* const block,
		const std::size_t n,
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
	* left_n: the size of the left memory block in bits. Doesn't have to be a log of 2.
	* left_start_bit: the starting bit for the left memory block's bounds (inclusive).
	* left_end_bit: the ending bit for the left memory block's bounds (exclusive).
	* right: the pointer to the right memory block. This can be the same as left or dst.
	* right_n: the size of the right memory block in bits. Doesn't have to be a log of 2.
	* right_start_bit: the starting bit for the right memory block's bounds (inclusive).
	* right_end_bit: the ending bit for the right memory block's bounds (exclusive).
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* dst_n: the size of the dst memory block in bits. Doesn't have to be a log of 2.
	* dst_start_bit: the starting bit for the dst memory block's bounds (inclusive).
	* dst_end_bit: the ending bit for the dst memory block's bounds (exclusive).
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

	/* Does the & bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left & right
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right or dst.
	* right: the pointer to the right memory block. This can be the same as left or dst.
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* n: the size of all 3 memory blocks in bits. Doesn't have to be a log of 2.
	* start_bit: the starting bit for all 3 memory blocks' bounds (inclusive).
	* end_bit: the ending bit for all 3 memory blocks' bounds (exclusive).
	*/
	void bitwise_and(const void* const left,
		const void* const right,
		void* const dst,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit);

	/* Does the | bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left | right
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right or dst.
	* left_n: the size of the left memory block in bits. Doesn't have to be a log of 2.
	* left_start_bit: the starting bit for the left memory block's bounds (inclusive).
	* left_end_bit: the ending bit for the left memory block's bounds (exclusive).
	* right: the pointer to the right memory block. This can be the same as left or dst.
	* right_n: the size of the right memory block in bits. Doesn't have to be a log of 2.
	* right_start_bit: the starting bit for the right memory block's bounds (inclusive).
	* right_end_bit: the ending bit for the right memory block's bounds (exclusive).
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* dst_n: the size of the dst memory block in bits. Doesn't have to be a log of 2.
	* dst_start_bit: the starting bit for the dst memory block's bounds (inclusive).
	* dst_end_bit: the ending bit for the dst memory block's bounds (exclusive).
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

	/* Does the | bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left | right
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right or dst.
	* right: the pointer to the right memory block. This can be the same as left or dst.
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* n: the size of all 3 memory blocks in bits. Doesn't have to be a log of 2.
	* start_bit: the starting bit for all 3 memory blocks' bounds (inclusive).
	* end_bit: the ending bit for all 3 memory blocks' bounds (exclusive).
	*/
	void bitwise_or(const void* const left,
		const void* const right,
		void* const dst,
		const std::size_t n,
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
	* left_n: the size of the left memory block in bits. Doesn't have to be a log of 2.
	* left_start_bit: the starting bit for the left memory block's bounds (inclusive).
	* left_end_bit: the ending bit for the left memory block's bounds (exclusive).
	* right: the pointer to the right memory block. This can be the same as left or dst.
	* right_n: the size of the right memory block in bits. Doesn't have to be a log of 2.
	* right_start_bit: the starting bit for the right memory block's bounds (inclusive).
	* right_end_bit: the ending bit for the right memory block's bounds (exclusive).
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* dst_n: the size of the dst memory block in bits. Doesn't have to be a log of 2.
	* dst_start_bit: the starting bit for the dst memory block's bounds (inclusive).
	* dst_end_bit: the ending bit for the dst memory block's bounds (exclusive).
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

	/* Does the ^ bitwise operation on two memory blocks and puts the result in the destination memory block.
	This is the equivalent of: dst = left ^ right
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right or dst.
	* right: the pointer to the right memory block. This can be the same as left or dst.
	* dst: the pointer to the destination memory block. This can be the same as left or right.
	* n: the size of all 3 memory blocks in bits. Doesn't have to be a log of 2.
	* start_bit: the starting bit for all 3 memory blocks' bounds (inclusive).
	* end_bit: the ending bit for all 3 memory blocks' bounds (exclusive).
	*/
	void bitwise_xor(const void* const left,
		const void* const right,
		void* const dst,
		const std::size_t n,
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
	* src_n: the size of the source memory block in bits. Doesn't have to be a log of 2.
	* src_start_bit: the starting bit for the source memory block's bounds (inclusive).
	* src_end_bit: the ending bit for the source memory block's bounds (exclusive).
	* dst: the pointer to the destination memory block. This can be the same as src.
	* dst_n: the size of the dst memory block in bits. Doesn't have to be a log of 2.
	* dst_start_bit: the starting bit for the dst memory block's bounds (inclusive).
	* dst_end_bit: the ending bit for the dst memory block's bounds (exclusive).
	*/
	void bitwise_not(const void* const src,
		const std::size_t src_n,
		const std::size_t src_start_bit,
		const std::size_t src_end_bit,
		void* const dst,
		const std::size_t dst_n,
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
	* n: the size of both of the memory blocks in bits. Doesn't have to be a log of 2.
	* start_bit: the starting bit for both memory blocks' bounds (inclusive).
	* end_bit: the ending bit for both memory blocks' bounds (exclusive).
	*/
	void bitwise_not(const void* const src,
		void* const dst,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit);

	/* Does the ~ bitwise operation on a memory block and puts the result in the same memory block.
	This is the equivalent of: block = ~block
	*
	Parameters
	* block: the pointer to the source and destination memory block.
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	*/
	void bitwise_not(void* const block,
		const std::size_t n,
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
	* n: the size of the memory block in bits. Doesn't have to be a log of 2.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	Returns false if all the bits are 0 else returns true.
	*/
	bool bool_op(const void* const block,
		const std::size_t n,
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
	* src_n: the size of the source memory block in bits. Doesn't have to be a log of 2.
	* src_start_bit: the starting bit for the source memory block's bounds (inclusive).
	* src_end_bit: the ending bit for the source memory block's bounds (exclusive).
	* dst: the pointer to the destination memory block. This can be the same as src.
	* dst_n: the size of the dst memory block in bits. Doesn't have to be a log of 2.
	* dst_start_bit: the starting bit for the dst memory block's bounds (inclusive).
	* dst_end_bit: the ending bit for the dst memory block's bounds (exclusive).
	*/
	void copy(const void* const src,
		const std::size_t src_n,
		const std::size_t src_start_bit,
		const std::size_t src_end_bit,
		void* const dst,
		const std::size_t dst_n,
		const std::size_t dst_start_bit,
		const std::size_t dst_end_bit);

	/* Copies memory from one memory block to another.
	*
	Parameters
	* src: the pointer to the source memory block.
	* dst: the pointer to the destination memory block. This can be the same as src (it won't do anything though).
	* n: the size of both memory blocks in bits. Doesn't have to be a log of 2.
	* start_bit: the starting bit for both memory blocks' bounds (inclusive).
	* end_bit: the ending bit for both memory blocks' bounds (exclusive).
	*/
	void copy(const void* const src,
		void* const dst,
		const std::size_t n,
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
	* left_n: the size of the left memory block in bits. Doesn't have to be a log of 2.
	* left_start_bit: the starting bit for the left memory block's bounds (inclusive).
	* left_end_bit: the ending bit for the left memory block's bounds (exclusive).
	* right: the pointer to the right memory block. This can be the same as left.
	* right_n: the size of the right memory block in bits. Doesn't have to be a log of 2.
	* right_start_bit: the starting bit for the right memory block's bounds (inclusive).
	* right_end_bit: the ending bit for the right memory block's bounds (exclusive).
	Returns:
	* a number < 0 if right has a bit set (to true) that left does not.
	* a number > 0 if left has a bit set (to true) that right does not.
	* 0 if left and right are equal.
	*/
	int compare(const void* const left,
		const std::size_t left_n,
		const std::size_t left_start_bit,
		const std::size_t left_end_bit,
		const void* const right,
		const std::size_t right_n,
		const std::size_t right_start_bit,
		const std::size_t right_end_bit);

	/* Compares two memory blocks as if they were numbers.
	*
	* Don't rely on the values of the numbers aside from them being either positive, negative, or zero.
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right (it'll always return 0 though).
	* right: the pointer to the right memory block. This can be the same as left (it'll always return 0 though).
	* n: the size of both memory blocks in bits. Doesn't have to be a log of 2.
	* start_bit: the starting bit for both memory blocks' bounds (inclusive).
	* right_end_bit: the ending bit for both memory blocks' bounds (exclusive).
	Returns:
	* a number < 0 if right has a bit set (to true) that left does not.
	* a number > 0 if left has a bit set (to true) that right does not.
	* 0 if left and right are equal.
	*/
	int compare(const void* const left,
		const void* const right,
		const std::size_t n,
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
	* left_n: the size of the left memory block in bits. Doesn't have to be a log of 2.
	* left_start_bit: the starting bit for the left memory block's bounds (inclusive).
	* left_end_bit: the ending bit for the left memory block's bounds (exclusive).
	* right: the pointer to the right memory block. This can be the same left.
	* right_n: the size of the right memory block in bits. Doesn't have to be a log of 2.
	* right_start_bit: the starting bit for the right memory block's bounds (inclusive).
	* right_end_bit: the ending bit for the right memory block's bounds (exclusive).
	* 
	Returns true if all pertinent bits, in both blocks, are the same else returns false.
	*/
	bool equals(const void* const left,
		const std::size_t left_n,
		const std::size_t left_start_bit,
		const std::size_t left_end_bit,
		const void* const right,
		const std::size_t right_n,
		const std::size_t right_start_bit,
		const std::size_t right_end_bit);

	/* Performs the == operation on two memory blocks.
	*
	Parameters
	* left: the pointer to the left memory block. This can be the same as right.
	* right: the pointer to the right memory block. This can be the same as left.
	* n: the size of both memory blocks in bits. Doesn't have to be a log of 2.
	* start_bit: the starting bit for both memory blocks' bounds (inclusive).
	* end_bit: the ending bit for both memory blocks' bounds (exclusive).
	*
	Returns true if all pertinent bits, in both blocks, are the same else returns false.
	*/
	bool equals(const void* const left,
		const void* const right,
		const std::size_t n,
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
	* n: the size of the memory blocks in bits. Doesn't have to be a log of 2.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	*/
	void shift_left(void* const block,
		const std::size_t n,
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
	* n: the size of the memory blocks in bits. Doesn't have to be a log of 2.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	*/
	void shift_right(void* const block,
		const std::size_t n,
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
	* n: the size of the memory blocks in bits. Doesn't have to be a log of 2.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	*/
	bool all(const void* const block,
		const std::size_t n,
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
	* n: the size (in bits) of the memory block. Does nothing if this is 0.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	* buf: the char buffer to put the string into. This should be of size n + 1 (for the null termination char).
	* 
	Note that this does null terminate the string by any means necessary (ie it will overwrite a bit so it can null terminate the string).
	*/
	void str(const void* const src, 
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit,
		char* const buf);

	/* Puts a wide string representation of the binary of the memory block into the supplied buffer.
	Bit 0 will always be the left most number regardless if the machine is big or little endian.
	*
	Parameters
	* src: pointer to the memory block.
	* n: the size (in bits) of the memory block. Does nothing if this is 0.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	* buf: the wide char buffer to put the string into. This should be of size n + 1 (for the null termination char).
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

	/* Puts a string representation of the binary of the memory block into the supplied output stream (character by character).
	Bit 0 will always be the left most number regardless if the machine is big or little endian.
	*
	Parameters
	* src: pointer to the memory block.
	* n: the size (in bits) of the memory block. Does nothing if this is 0.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	* os: the output stream to put the string into.
	*/
	void str(const void* const src,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit,
		std::ostream& os);

	/* Puts a wide string representation of the binary of the memory block into the supplied wide output stream (character by character).
	Bit 0 will always be the left most number regardless if the machine is big or little endian.
	*
	Parameters
	* src: pointer to the memory block.
	* n: the size (in bits) of the memory block. Does nothing if this is 0.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
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
	);

	std::string str(const void* const src,
		const std::size_t n);

	std::wstring wstr(const void* const src,
		const std::size_t n,
		const std::size_t start_bit,
		const std::size_t end_bit
	);

	std::wstring wstr(const void* const src,
		const std::size_t n);

	/* Interprets a string made from str() and puts the data into the memory block.
	* 
	Parameters
	* block: pointer to the memory block.
	* n: the size (in bits) of the memory block. Does nothing if this is 0.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	* s: reference to the string to interpret.
	*/
	void from_str(void* const block,
		const std::size_t n,
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
	* n: the size (in bits) of the memory block. Does nothing if this is 0.
	* start_bit: the starting bit for the memory block's bounds (inclusive).
	* end_bit: the ending bit for the memory block's bounds (exclusive).
	* s: reference to the wstring to interpret.
	*/
	void from_wstr(void* const block,
		const std::size_t n,
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

#if __cplusplus >= 201700 // C++17
	template <
		const std::size_t _n,
		const std::size_t start_bit = 0,
		const std::size_t end_bit = _n,
		std::enable_if_t <
			(_n > 0) &&
			(end_bit > 0) &&
			(start_bit < _n) &&
			(end_bit <= _n) &&
			(start_bit < end_bit),
			bool
		> = true
	>
	class BitUtils {
#define _BITUTILS_MIN(left, right) ((left) < (right) ? (left) : (right))

		template <
			class BitUtils_left = BitUtils,
			class BitUtils_right = BitUtils,
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_left>, bool > = true,
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_right>, bool > = true
		>
		constexpr static const bool do_bounds_overlap(const void* const left, const void* const right) {
			return
				reinterpret_cast<const unsigned char*>(left) + (BitUtils_left::end_bit - BitUtils_left::start_bit) >= right ||
				reinterpret_cast<const unsigned char*>(right) + (BitUtils_right::end_bit - BitUtils_right::start_bit) >= left;
		}

		// Thank you stack overflow! (adapted code from there)

		constexpr static std::size_t floorlog2(std::size_t  x) {
#if _BITUTILS_IS_LITTLE_ENDIAN
			return x == 1 ? 0 : 1 + floorlog2(x >> 1);
#else
			return x == 1 ? 0 : 1 + floorlog2(x << 1);
#endif
		}

		constexpr static std::size_t ceillog2(std::size_t x) {
			return x == 1 ? 0 : floorlog2(x - 1) + 1;
		}

	public:
		constexpr static const std::size_t n = end_bit - start_bit; // The number of bits we're working with.
		constexpr static const std::size_t size = floorlog2(_n); // The number of bytes that would be allocated.
		constexpr static const std::size_t start_bit = start_bit; // The index of the bit to start on (inclusive).
		constexpr static const std::size_t end_bit = end_bit; // The index of the bit to end on (exclusive).
		constexpr static const bool is_bounded = start_bit != 0 || end_bit != _n;
		constexpr static const bool is_soft_bounded = !is_bounded && size * CHAR_SIZE != _n;

		// ========== TYPE DEFS ==========

		/* A typedef that sets bounds for the BitUtils relative to the current bounds (if any are there).
		This guarantees:
		* the safe versions of functions (where available) are forcibly used.
		*/
		template <
			const std::size_t start,
			const std::size_t end,
			std::enable_if_t <
				start >= start_bit &&
				end <= end_bit &&
				start < end,
				bool
			> = true
		>
		using bound = BitUtils<
			_n,
			start,
			end
		>;

		/* A typedef that removes all bounds for the BitUtils.
		It guarantees:
		* _n will be a log of 2.
		* the safe versions of functions (where available) are not forcibly used.
		*/
		typedef BitUtils<size * CHAR_SIZE> unbound;

		/* A typedef that, given a type, makes an unbounded BitUtils class that accomodates for its size. */
		template <
			class Type
		>
		using of = BitUtils<sizeof(Type) * CHAR_SIZE>;

		// ========== CORE FUNCTIONS ==========

		static unsigned char* const getPage(void* const block, const std::size_t i) {
			if constexpr (n == CHAR_SIZE)
				return reinterpret_cast<unsigned char*>(block);
			else if constexpr (is_bounded)
				return reinterpret_cast<unsigned char*>(block) + ((i + start_bit) / CHAR_SIZE);
			else
				return reinterpret_cast<unsigned char*>(block) + (i / CHAR_SIZE);
		}
		static const unsigned char* const getPage(const void* const block, const std::size_t i) {
			if constexpr (n == CHAR_SIZE)
				return reinterpret_cast<const unsigned char*>(block);
			else if constexpr (is_bounded)
				return reinterpret_cast<const unsigned char*>(block) + ((i + start_bit) / CHAR_SIZE);
			else
				return reinterpret_cast<const unsigned char*>(block) + (i / CHAR_SIZE);
		}

		/// <summary>
		/// Gets the designated bit.
		/// </summary>
		/// <param name="block">the pointer to the memory block.</param>
		/// <param name="i">the index of the bit to get.</param>
		/// <returns>true if the bit is 1 and false if the bit 0.</returns>
		static bool get(const void* const block, std::size_t i) {
			if constexpr (_BITUTILS_IS_LITTLE_ENDIAN)
				return *BitUtils::getPage(block, i) & ((std::size_t)1 << ((i + start_bit) % CHAR_SIZE));
			else
				return *BitUtils::getPage(block, i) & ((std::size_t)1 >> ((i + start_bit) % CHAR_SIZE));
		}

		/// <summary>
		/// Flips the designated bit (ie from 0 to 1 or from 1 to 0). It is worth noting this is faster than set().
		/// </summary>
		/// <param name="src">the pointer to the memory block.</param>
		/// <param name="i">the index of the bit to flip.</param>
		static void flip(void* const block, std::size_t i) {
			if constexpr (_BITUTILS_IS_LITTLE_ENDIAN)
				*BitUtils::getPage(block, i) ^= ((std::size_t)1 << ((i + start_bit) % CHAR_SIZE));
			else
				*BitUtils::getPage(block, i) ^= ((std::size_t)1 >> ((i + start_bit) % CHAR_SIZE));
		}

		/// <summary>
		/// Sets the designated bit to reflect the supplied boolean.
		/// </summary>
		/// <param name="src">the pointer to the memory block.</param>
		/// <param name="i">the index of the bit to set.</param>
		/// <param name="b">the boolean that reflects what to set the bit to (ie 1 for true and 0 for false).</param>
		static void set(void* const src, std::size_t i, bool b) {
			if constexpr (_BITUTILS_IS_LITTLE_ENDIAN)
				*BitUtils::getPage(src, i) |= ((std::size_t)1 << ((i + start_bit) % CHAR_SIZE));
			else
				*BitUtils::getPage(src, i) |= ((std::size_t)1 >> ((i + start_bit) % CHAR_SIZE));

			// Flipping the bit if user wants it to be false
			if (!b)
				flip(src, i);
		}

		// ========== FUNCTIONS ==========

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
		/// <param name="block">the pointer to the memory block.</param>
		/// <param name="b">the boolean that reflects what to fll the memory block with (ie 1 for true and 0 false).</param>
		static void fill(void* const block, bool b) {
			if constexpr (is_bounded) {
				for (std::size_t i = 0; i < n; i++) {
					set(block, i, b);
				}
			}
			else if constexpr (size == 1)
				*(reinterpret_cast<unsigned char*>(block)) = b ? -1 : 0;
			else
				memset(block, b ? (unsigned char)-1 : 0, size);
		}

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
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_src>, bool > = true,
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_dst>, bool > = true
		>
		static void copy(const void* const src, void* const dst) {
			if constexpr (
				BitUtils_src::start_bit == BitUtils_dst::start_bit &&
				BitUtils_src::end_bit == BitUtils_dst::end_bit
			) {
				if (src == dst)
					return;
			}

			if constexpr (BitUtils_src::is_bounded || BitUtils_dst::is_bounded) {
				constexpr const std::size_t min_n = (BitUtils_src::n < BitUtils_dst::n)
					? BitUtils_src::n
					: BitUtils_dst::n;
				if (do_bounds_overlap<BitUtils_src, BitUtils_dst>(src, dst)) {
					if constexpr (BitUtils_src::start_bit < BitUtils_dst::start_bit) {
						for (std::size_t i = min_n; i > 0; i--) {
							BitUtils_dst::set(dst, i - 1, BitUtils_src::get(src, i - 1));
						}
					}
					else {
						for (std::size_t i = 0; i < min_n; i++) {
							BitUtils_dst::set(dst, i, BitUtils_src::get(src, i));
						}
					}
				}
				else {
					BitUtils<
						min_n, BitUtils_dst::start_bit, BitUtils_dst::start_bit + min_n
					>::fill(dst, 0);
					bitwise_or<
						BitUtils<BitUtils_src::n, BitUtils_src::start_bit, BitUtils_src::start_bit + min_n>,
						BitUtils<BitUtils_dst::n, BitUtils_dst::start_bit, BitUtils_dst::start_bit + min_n>,
						BitUtils<BitUtils_dst::n, BitUtils_dst::start_bit, BitUtils_dst::start_bit + min_n>
					>(src, dst, dst);
				}
			}
			else { // unbounded
				if (src == dst)
					return;
#if defined(__GNUG__) || defined(_CRT_SECURE_NO_WARNINGS)
				memcpy(dst, src, BitUtils_src::size);
#else
				memcpy_s(dst, BitUtils_dst::size, src, BitUtils_src::size);
#endif
			}
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
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_left>, bool > = true,
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_right>, bool > = true,
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_dst>, bool > = true
		>
		static void bitwise_and(const void* const left, const void* const right, void* const dst) {
			if constexpr (BitUtils_left::is_bounded || BitUtils_right::is_bounded || BitUtils_dst::is_bounded) {
				if constexpr (
					BitUtils_left::n == BitUtils_right::n &&
					BitUtils_left::start_bit == BitUtils_right::start_bit &&
					BitUtils_left::end_bit == BitUtils_right::end_bit
				) {
					if (left == right) {
						if (left == dst)
							return;
						copy<BitUtils_left, BitUtils_dst>(left, dst);
						return;
					}
				}
				constexpr const std::size_t min_n = _BITUTILS_MIN(_BITUTILS_MIN(BitUtils_left::n, BitUtils_right::n), BitUtils_dst::n);

				std::size_t i = 0;
				int step = 1;
				if constexpr (
					do_bounds_overlap<BitUtils_left, BitUtils_right>(left, right) &&
					do_bounds_overlap<BitUtils_left, BitUtils_dst>(left, dst)
				) {
					if constexpr (
						BitUtils_left::start_bit < BitUtils_dst::start_bit ||
						BitUtils_right::start_bit < BitUtils_dst::start_bit
					) {
						i = min_n;
						step = -1;
					}
				}
				bool l, r;
				for (; (step < 0 ? i > 0 : i < min_n); i += step) {
					l = BitUtils<
						BitUtils_left::n,
						BitUtils_left::start_bit,
						BitUtils_left::start_bit + min_n
					>::get(left, (step < 0 ? i - 1 : i));
					r = BitUtils<
						BitUtils_right::n,
						BitUtils_right::start_bit,
						BitUtils_right::start_bit + min_n
					>::get(right, (step < 0 ? i - 1 : i));
					BitUtils<
						BitUtils_dst::n,
						BitUtils_dst::start_bit,
						BitUtils_dst::start_bit + min_n
					>::set(dst, (step < 0 ? i - 1 : i), l & r);
				}
			}
			else {
				if (left == right) {
					if (left == dst)
						return;
					copy<BitUtils_left, BitUtils_dst>(left, dst);
					return;
				}

				for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
					*BitUtils_dst::getPage(dst, i) =
						*BitUtils_left::getPage(left, i) &
						*BitUtils_right::getPage(right, i);
				}
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
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_left>, bool > = true,
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_right>, bool > = true,
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_dst>, bool > = true
		>
		static void bitwise_or(const void* const left, const void* const right, void* const dst) {
			if (left == right) {
				if (left == dst)
					return;
				copy<BitUtils_left, BitUtils_dst>(left, dst);
				return;
			}

			if constexpr (BitUtils_left::is_bounded || BitUtils_right::is_bounded || BitUtils_dst::is_bounded) {
				constexpr const std::size_t min_n = _BITUTILS_MIN(_BITUTILS_MIN(BitUtils_left::n, BitUtils_right::n), BitUtils_dst::n);
				bool l, r;
				for (std::size_t i = 0; i < min_n; i++) {
					l = BitUtils<
						BitUtils_left::n,
						BitUtils_left::start_bit,
						BitUtils_left::start_bit + min_n
					>::get(left, i);
					r = BitUtils<
						BitUtils_right::n,
						BitUtils_right::start_bit,
						BitUtils_right::start_bit + min_n
					>::get(right, i);
					BitUtils<
						BitUtils_dst::n,
						BitUtils_dst::start_bit,
						BitUtils_dst::start_bit + min_n
					>::set(dst, i, l | r);
				}
			}
			else {
				for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
					*BitUtils_dst::getPage(dst, i) =
						*BitUtils_left::getPage(left, i) |
						*BitUtils_right::getPage(right, i);
				}
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
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_left>, bool > = true,
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_right>, bool > = true,
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_dst>, bool > = true
		>
		static void bitwise_xor(const void* const left, const void* const right, void* const dst) {
			if constexpr (BitUtils_left::is_bounded || BitUtils_right::is_bounded || BitUtils_dst::is_bounded) {
				constexpr const std::size_t min_n = _BITUTILS_MIN(_BITUTILS_MIN(BitUtils_left::n, BitUtils_right::n), BitUtils_dst::n);

				std::size_t i = 0;
				int step = 1;
				if constexpr (
					(
						do_bounds_overlap<BitUtils_left, BitUtils_right>(left, right) &&
						do_bounds_overlap<BitUtils_left, BitUtils_dst>(left, dst)
					) &&
					(
						BitUtils_left::start_bit < BitUtils_dst::start_bit ||
						BitUtils_right::start_bit < BitUtils_dst::start_bit
					)
				) {
					i = min_n;
					step = -1;
				}

				bool l, r;
				for (; (step < 0 ? i > 0 : i < min_n); i += step) {
					l = BitUtils<
						BitUtils_left::n,
						BitUtils_left::start_bit,
						BitUtils_left::start_bit + min_n
					>::get(left, (step < 0 ? i - 1 : i));
					r = BitUtils<
						BitUtils_right::n,
						BitUtils_right::start_bit,
						BitUtils_right::start_bit + min_n
					>::get(right, (step < 0 ? i - 1 : i));
					BitUtils<
						BitUtils_dst::n,
						BitUtils_dst::start_bit,
						BitUtils_dst::start_bit + min_n
					>::set(dst, (step < 0 ? i - 1 : i), l ^ r);
				}
			}
			else {
				if (left == right) {
					BitUtils_dst::fill(dst, 0);
					return;
				}
				for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
					*BitUtils_dst::getPage(dst, i) = 
						*BitUtils_left::getPage(left, i) ^ *BitUtils_right::getPage(right, i);
				}
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
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_src>, bool > = true,
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_dst>, bool > = true
		>
		static void bitwise_not(const void* const src, void* const dst) {
			if constexpr (BitUtils_src::is_bounded || BitUtils_dst::is_bounded) {
				if constexpr (do_bounds_overlap<BitUtils_src, BitUtils_dst>(src, dst)) {

					constexpr std::size_t min_n = _BITUTILS_MIN(BitUtils_src::n, BitUtils_dst::n);

					constexpr std::size_t i = (BitUtils_src::start_bit < BitUtils_dst::start_bit) ? min_n : 0;
					constexpr int step = (BitUtils_src::start_bit < BitUtils_dst::start_bit) ? -1 : 1;
					
					for (; (step < 0 ? i > 0 : i < min_n); i += step) {
						BitUtils_dst::flip(dst, (step < 0 ? i - 1 : i));
					}
				}
				else {
					BitUtils_dst::fill(dst, 1);

					bitwise_xor<
						BitUtils_src,
						BitUtils_dst,
						BitUtils_dst
					>(src, dst, dst);
				}
			}
			else { // unbounded
				for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
					*BitUtils_dst::getPage(dst, i) = ~(*BitUtils_dst::getPage(src, i));
				}
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

		static void shift_left(void* const block, const std::size_t amount) {
			if (amount == 0)
				return;
			if (amount >= n) {
				fill(block, 0);
				return;
			}

			copy<
				BitUtils<n, start_bit + amount, end_bit>,
				BitUtils<n, start_bit, end_bit - amount>
			>(block, block);
			BitUtils<n, end_bit - amount, end_bit>::fill(block, 0);
		}

		static void shift_right(void* const block, const std::size_t amount) {
			if (amount == 0)
				return;
			if (amount >= n) {
				fill(block, 0);
				return;
			}

			copy<
				BitUtils<n, start_bit, end_bit - amount>,
				BitUtils<n, start_bit + amount, end_bit>
			>(block, block);
			BitUtils<n, start_bit, start_bit + amount>::fill(block, 0);
		}

		/// <summary>
		/// Performs the bool() operation on the entire memory block (unless BitUtils is bounded, then it defaults to the safe version).
		/// </summary>
		/// <param name="src">the pointer to the source memory block.</param>
		/// <returns>true if any of the bits are 1 and false if all the bits are 0.</returns>
		static bool bool_op(const void* const block) {
			if constexpr (is_bounded) {
				for (std::size_t i = 0; i < n; i++) {
					if (get(block, i))
						return true;
				}
				return false;
			}
			else { // unbounded
				for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
					if (*BitUtils::getPage(block, i))
						return true;
				}
				return false;
			}
		}

		template <
			class BitUtils_left = BitUtils,
			class BitUtils_right = BitUtils,
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_left>, bool > = true,
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_right>, bool > = true
		>
		static bool equals(const void* const left, const void* const right) {
			if constexpr (BitUtils_left::is_bounded || BitUtils_right::is_bounded)
				return 0 == compare<BitUtils_left, BitUtils_right>(left, right);
			else { // unbounded
				for (std::size_t i = 0; i < size; i++) {
					if (bool(*BitUtils_left::getPage(left, i * CHAR_SIZE)) != bool(*BitUtils_right::getPage(right, i * CHAR_SIZE)))
						return false;
				}
				return true;
			}
		}

		template <
			class BitUtils_left = BitUtils,
			class BitUtils_right = BitUtils,
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_left>, bool > = true,
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_right>, bool > = true
		>
		static int compare(const void* const left, const void* const right) {
			if constexpr (BitUtils_left::is_bounded || BitUtils_right::is_bounded) {
				constexpr const std::size_t min_n = _BITUTILS_MIN(BitUtils_left::n, BitUtils_right::n);
				bool l, r;
				for (std::size_t i = 0; i < min_n; i++) {
					l = BitUtils_left::get(left, i);
					r = BitUtils_right::get(right, i);
					if (l ^ r)
						return 0 + (l && !r ? 1 : -1);
				}
				return 0;
			}
			else { // unbounded
				return memcmp(left, right, _BITUTILS_MIN(BitUtils_left::size, BitUtils_right::size));
			}
		}

		static void all(const void* const block) {
			if constexpr (is_bounded) {
				for (std::size_t i = 0; i < n; i++) {
					if (!get(block, i))
						return false;
				}
				return true;
			}
			else { // unbounded
				for (std::size_t i = 0; i < size; i++) {
					if (*BitUtils::getPage(block, i * CHAR_SIZE) != (unsigned char)-1)
						return false;
				}
				return true;
			}
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

		static void from_str(void* const block, const std::string& s) {
			std::size_t min_n = n < s.length() ? n : s.length();

			char c;
			for (std::size_t i = 0; i < min_n; i++) {
				c = s.at(i);
				switch (c) {
				case '0':
					set(block, i, 0);
					break;
				case '1':
					set(block, i, 1);
					break;
				default:
					throw std::invalid_argument("unrecognized char: " + c);
				}
			}
		}

		static void from_wstr(void* const block, const std::wstring& s) {
			std::size_t min_n = n < s.length() ? n : s.length();

			wchar_t c;
			for (std::size_t i = 0; i < min_n; i++) {
				c = s.at(i);
				switch (c) {
				case L'0':
					set(block, i, 0);
					break;
				case L'1':
					set(block, i, 1);
					break;
				default:
					throw std::invalid_argument(L"unrecognized char: " + c);
				}
			}
		}

		template <
			typename FF,
			std::enable_if_t < std::is_invocable_v <FF, unsigned char* const>, bool > = true
		>
		static void for_each_byte(void* const block, const FF& func) {
			for (std::size_t i = 0; i < size; i++) {
				std::invoke(func, BitUtils::getPage(block, i * CHAR_SIZE));
			}
		}

		template <
			typename FF,
			std::enable_if_t < std::is_invocable_v <FF, unsigned char* const>, bool > = true
		>
		static void rfor_each_byte(void* const block, const FF& func) {
			for (std::size_t i = size; i > 0; i--) {
				std::invoke(func, BitUtils::getPage(block, (i - 1) * CHAR_SIZE));
			}
		}

		template <
			typename FF,
			std::enable_if_t < std::is_invocable_v <FF, const unsigned char* const>, bool > = true
		>
		static void for_each_byte(const void* const block, const FF& func) {
			for (std::size_t i = 0; i < size; i++) {
				std::invoke(func, BitUtils::getPage(block, i * CHAR_SIZE));
			}
		}

		template <
			typename FF,
			std::enable_if_t < std::is_invocable_v <FF, const unsigned char* const>, bool > = true
		>
		static void rfor_each_byte(const void* const block, const FF& func) {
			for (std::size_t i = size; i > 0; i--) {
				std::invoke(func, BitUtils::getPage(block, (i - 1) * CHAR_SIZE));
			}
		}

		template <
			typename FF,
			std::enable_if_t < std::is_invocable_v <FF, bool>, bool > = true
		>
		static void for_each_bit(const void* const block, const FF& func) {
			for (std::size_t i = 0; i < n; i++) {
				std::invoke(func, get(block, i));
			}
		}

		template <
			typename FF,
			std::enable_if_t < std::is_invocable_v <FF, bool>, bool > = true
		>
		static void rfor_each_bit(const void* const block, const FF& func) {
			for (std::size_t i = n; i > 0; i--) {
				std::invoke(func, get(block, i - 1));
			}
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
			std::enable_if_t <
				(other_n > 0) &&
				(other_start_bit < other_n) &&
				(other_end_bit <= other_n) &&
				(other_start_bit < other_end_bit) &&
				(other_end_bit - other_start_bit) >= n,
				bool 
			> = true
		>
		operator BitUtils<other_n, other_start_bit, other_end_bit>() const {
			return BitUtils<other_n, other_start_bit, other_end_bit>();
		}
	};
#undef _BITUTILS_MIN
#endif // C++17
#undef _BITUTILS_IS_LITTLE_ENDIAN
};
#else // C++98 or error
#error If you're using VC++, use the /Zc:__cplusplus command line argument. Or you could just remove the preprocessor stuff. That works too. All relevant preprocessor stuff should end with (where XX is the version number) // C++XX
#endif // C++11

#endif // BITUTILS_H