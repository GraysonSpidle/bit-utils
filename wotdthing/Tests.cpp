// Just runs some tests

#include <iostream>
#include <cassert>
#include <iomanip>
#include <cassert>

#include "BitUtils.h"
#include "TestCpp11.h"

#ifdef CHAR_BIT
constexpr const std::size_t CHAR_SIZE = CHAR_BIT;
#else
#ifdef __CHAR_BIT__
constexpr const std::size_t CHAR_SIZE = __CHAR_BIT__;
#endif // __CHAR_BIT__
#endif // CHAR_BIT

#define IS_LITTLE_ENDIAN (1 << 1) > 1

void testCpp11();
#if 0//__cplusplus >= 201700 // C++17
void testCpp17();
#endif // C++17

int main(int argc, char * argv[]) {
	testCpp11();
	return 0;
}

void printBarThing(std::size_t start_bit, std::size_t n) {
	std::cout << std::setfill(' ') << std::setw(start_bit + 1);
	std::cout << '_';
	std::cout << std::setfill('_') << std::setw(n);
	std::cout << " " << std::endl;
}

void testCpp11() {
	std::cout << "Testing the C++11 version" << std::endl;
	TestCpp11::test_everything();
	std::cout << "All tests passed!" << std::endl;
}

#if 0//__cplusplus >= 201700 // C++17
void testCpp17() {
	typedef BitUtils::BitUtils<10> Thing;

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
		printBarThing(Thing::start_bit, Thing::n);
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

	typedef Thing::bound<Thing::start_bit + 1, Thing::n - 1> Thing2;
	std::cout << "Fill the center bits with 0s: ";
	Thing2::fill(block, 0);
	Thing::unbound::str(block, std::cout);
	std::cout << std::endl;

	std::cout << "Performing bitwise not operation: ";
	Thing::unbound::bitwise_not(block);
	Thing::unbound::str(block, std::cout);
	std::cout << std::endl;

	if constexpr (Thing::is_bounded) {
		std::cout << "Performing safe bitwise not operation: ";
		Thing::bitwise_not_s(block);
		Thing::unbound::str(block, std::cout);
		std::cout << std::endl;
	}
	
	std::cout << "Flipping every other bit: ";
	for (std::size_t i = 0; i < Thing::n; i += 2) {
		Thing::flip(block, i);
	}
	std::cout << Thing::unbound::str(block) << std::endl;

	typedef Thing::bound<Thing::start_bit, Thing::end_bit - 1> BU_src;
	typedef Thing::bound<Thing::start_bit + 1, Thing::end_bit> BU_dst;
	typedef Thing::bound<Thing::start_bit, Thing::start_bit + 1> BU_invert;
	for (std::size_t i = 0; i < BU_src::n; i++) {
		BU_dst::set(block, i, BU_src::get(block, i));
	}
	//BU_invert::fill_s(block, 0);

	Thing::unbound::str(block, std::cout);
	std::cout << std::endl;
	
	/*std::cout << "Performing bitshift left by 2: ";
	Thing::bitshift_left(block, 2);
	Thing::unbound::str(block, std::cout);
	std::cout << std::endl;*/
	
	free(block);
}
#endif // C++17