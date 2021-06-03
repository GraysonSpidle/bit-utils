#ifndef TESTCPP11_H
#define TESTCPP11_H

#define __STDC_WANT_LIB_EXT1__ 1
#include "BitUtils.h"
#undef __STDC_WANT_LIB_EXT1__
#include <cassert>

#define IS_LITTLE_ENDIAN (1 << 1) > 1

namespace TestCpp11 {
	void test_get() {
		void* block = malloc(2);
#if IS_LITTLE_ENDIAN
		*((unsigned char*)block) = 170;
		*((unsigned char*)block + 1) = 170;
#else
		*((unsigned char*)block) = 85;
		*((unsigned char*)block + 1) = 85;
#endif
		// 01010101

		assert(BitUtils::get(block, 1, 1, 2, 0) == true);

		for (std::size_t start = 0; start < 15; start++) {
			for (std::size_t end = start + 1; end <= 16; end++) {
				for (std::size_t i = start; i < end; i++) {
					assert(BitUtils::get(block, 16, i) == bool(i % 2)); // unbounded
					assert(BitUtils::get(block, i + 1, i) == bool(i % 2)); // soft bounded
					assert(BitUtils::get(block, 1, i, i + 1, 0) == bool(i % 2)); // bounded
					assert(BitUtils::get(block, 16, start, end, i - start) == bool(i % 2)); // bounded
				}
			}
		}

		free(block);
	}

	void test_flip() {
		// Assuming get() works as intended.

		void* block = calloc(2, 1);

		// 0000000000000000

		for (std::size_t i = 0; i < 16; i++) { // unbounded test
			BitUtils::flip(block, 16, i);
			assert(BitUtils::get(block, 16, i));
		}

		// 1111111111111111

		for (std::size_t i = 0; i < 16; i++) { // singleton test
			BitUtils::flip(block, 16, i, i + 1, 0);
			assert(!BitUtils::get(block, 16, i));
		}

		// 0000000000000000

		for (std::size_t i = 0; i < 15; i++) { // singleton test 2
			BitUtils::flip(block, 15, i, i + 1, 0);
			assert(BitUtils::get(block, 15, i));
		}

		// 1111111111111110

		// singleton test 3
		BitUtils::flip(block, 1, 15, 16, 0);
		assert(BitUtils::get(block, 1, 15, 16, 0));

		// 1111111111111111

		for (std::size_t i = 0; i < 10; i++) { // soft bounded test
			BitUtils::flip(block, 10, i);
			assert(!BitUtils::get(block, 10, i));
		}

		// 0000000000111111

		for (std::size_t i = 0; i < 5; i++) { // bounded test
			BitUtils::flip(block, 10, 5, 10, i);
			assert(BitUtils::get(block, 10, 5, 10, i));
		}

		// 0000011111111111

		free(block);
	}

	void test_set() {
		// Assuming get() and flip() work as intended.

		void* block = calloc(2, 1);

		for (std::size_t i = 0; i < 16; i++) { // unbounded test
			BitUtils::set(block, 16, i, i % 2);
			assert(BitUtils::get(block, 16, i) == bool(i % 2));
		}

		for (std::size_t i = 0; i < 10; i++) { // soft bounded test
			BitUtils::set(block, 10, i, 1);
			assert(BitUtils::get(block, 10, i));
		}

		for (std::size_t i = 0; i < 5; i++) { // bounded test
			BitUtils::set(block, 10, 5, 10, i, 0);
			assert(!BitUtils::get(block, 10, 5, 10, i));
		}

		free(block);
	}

	void test_fill() {
		// Assuming get() and set() work as intended.

		void* block = malloc(2);

		// unbounded test
		BitUtils::fill(block, 16, 0);
		for (std::size_t i = 0; i < 16; i++) {
			assert(!BitUtils::get(block, 16, i));
		}

		// 0000000000000000

		// soft bounded test
		BitUtils::fill(block, 10, 1);
		for (std::size_t i = 0; i < 10; i++) {
			assert(BitUtils::get(block, 10, i));
		}

		// 1111111111111111

		// bounded test

		BitUtils::fill(block, 16, 5, 10, 0);
		for (std::size_t i = 0; i < 5; i++) {
			assert(!BitUtils::get(block, 16, 5, 10, i));
		}
		for (std::size_t i = 0; i < 5; i++) {
			assert(BitUtils::get(block, 16, 0, 5, i));
		}
		for (std::size_t i = 0; i < 6; i++) {
			assert(BitUtils::get(block, 16, 10, 16, i));
		}

		// 1111100000000000

		free(block);
	}
	
