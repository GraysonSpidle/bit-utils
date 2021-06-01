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

		// 1111111111000000

		// bounded test

		BitUtils::fill(block, 16, 5, 10, 0);
		for (std::size_t i = 0; i < 5; i++) {
			assert(!BitUtils::get(block, 16, 5, 10, i));
		}
		for (std::size_t i = 0; i < 5; i++) {
			assert(BitUtils::get(block, 16, 0, 5, i));
		}
		for (std::size_t i = 0; i < 6; i++) {
			assert(!BitUtils::get(block, 16, 10, 16, i));
		}

		// 1111100000000000

		free(block);
	}
	
	void test_fill_s() {
		// Assuming get() and set() work as intended.

		void* block = malloc(2);

		// Dunno what the bit array looks like because we're using malloc

		// unbounded test
		BitUtils::fill_s(block, 16, 0);
		for (std::size_t i = 0; i < 16; i++) {
			assert(!BitUtils::get(block, 16, i));
		}

		// 0000000000000000

		// soft bounded test
		BitUtils::fill_s(block, 10, 1);
		for (std::size_t i = 0; i < 10; i++) {
			assert(BitUtils::get(block, 10, i));
		}
		for (std::size_t i = 10; i < 16; i++) {
			assert(!BitUtils::get(block, 16, i));
		}

		// 1111111111000000

		// bounded test
		BitUtils::fill_s(block, 5, 5, 10, 0);
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

		BitUtils::fill_s(right, 16, 0, 2, 1);

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

		assert(BitUtils::compare_s(left, right, 16) == 0);

		BitUtils::fill_s(left, 16, 5, 10, 1);
		BitUtils::fill_s(right, 16, 6, 11, 1);

		// left:  0000011111000000
		// right: 0000001111100000

		assert(BitUtils::compare_s(left, right, 16, 5, 10) > 0);
		assert(BitUtils::compare_s(left, right, 16, 5, 16) > 0);
		assert(BitUtils::compare_s(left, right, 16, 6, 11) < 0);
		assert(BitUtils::compare_s(left, right, 16, 6, 16) < 0);

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

		for (std::size_t i = 0; i < 10; i++) {
			assert(BitUtils::get(dst, 10, i));
		}

		for (std::size_t i = 10; i < 16; i++) {
			assert(!BitUtils::get(dst, 16, i));
		}

		// src: 1111111111000000
		// dst: 1111111111000000

		free(src);
		free(dst);
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
	}
};

#endif