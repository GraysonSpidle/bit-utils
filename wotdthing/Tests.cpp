// Just runs some tests

#include <iostream>
#include <cassert>
#include <iomanip>

#include "BitUtils.h"

#ifdef CHAR_BIT
constexpr const std::size_t CHAR_SIZE = CHAR_BIT;
#else
#ifdef __CHAR_BIT__
constexpr const std::size_t CHAR_SIZE = __CHAR_BIT__;
#endif // __CHAR_BIT__
#endif // CHAR_BIT

void testCpp11();
#if __cplusplus >= 201700 // C++17
void testCpp17();
#endif // C++17

int main(int argc, char * argv[]) {
#if __cplusplus >= 201700 // C++17
	testCpp17();
#endif // C++17
	testCpp11();
	return 0;
}

void testCpp11() {
	constexpr const std::size_t n = 10;
	constexpr const std::size_t start = 0;
	constexpr const std::size_t end = 8;

	static_assert(n > 0, "n must be greater than 0.");
	static_assert(start < n, "start_bit must be less than n.");
	static_assert(end <= n, "end_bit must be less than or equal to n.");
	static_assert(start < end, "start_bit must be less than end_bit.");

	std::cout << "n = " << n << std::endl;
	std::cout << "Size: " << BitUtils::size(n) << " bytes" << std::endl;
	std::cout << "Total bits: " << BitUtils::size(n) * CHAR_SIZE << std::endl;
	std::cout << "Bits to work with: " << (end - start) << std::endl;
	if ((end - start) != n) {
		std::cout << "Start bit: " << start << std::endl;
		std::cout << "End bit: " << end << std::endl;
		std::cout << "Forcibly using safe versions of functions." << std::endl;
	}
	std::cout << std::endl;
}

#if __cplusplus >= 201700 // C++17
void testCpp17() {
	typedef BitUtils::BitUtils<10, 1, 9> Thing;

	void* block = Thing::create();

	std::cout << "n = " << Thing::n << std::endl;
	std::cout << "Size: " << Thing::size << " bytes" << std::endl;
	std::cout << "Total bits: " << Thing::unbound::n << std::endl;
	std::cout << "Bits to work with: " << Thing::n << std::endl;
	if constexpr (Thing::end_bit - Thing::start_bit != Thing::size * CHAR_SIZE) {
		std::cout << "Start bit: " << Thing::start_bit << std::endl;
		std::cout << "End bit: " << Thing::end_bit << std::endl;
		std::cout << "Forcibly using safe versions of functions." << std::endl;

		std::cout << std::endl;
		std::cout << "These are the bits within the bounds" << std::endl;
		std::cout << std::setfill(' ') << std::setw(Thing::start_bit + 1);
		std::cout << '_';
		std::cout << std::setfill('_') << std::setw(Thing::n);
		std::cout << " " << std::endl;
		Thing::unbound::str(block, std::cout);
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Initial: ";
	Thing::unbound::str(block, std::cout);
	std::cout << std::endl;

	std::cout << "Safe fill with 1s: ";
	Thing::fill_s(block, 1);
	Thing::unbound::str(block, std::cout);
	std::cout << std::endl;

	std::cout << "Fill with 1s: ";
	Thing::unbound::fill(block, 1);
	Thing::unbound::str(block, std::cout);
	std::cout << std::endl;

	typedef Thing::bound<1, Thing::n> Thing2;
	static_assert(Thing::n >= Thing2::n);
	std::cout << "Fill the center bits with 0s: ";
	Thing2::fill(block, 0);
	Thing::unbound::str(block, std::cout);
	std::cout << std::endl;

	std::cout << "Performing bitwise not operation: ";
	Thing::unbound::bitwise_not(block);
	Thing::unbound::str(block, std::cout);
	std::cout << std::endl;

	std::cout << "Performing safe bitwise not operation: ";
	Thing::bitwise_not(block);
	Thing::unbound::str(block, std::cout);
	std::cout << std::endl;

	size_t l = 1212;
	size_t r = 34321;
	std::cout << "Performing bitwise or operation: ";
	Thing::bitwise_or<Thing::of<size_t>, Thing::of<size_t>, Thing>(&l, &r, block);
	Thing::unbound::str(block, std::cout);
	std::cout << std::endl;
	
	free(block);
}
#endif // C++17