	void test_fill_s() {
		// Assuming get() and set() work as intended.

		void* block = malloc(2);

		// Dunno what the bit array looks like because we're using malloc

		// unbounded test
		BitUtils::fill(block, 16, 0, 16, 0);
		for (std::size_t i = 0; i < 16; i++) {
			assert(!BitUtils::get(block, 16, i));
		}

		// 0000000000000000

		BitUtils::fill(block, 10, 0, 10, 1);
		for (std::size_t i = 0; i < 10; i++) {
			assert(BitUtils::get(block, 10, i));
		}
		for (std::size_t i = 10; i < 16; i++) {
			assert(!BitUtils::get(block, 16, i));
		}

		// 1111111111000000

		// bounded test
		BitUtils::fill(block, 5, 5, 10, 0);
		for (std::size_t i = 0; i < 5; i++) {
			assert(!BitUtils::get(block, 5, 5, 10, i));
		}
		for (std::size_t i = 0; i < 5; i++) {
			assert(BitUtils::get(block, 5, 0, 5, i));
		}
		for (std::size_t i = 0; i < 6; i++) {
			assert(!BitUtils::get(block, 6, 10, 16, i));
		}

		// 1111100000000000

		free(block);
	}

	void test_compare() {
		void* left = calloc(2, 1);
		void* right = calloc(2, 1);

		// left:  0000000000000000
		// right: 0000000000000000

		assert(BitUtils::compare(left, right, 16) == 0);

		BitUtils::flip(left, 16, 0);

		// left:  1000000000000000
		// right: 0000000000000000

		assert(BitUtils::compare(left, right, 16) > 0);

		BitUtils::fill(right, 16, 0, 2, 1);

		// left:  1000000000000000
		// right: 1100000000000000

		assert(BitUtils::compare(left, right, 16) < 0);

		free(left);
		free(right);
	}

	void test_compare_s() {
		void* left = calloc(2, 1);
		void* right = calloc(2, 1);

		// left:  0000000000000000
		// right: 0000000000000000

		assert(BitUtils::compare(left, right, 16, 0, 16) == 0);

		BitUtils::fill(left, 16, 5, 10, 1);
		BitUtils::fill(right, 16, 6, 11, 1);

		// left:  0000011111000000
		// right: 0000001111100000

		assert(BitUtils::compare(left, right, 16, 5, 10) > 0);
		assert(BitUtils::compare(left, right, 16, 5, 16) > 0);
		assert(BitUtils::compare(left, right, 16, 6, 11) < 0);
		assert(BitUtils::compare(left, right, 16, 6, 16) < 0);

		free(left);
		free(right);
	}

	void test_copy() {
		// Assuming get() and fill() work as intended.

		// We're only testing part of copy() because the second part relies on another function
		// that we have yet to test

		void* src = calloc(2, 1);
		void* dst = malloc(2);

		BitUtils::fill(dst, 16, 1);

		// src: 0000000000000000
		// dst: 1111111111111111

		// only testing when both blocks have the same bounds and sizes

		BitUtils::copy(src, dst, 16);

		// src: 0000000000000000
		// dst: 0000000000000000

		for (std::size_t i = 0; i < 16; i++) {
			assert(!BitUtils::get(dst, 16, i));
		}

		BitUtils::fill(src, 10, 1);

		// src: 1111111111000000
		// dst: 0000000000000000

		BitUtils::copy(src, dst, 10);

		// src: 1111111111000000
		// dst: 1111111111111111

		for (std::size_t i = 0; i < 10; i++) {
			assert(BitUtils::get(dst, 10, i));
		}

		for (std::size_t i = 10; i < 16; i++) {
			assert(BitUtils::get(dst, 16, i));
		}

		BitUtils::copy(src, src, 16);

		assert(BitUtils::compare(src, dst, 16) == 0);

		free(src);
		free(dst);
	}

	void test_bitwise_or() {
		// Assuming get(), fill(), compare(), and the first part of copy() work as intended.
		
		void* left = calloc(2, 1);
		void* right = malloc(2);
		void* dst = malloc(2);

		BitUtils::fill(right, 16, 1);

		// left:  0000000000000000
		// right: 1111111111111111

		BitUtils::bitwise_or(left, right, dst, 16);
		
		// dst:   1111111111111111

		assert(BitUtils::compare(right, dst, 16) == 0);

		BitUtils::bitwise_or(left, left, dst, 16); // this is intentional.

		// left:  0000000000000000
		// right: 1111111111111111
		// dst:   0000000000000000

		assert(BitUtils::compare(left, dst, 16) == 0);

		BitUtils::bitwise_or(left, left, left, 16);

		// nothing changed

		assert(BitUtils::compare(left, dst, 16) == 0);

		free(left);
		free(right);
		free(dst);
	}

