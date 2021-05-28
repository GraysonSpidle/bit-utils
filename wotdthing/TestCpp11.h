#ifndef TESTCPP11_H
#define TESTCPP11_H

#define __STDC_WANT_LIB_EXT1__ 1
#include "BitUtils.h"
#undef __STDC_WANT_LIB_EXT1__
#include <cassert>

#define IS_LITTLE_ENDIAN (1 << 1) > 1

/* Test in this order:
* get()
* flip()
* set()
* fill()
* fill_s()
* copy()
*/

namespace TestCpp11 {
	void test_get() {
		void* block = malloc(2);
#if IS_LITTLE_ENDIAN
		* ((unsigned char*)block) = 170;
		*((unsigned char*)block + 1) = 170;
#else
		* ((unsigned char*)block) = 85;
		*((unsigned char*)block + 1) = 85;
#endif

		for (std::size_t start = 0; start < 15; start++) {
			for (std::size_t end = start + 1; end <= 16; end++) {
				for (std::size_t i = start; i < end; i++) {
					assert(BitUtils::get(block, 16, i) == !bool(i % 2)); // unbounded
					assert(BitUtils::get(block, i + 1, i) == !bool(i % 2)); // soft bounded
					assert(BitUtils::get(block, end - start, start, end, i) == !bool(i % 2)); // bounded
					assert(BitUtils::get(block, 16, start, end, i) == !bool(i % 2)); // bounded
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
			BitUtils::flip(block, 16, i, i + 1, i);
			assert(!BitUtils::get(block, 16, i));
		}

		// 0000000000000000

		for (std::size_t i = 0; i < 15; i++) { // singleton test 2
			BitUtils::flip(block, 15, i, i + 1, i);
			assert(BitUtils::get(block, 15, i));
		}

		// 1111111111111110

		// singleton test 3
		BitUtils::flip(block, 1, 15, 16, 15);
		assert(BitUtils::get(block, 1, 15, 16, 15));

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
	
	void test_fill_s() {
		// Assuming get() and set() work as intended.

		void* block = malloc(2);

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

	void test_copy() {
		// Assuming get() and fill() work as intended.

		// We're only testing part of copy() because the second part relies on another function
		// that we have yet to test

		void* src = malloc(2);
		void* dst = malloc(2);

		// only testing when both blocks have the same bounds and sizes




		free(src);
		free(dst);
	}

	void test_everything() {
		test_get();
		test_flip();
		test_set();
		test_fill();
		test_fill_s();
		test_copy();
	}
};

#endif