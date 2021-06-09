// Just runs some tests

#include <iostream>
#include <cassert>
#include <iomanip>
#include <cassert>

#include "BitUtils.h"
#include "TestCpp11.h"
#if __cplusplus >= 201700 // C++17
#include "TestCpp17.h"
#endif // C++17

#ifdef CHAR_BIT
constexpr const std::size_t CHAR_SIZE = CHAR_BIT;
#else
#ifdef __CHAR_BIT__
constexpr const std::size_t CHAR_SIZE = __CHAR_BIT__;
#endif // __CHAR_BIT__
#endif // CHAR_BIT

#define IS_LITTLE_ENDIAN (1 << 1) > 1

void testCpp11();
#if __cplusplus >= 201700 // C++17
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

#if __cplusplus >= 201700 // C++17
void testCpp17() {

}
#endif // C++17