	void test_bitwise_or_s() {

		void* left = calloc(2, 1);
		void* right = malloc(2);
		void* dst = malloc(2);

		// left:  0000000000000000
		// right: malloc
		// dst:   malloc

		BitUtils::bitwise_or(left, right, dst, 16, 0, 16);

		// left:  0000000000000000
		// right: malloc
		// dst: should be the same as right

		assert(BitUtils::compare(right, dst, 16) == 0);

		BitUtils::fill(right, 16, 0);
		for (std::size_t i = 0; i < 16; i += 2) {
			BitUtils::flip(right, 16, i);
		}
		
		for (std::size_t i = 1; i < 16; i += 2) {
			BitUtils::flip(left, 16, i);
		}

		BitUtils::fill(dst, 16, 0);

		BitUtils::bitwise_or(left, right, dst, 16, 5, 10);

		// left:  0101010101010101
		// right: 1010101010101010
		// dst:   0000011111000000

		for (std::size_t i = 0; i < 5; i++) {
			assert(BitUtils::get(dst, 16, 5, 10, i));
		}

		BitUtils::bitwise_or(left, left, dst, 16, 0, 16);

		// left:  0101010101010101
		// right: 1010101010101010
		// dst:   0101010101010101

		assert(BitUtils::compare(left, dst, 16) == 0);

		BitUtils::bitwise_or(left, right, right, 16, 5, 10);

		// left:  0101010101010101
		// right: 1010111111101010
		// dst:   0101010101010101

		for (std::size_t i = 0; i < 5; i++) {
			assert(BitUtils::get(right, 16, i) == !bool(i % 2));
		}
		for (std::size_t i = 5; i < 10; i++) {
			assert(BitUtils::get(right, 16, i));
		}
		for (std::size_t i = 10; i < 16; i++) {
			assert(BitUtils::get(right, 16, i) == !bool(i % 2));
		}

		free(left);
		free(right);
		free(dst);
	}

	void test_copy2() {

		void* src = calloc(2, 1);
		void* dst = malloc(2);

		BitUtils::fill(dst, 16, 1);
		BitUtils::copy(src, 16, 0, 16, dst, 16, 0, 5);

		// src: 0000000000000000
		// dst: 0000011111111111

		for (std::size_t i = 0; i < 5; i++) {
			assert(!BitUtils::get(dst, 16, i));
		}

		BitUtils::copy(src, 16, 0, 16, dst, 16, 5, 11);

		// src: 0000000000000000
		// dst: 0000000000011111

		for (std::size_t i = 0; i < 6; i++) {
			assert(!BitUtils::get(dst, 16, 5, 11, i));
		}

		BitUtils::copy(src, 16, 0, 1, dst, 16, 12, 16);

		// src: 0000000000000000
		// dst: 0000000000010111

		assert(BitUtils::get(dst, 16, 11));
		assert(!BitUtils::get(dst, 16, 12));
		for (std::size_t i = 0; i < 3; i++) {
			assert(BitUtils::get(dst, 16, 13, 16, i));
		}

		free(src);
		free(dst);
	}

	void test_bitwise_and() {
		void* left = calloc(2, 1);
		void* right = malloc(2);
		void* dst = malloc(2);

		// left:  0000000000000000
		// right: malloc
		// dst:   malloc

		BitUtils::bitwise_and(left, right, dst, 16);

		// left:  0000000000000000
		// right: malloc
		// dst:   0000000000000000

		for (std::size_t i = 0; i < 16; i++) {
			assert(!BitUtils::get(dst, 16, i));
		}

		BitUtils::fill(right, 16, 0);
		for (std::size_t i = 0; i < 16; i += 2) {
			BitUtils::flip(right, 16, i);
		}

		BitUtils::bitwise_and(left, right, dst, 16);

		// left:  0000000000000000
		// right: 1010101010101010
		// dst:   0000000000000000

		assert(BitUtils::compare(left, dst, 16) == 0);

		BitUtils::fill(dst, 16, 1);
		for (std::size_t i = 1; i < 16; i += 2) {
			BitUtils::flip(left, 16, i);
		}

		BitUtils::bitwise_and(left, right, dst, 16);

		// left:  0101010101010101
		// right: 1010101010101010
		// dst:   0000000000000000

		for (std::size_t i = 0; i < 16; i++) {
			assert(!BitUtils::get(dst, 16, i));
		}

		free(left);
		free(right);
		free(dst);
	}

