/* What I need to test:
* Functions give proper outputs (duh).
* Ensure the SFINAE works.
* Catching potentially destructive behavior.
*/


#ifndef BITUTILS_TESTS_H
#define BITUTILS_TESTS_H

#include <cassert>
#include <cstdio>
#include <iostream>

#include "BitUtils2.h"

using namespace std;

namespace BitUtilsTests {

	static void testFill() {
		typedef BitUtils<8> BU;
		char c = 127;
		printf_s("BitUtils<%d,%d,%d>\n", 8, 0, 8);

		cout << "Filling with 0s: ";
		BU::fill(&c, 0);
		assert(c == -128);
		cout << "passed" << endl << endl;




	}



}
#endif // BITUTILS_TESTS_H
