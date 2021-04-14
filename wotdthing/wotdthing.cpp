// wotdthing.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "BitUtils2.h"

using namespace std;


int main(int argc, char * argv[]) {
	constexpr const std::size_t n = 10;
	typedef BitUtils<n> Thing;

	void* block = Thing::create();

	cout << "n = " << n << endl;
	cout << "Size: " << Thing::size << " bytes" << endl;
	cout << "Total bits: " << Thing::unbound::n << endl;
	cout << "Bits to work with: " << Thing::n << endl;
	if constexpr (n != Thing::n) {
		cout << "Start bit: " << Thing::start_bit << endl;
		cout << "End bit: " << Thing::end_bit << endl;
		cout << "Forcibly using safe versions of functions." << endl;
	}
	cout << endl;

	cout << "Initial: ";
	Thing::unbound::str(block, cout);
	cout << endl;

	cout << "Safe fill with 1s: ";
	Thing::fill_s(block, 1);
	Thing::str(block, cout);
	cout << endl;

	cout << "Fill with 1s: ";
	Thing::unbound::fill(block, 1);
	Thing::unbound::str(block, cout);
	cout << endl;

	typedef Thing::unbound::bound<1, Thing::unbound::n - 1> Thing2;
	cout << "Fill the center bits with 0s: ";
	Thing2::fill(block, 0);
	Thing::unbound::str(block, cout);
	cout << endl;

	cout << "Performing bitwise not operation: ";
	Thing::unbound::bitwise_not(block);
	Thing::unbound::str(block, cout);
	cout << endl;

	cout << "Performing safe bitwise not operation: ";
	Thing::bitwise_not(block);
	Thing::unbound::str(block, cout);
	cout << endl;

	size_t l = 1212;
	size_t r = 34321;
	cout << "Performing bitwise or operation: ";
	Thing::bitwise_or<Thing::of<size_t>, Thing::of<size_t>, Thing>(&l, &r, block);
	Thing::unbound::str(block, cout);
	cout << endl;

	free(block);

	return 0;
}