	void test_bitwise_and_s() {
		void* left = calloc(2, 1);
		void* right = malloc(2);
		void* dst = malloc(2);

		// left:  0000000000000000
		// right: malloc
		// dst:   malloc

		BitUtils::bitwise_and(left, right, dst, 16, 0, 16);

		// left:  0000000000000000
		// right: malloc
		// dst:   0000000000000000

		for (std::size_t i = 0; i < 16; i++) {
			assert(!BitUtils::get(dst, 16, i));
		}

		BitUtils::fill(left, 16, 0);
		BitUtils::fill(left, 16, 5, 10, 1);
		BitUtils::fill(right, 16, 0);
		BitUtils::bitwise_and(left, right, dst, 16, 5, 10);

		// left:  0000011111000000
		// right: 0000000000000000
		// dst:   0000000000000000

		for (std::size_t i = 0; i < 16; i++) {
			assert(!BitUtils::get(dst, 16, i));
		}

		BitUtils::copy(left, 16, 5, 10, right, 16, 6, 11);
		BitUtils::bitwise_and(left, 16, 5, 10, right, 16, 6, 11, dst, 16, 7, 12);

		// left:  0000011111000000
		// right: 0000001111100000
		// dst:   0000000111110000

		for (std::size_t i = 7; i < 12; i++) {
			assert(BitUtils::get(dst, 16, i));
		}

		BitUtils::bitwise_and(left, 16, 5, 10, left, 16, 4, 9, dst, 16, 5, 10);

		// left:  0000011111000000
		// right: 0000001111100000
		// dst:   0000001111110000

		assert(BitUtils::compare(right, dst, 16, 5, 10) == 0);
		assert(BitUtils::get(dst, 16, 11));

		free(left);
		free(right);
		free(dst);
	}

	void test_bitwise_xor() {
		void* left = malloc(2);
		void* right = malloc(2);
		void* dst = malloc(2);

		// left:  malloc
		// right: malloc
		// dst:   malloc

		BitUtils::bitwise_xor(left, left, left, 16); // fun fact: in assembly this is how to zero out memory

		// left:  0000000000000000
		// right: malloc
		// dst:   malloc 

		for (std::size_t i = 0; i < 16; i++) {
			assert(!BitUtils::get(left, 16, i));
		}

		BitUtils::bitwise_xor(left, right, dst, 16);

		// left:  0000000000000000
		// right: malloc
		// dst:   same as right

		assert(BitUtils::compare(right, dst, 16) == 0);

		BitUtils::fill(left, 16, 1);
		BitUtils::fill(right, 16, 1);

		BitUtils::bitwise_xor(left, right, dst, 16);

		// left:  1111111111111111
		// right: 1111111111111111
		// dst:   0000000000000000

		for (std::size_t i = 0; i < 16; i++) {
			assert(!BitUtils::get(dst, 16, i));
		}

		BitUtils::bitwise_xor(dst, dst, dst, 16);

		// nothing changed

		for (std::size_t i = 0; i < 16; i++) {
			assert(!BitUtils::get(dst, 16, i));
		}
	}

	void test_bitwise_xor_s() {
		void* left = malloc(2);
		void* right = malloc(2);
		void* dst = malloc(2);

		// left:  malloc
		// right: malloc
		// dst:   malloc

		BitUtils::bitwise_xor(left, left, left, 16);

		// left:  0000000000000000
		// right: malloc
		// dst:   malloc 

		for (std::size_t i = 0; i < 16; i++) {
			assert(!BitUtils::get(left, 16, i));
		}

		BitUtils::bitwise_xor(right, right, right, 16, 1, 4);
		BitUtils::bitwise_xor(right, right, right, 16, 5, 10);
		BitUtils::bitwise_xor(right, right, right, 16, 12, 15);

		// left:  0000000000000000
		// right: ?000?00000??000?
		// dst:   malloc

		for (std::size_t i = 1; i < 4; i++) {
			assert(!BitUtils::get(right, 16, i));
		}
		for (std::size_t i = 5; i < 10; i++) {
			assert(!BitUtils::get(right, 16, i));
		}
		for (std::size_t i = 12; i < 15; i++) {
			assert(!BitUtils::get(right, 16, i));
		}


		for (std::size_t i = 2; i < 16; i += 2) {
			BitUtils::flip(left, 16, i);
		}

		for (std::size_t i = 3; i < 16; i += 3) {
			BitUtils::flip(left, 16, i);
		}

		BitUtils::fill(right, 16, 0);

		for (std::size_t i = 0; i < 16; i += 4) {
			BitUtils::flip(right, 16, i);
		}

		for (std::size_t i = 5; i < 16; i += 5) {
			BitUtils::flip(right, 16, i);
		}

		BitUtils::bitwise_xor(left, 16, 5, 10, right, 16, 8, 13, dst, 16, 0, 5);

		// left:  0011100011100011
		// right: 1000110010101001
		// dst:   10110???????????

		assert(BitUtils::get(dst, 16, 0));
		assert(!BitUtils::get(dst, 16, 1));
		assert(BitUtils::get(dst, 16, 2));
		assert(BitUtils::get(dst, 16, 3));
		assert(!BitUtils::get(dst, 16, 4));

		free(left);
		free(right);
		free(dst);
	}

