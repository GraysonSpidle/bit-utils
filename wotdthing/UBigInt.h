#pragma once
#include "BitUtils.h"
#include <cmath>

#ifndef MIN(a,b)
#define MIN(a,b) ((a) < (b) ? : (a) : (b))
#endif

#ifndef MAX(a,b)
#define MAX(a,b) ((a) > (b) ? : (a) : (b))
#endif

using namespace std;

template <const size_t bits>
class UBigInt {
	void* data;
public:
	UBigInt() {
		data = BitUtils::create(bits);
	}

	~UBigInt() {
		BOOLARRAY_FREE(data);
	}

	template <class _Ty, typename = enable_if_t < is_arithmetic_v<_Ty>, bool >>
	bool operator==(const _Ty other) const {
		if (bits > sizeof(_Ty) * CHAR_BIT) {
			for (auto i = sizeof(_Ty) * CHAR_BIT; i < bits; i++) {
				if (BOOLARRAY_GET(data, bits, i))
					return false;
			}
		}
		return *((size_t*)_BOOLARRAY_OFFSET(data, bits - 1)) == other;
	}

	template <const size_t other_bits = bits> 
	bool operator==(const UBigInt<other_bits>& other) const {
		if (bits != other_bits) {
			void* toUse = bits < other_bits ? other.getData() : data;
			for (auto i = MIN(bits, other_bits); i < MAX(bits, other_bits); i++) {
				if (BOOLARRAY_GET(toUse, MAX(bits, other_bits), i))
					return false;
			}
		}

		for (auto i = 0; i < MIN(bits, other_bits); i++) {
			if (bool(BOOLARRAY_GET(data, bits, i)) != bool(BOOLARRAY_GET(*other, other_bits, i)))
				return false;
		}
		return true;
	}

	template <class _Ty, typename = enable_if_t < is_arithmetic_v<_Ty>, bool >>
	bool operator!=(const _Ty other) const {
		return !operator==(other);
	}

	template <const size_t other_bits = bits>
	bool operator!=(const UBigInt<other_bits>& other) const {
		return !operator==(other);
	}

	template <class _Ty, typename = enable_if_t < is_arithmetic_v<_Ty>, bool >>
	bool operator>(const _Ty other) const {
		if (bits > sizeof(_Ty) * CHAR_BIT) {
			for (auto i = sizeof(_Ty) * CHAR_BIT; i < bits; i++) {
				if (BOOLARRAY_GET(data, bits, i))
					return true;
			}
		}
		return *((size_t*)_BOOLARRAY_OFFSET(data, bits - 1)) > other;
	}

	template <const size_t other_bits = bits>
	bool operator>(const UBigInt<other_bits>& other) const {
		if (bits < other_bits) {
			void* toUse = other.getData();
			for (auto i = bits; i < other_bits; i++) {
				if (BOOLARRAY_GET(toUse, other_bits, i))
					return false;
			}
		}

		for (auto i = 0; i < MIN(bits, other_bits); i++) {
			if (bool(BOOLARRAY_GET(data, bits, i)) != bool(BOOLARRAY_GET(other.getData(), other_bits, i)))
				return false;
		}
		return true;
	}

	template <class _Ty, typename = enable_if_t < is_arithmetic_v<_Ty>, bool >>
	bool operator<(const _Ty other) const {
		if (bits > sizeof(_Ty) * CHAR_BIT) {
			for (auto i = sizeof(_Ty) * CHAR_BIT; i < bits; i++) {
				if (BOOLARRAY_GET(data, bits, i))
					return false;
			}
		}
		return *((size_t*)_BOOLARRAY_OFFSET(data, bits - 1)) < other;
	}
	
	template <const size_t other_bits = bits>
	bool operator<(const UBigInt<other_bits>& other) const {
		if (bits > other_bits) {
			void* toUse = data;
			for (auto i = other_bits; i < bits; i++) {
				if (BOOLARRAY_GET(toUse, bits, i))
					return false;
			}
		}

		for (auto i = 0; i < MIN(bits, other_bits); i++) {
			if (bool(BOOLARRAY_GET(data, bits, i)) != bool(BOOLARRAY_GET(other.getData(), other_bits, i)))
				return false;
		}
		return true;
	}

	template <class _Ty, typename = enable_if_t < is_arithmetic_v<_Ty>, bool >>
	bool operator<=(const _Ty other) const { return operator<(other) | operator==(other); }
	template <const size_t other_bits = bits>
	bool operator<=(const UBigInt<other_bits>& other) const { return operator<(other) | operator==(other); }

	template <class _Ty, typename = enable_if_t < is_arithmetic_v<_Ty>, bool >>
	bool operator>=(const _Ty other) const { return operator>(other) | operator==(other); }
	template <const size_t other_bits = bits>
	bool operator>=(const UBigInt<other_bits>& other) const { return operator>(other) | operator==(other); }

	bool operator bool() const {
		if (*((size_t*)data))
			return true;
		if (bits <= sizeof(_Ty) * CHAR_BIT)
			return false;

		for (auto i = 0; i < bits; i++) {
			if (BOOLARRAY_GET(data, bits, i))
				return true;
		}
		return false;
	}

	const void const* getData() const noexcept {
		return data;
	}
};