	void test_bitwise_not() {
		void* src = calloc(2, 1);
		void* dst = malloc(2);

		BitUtils::bitwise_not(src, dst, 16);

		// src: 0000000000000000
		// dst: 1111111111111111

		for (std::size_t i = 0; i < 16; i++) {
			assert(BitUtils::get(dst, 16, i));
		}

		BitUtils::bitwise_not(src, 10);

		// src: 1111111111000000
		// dst: 1111111111111111

		for (std::size_t i = 0; i < 10; i++) {
			assert(BitUtils::get(dst, 16, i));
		}
		for (std::size_t i = 10; i < 16; i++) {
			assert(BitUtils::get(dst, 16, i));
		}

		free(src);
		free(dst);
	}

	void test_bitwise_not_s() {
		void* src = malloc(2);
		void* dst = malloc(2);

		BitUtils::copy(src, dst, 16);
		BitUtils::bitwise_not(dst, 16);
		BitUtils::bitwise_and(src, dst, dst, 16);

		// src: malloc
		// dst: 0000000000000000

		for (std::size_t i = 0; i < 16; i++) {
			assert(!BitUtils::get(dst, 16, i));
		}

		for (std::size_t i = 0; i < 16; i += 3) {
			BitUtils::bitwise_not(dst, 16, i, (i + 2 < 16 ? i + 2 : 16));
		}

		// src: malloc
		// dst: 1101101101101101

		for (std::size_t i = 2; i < 16; i += 3) {
			assert(!BitUtils::get(dst, 16, i));
		}
		
		for (std::size_t i = 0; i < 16; i += 3) {
			assert(BitUtils::get(dst, 16, i));
			if (i != 15) {
				assert(BitUtils::get(dst, 16, i + 1));
			}
		}

		free(src);
		free(dst);
	}

	void test_bool_op() {
		void* block = calloc(2, 1);

		// block: 0000000000000000

		assert(!BitUtils::bool_op(block, 16));

		BitUtils::flip(block, 16, 1);

		// block: 0100000000000000

		assert(BitUtils::bool_op(block, 16));

		BitUtils::fill(block, 16, 1);

		// block: 1111111111111111
		
		assert(BitUtils::bool_op(block, 16));

		BitUtils::flip(block, 16, 10);

		// block: 1111111111011111

		assert(BitUtils::bool_op(block, 16));

		free(block);
	}

	void test_bool_op_s() {
		void* block = calloc(2, 1);

		// block: 0000000000000000

		assert(!BitUtils::bool_op(block, 16, 0, 16));

		BitUtils::fill(block, 16, 0, 10, 1);

		// block: 1111111111000000

		assert(BitUtils::bool_op(block, 16, 0, 16));
		assert(BitUtils::bool_op(block, 10, 0, 10));
		assert(!BitUtils::bool_op(block, 16, 10, 16));
		
		BitUtils::fill(block, 10, 0);
		BitUtils::fill(block, 16, 10, 16, 1);

		// block: 0000000000111111

		assert(BitUtils::bool_op(block, 16, 0, 16));
		assert(!BitUtils::bool_op(block, 10, 0, 10));
		assert(BitUtils::bool_op(block, 16, 0, 11));
		assert(BitUtils::bool_op(block, 16, 10, 16));

		free(block);
	}

	void test_everything() {
		test_get();
		test_flip();
		test_set();
		test_fill();
		test_fill_s();
		test_compare();
		test_compare_s();
		test_copy();
		test_bitwise_or();
		test_bitwise_or_s();
		test_copy2();
		test_bitwise_and();
		test_bitwise_and_s();
		test_bitwise_xor();
		test_bitwise_xor_s();
		test_bitwise_not();
		test_bitwise_not_s();
		test_bool_op();
		test_bool_op_s();
	}
};

#endif