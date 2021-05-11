#include "BitUtils.h"

#ifdef CHAR_BIT
constexpr const std::size_t CHAR_SIZE = CHAR_BIT;
#else
#ifdef __CHAR_BIT__
constexpr const std::size_t CHAR_SIZE = __CHAR_BIT__;
#endif // __CHAR_BIT__
#endif // CHAR_BIT

// Preprocessor macro. Checks if the current machine is big endian or little endian. 
#define _BITUTILS_IS_LITTLE_ENDIAN (1 << 1) > 1

inline void _validateBounds(const std::size_t n, const std::size_t start_bit, const std::size_t end_bit, const std::size_t i) {
	bool isBounded = end_bit - start_bit == n;
	if (n == 0)
		throw std::invalid_argument("n must be > 0.");
	if (start_bit >= n)
		throw std::invalid_argument("start_bit cannot be >= n");
	if (end_bit > n)
		throw std::invalid_argument("end_bit cannot be > n");
	if (start_bit >= end_bit)
		throw std::invalid_argument("start_bit cannot be >= end_bit");
	if (i >= end_bit - start_bit) {
		if (isBounded)
			throw std::out_of_range("i is out of range for a bounded memory block of size " + std::to_string(end_bit - start_bit));
		else
			throw std::out_of_range("i is out of range for an unbounded memory block of size " + std::to_string(n));
	}
}

char* const getPage(void* const src, const std::size_t _N, const std::size_t start_bit, const std::size_t end_bit, const std::size_t i) {
	_validateBounds(_N, start_bit, end_bit, i);
	bool isBounded = end_bit - start_bit == _N;
	if (!isBounded && _N == CHAR_SIZE)
		return (char*)src;
	else
		return (char*)src + ((i + start_bit) / CHAR_SIZE);
}

const char* const getPage(const void* const src, const std::size_t _N, const std::size_t start_bit, const std::size_t end_bit, const std::size_t i) {
	_validateBounds(_N, start_bit, end_bit, i);
	bool isBounded = end_bit - start_bit == _N;
	if (!isBounded && _N == CHAR_SIZE)
		return (char*)src;
	else
		return (char*)src + ((i + start_bit) / CHAR_SIZE);
}

void* BitUtils::create(const std::size_t n) {
	return calloc(BitUtils::size(n), 1);
}

std::size_t BitUtils::size(const std::size_t n) {
	if (n <= CHAR_SIZE)
		return 1;
	return (n / CHAR_SIZE) + (n % CHAR_SIZE);
}

bool BitUtils::get(const void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit,
	const std::size_t i
) {
	_validateBounds(n, start_bit, end_bit, i);
#if _BITUTILS_IS_LITTLE_ENDIAN
	return *getPage(src, n, start_bit, end_bit, i) & ((std::size_t)1 << (i % CHAR_SIZE));
#else
	return *getPage(src, n, start_bit, end_bit, i) & ((std::size_t)1 >> (i % CHAR_SIZE));
#endif
}

void BitUtils::flip(void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit,
	const std::size_t i) {
	_validateBounds(n, start_bit, end_bit, i);
#if _BITUTILS_IS_LITTLE_ENDIAN
	* getPage(src, n, start_bit, end_bit, i) ^= ((std::size_t)1 << (i % CHAR_SIZE));
#else
	* getPage(src, n, start_bit, end_bit, i) ^= ((std::size_t)1 >> (i % CHAR_SIZE));
#endif
}

void BitUtils::set(void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit,
	const std::size_t i,
	const bool b) {
	_validateBounds(n, start_bit, end_bit, i);
	// Setting the bit to true regardless of its state
#if _BITUTILS_IS_LITTLE_ENDIAN
	*getPage(src, n, start_bit, end_bit, i) |= ((std::size_t)1 << (i % CHAR_SIZE));
#else
	*getPage(src, n, start_bit, end_bit, i) |= ((std::size_t)1 >> (i % CHAR_SIZE));
#endif
	// Flipping the bit if user wants it to be false
	if (!b)
		BitUtils::flip(src, n, start_bit, end_bit, i);
}

void BitUtils::fill(void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit,
	const bool b
) {
	if (end_bit - start_bit != n)
		BitUtils::fill_s(src, n, start_bit, end_bit, b);
	else if (n == CHAR_SIZE)
		*((unsigned char*)src) = b ? -1 : 0;
	else
		memset(src, b ? (unsigned char)-1 : 0, BitUtils::size(n));
}

void BitUtils::fill_s(void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit,
	const bool b
) {
	if ((end_bit - start_bit) != n) {
		if (n < CHAR_SIZE) {
			char* page = getPage(src, n, start_bit, end_bit, start_bit);
			const std::size_t bounds[3] = {
				n,
				start_bit % CHAR_SIZE,
				(CHAR_SIZE <= n ? CHAR_SIZE : (end_bit % CHAR_SIZE))
			};

			for (std::size_t i = 0; i < (bounds[2] - bounds[1]); i++) {
				set(page, bounds[0], bounds[1], bounds[2], i, b);
			}
		}
		else {
			char* page;
			if (start_bit) {
				page = getPage(src, n, start_bit, end_bit, start_bit);
				std::size_t bounds[3] = {
					CHAR_SIZE,
					start_bit % CHAR_SIZE,
					CHAR_SIZE
				};

				for (std::size_t i = 0; i < bounds[0]; i++) {
					set(page, bounds[0], bounds[1], bounds[2], i, b);
				}
			}

			page = getPage(src, n, start_bit, end_bit, start_bit);
			if (end_bit < n) {
				for (std::size_t i = 1; i < size(n) - 1; i++) {
					fill(page + i, CHAR_SIZE, b);
				}
				page = getPage(src, n, start_bit, end_bit, end_bit - 1);
				std::size_t bounds[3] = {
					CHAR_SIZE,
					0,
					end_bit % CHAR_SIZE
				};

				for (std::size_t i = 0; i < bounds[2] - bounds[1]; i++) {
					set(page, bounds[0], i, b);
				}
			}
			else {
				for (std::size_t i = 1; i < size(n); i++) {
					fill(page + i, n, b);
				}
			}
		}
	}
	else {
		if ((size(n) * CHAR_SIZE) == n)
			fill(src, n, start_bit, end_bit, b);
		else {
			for (std::size_t i = 0; i < end_bit - start_bit; i++) {
				set(src, n, start_bit, end_bit, i, b);
			}
		}
	}
}

void BitUtils::copy(const void* const src,
	const std::size_t src_n,
	const std::size_t src_start_bit,
	const std::size_t src_end_bit,
	void* const dst,
	const std::size_t dst_n,
	const std::size_t dst_start_bit,
	const std::size_t dst_end_bit
) {
	if (src == dst)
		return;

	if (src_n == dst_n &&
		src_start_bit == dst_start_bit &&
		src_end_bit == dst_end_bit) {
#ifdef __STDC_LIB_EXT1__
		memcpy_s(dst, dst_n * CHAR_SIZE, src, src_n * CHAR_SIZE);
		return;
#else
#ifdef __GNUG__
		memcpy(dst, src, src_n * CHAR_SIZE);
		return;
#endif
#endif
	}
	fill(dst, dst_n, dst_start_bit, dst_end_bit, 0);
	bitwise_or(src, src_n, src_start_bit, src_end_bit, dst, dst_n, dst_start_bit, dst_end_bit, dst, dst_n, dst_start_bit, dst_end_bit);
}

void BitUtils::bitwise_and(
	const void* const left,
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
	const std::size_t dst_end_bit
) {
	if (left == right) {
		if (left == dst)
			return;
		copy(left, left_n, left_start_bit, left_end_bit, dst, dst_n, dst_start_bit, dst_end_bit);
		return;
	}
	std::size_t n = (left_n < right_n ? left_n : right_n);
	n = (dst_n < n ? dst_n : n);
	for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
		*getPage(dst, dst_n, dst_start_bit, dst_end_bit, i) = *getPage(left, left_n, left_start_bit, left_end_bit, i) & *getPage(right, right_n, right_start_bit, right_end_bit, i);
	}
}

void BitUtils::bitwise_or(
	const void* const left,
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
	const std::size_t dst_end_bit
) {
	if (left == right) {
		if (left == dst)
			return;
		copy(left, left_n, left_start_bit, left_end_bit, dst, dst_n, dst_start_bit, dst_end_bit);
		return;
	}
	std::size_t n = (left_n < right_n ? left_n : right_n);
	n = (dst_n < n ? dst_n : n);
	for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
		*getPage(dst, dst_n, dst_start_bit, dst_end_bit, i) = *getPage(left, left_n, left_start_bit, left_end_bit, i) | *getPage(right, right_n, right_start_bit, right_end_bit, i);
	}
}

void BitUtils::bitwise_xor(
	const void* const left,
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
	const std::size_t dst_end_bit
) {
	if (left == right) {
		if (left == dst)
			return;
		copy(left, left_n, left_start_bit, left_end_bit, dst, dst_n, dst_start_bit, dst_end_bit);
		return;
	}
	std::size_t n = (left_n < right_n ? left_n : right_n);
	n = (dst_n < n ? dst_n : n);
	for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
		*getPage(dst, dst_n, dst_start_bit, dst_end_bit, i) = *getPage(left, left_n, left_start_bit, left_end_bit, i) ^ *getPage(right, right_n, right_start_bit, right_end_bit, i);
	}
}

void BitUtils::bitwise_not(
	const void* const src,
	const std::size_t src_n,
	const std::size_t src_start_bit,
	const std::size_t src_end_bit,
	void* const dst,
	const std::size_t dst_n,
	const std::size_t dst_start_bit,
	const std::size_t dst_end_bit
) {
	if (src != dst)
		copy(src, src_n, src_start_bit, src_end_bit, dst, dst_n, dst_start_bit, dst_end_bit);
	for (std::size_t i = 0; i < dst_n; i += CHAR_SIZE) {
		*getPage(dst, dst_n, dst_start_bit, dst_end_bit, i) = ~(*getPage(dst, dst_n, dst_start_bit, dst_end_bit, i));
	}
}

bool BitUtils::bool_op(const void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
		if (*getPage(src, n, 0, n, i))
			return true;
	}
	return false;
}

bool BitUtils::bool_op_s(const void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	if (n % CHAR_SIZE) {
		if (bool_op(src, (n - CHAR_SIZE ? n - CHAR_SIZE : CHAR_SIZE))) // We're evaluating all the previous bytes as bools (as it is faster)
			return true;
	}

	// Now evaluating each individual bit. Slow, but necessary.
	for (std::size_t i = 0; i < n % CHAR_SIZE; i++) {
		if (get(src, n, start_bit, end_bit, i))
			return true;
	}
	return false;
}

bool BitUtils::equals(const void* const left,
	const std::size_t left_n,
	const std::size_t left_start_bit,
	const std::size_t left_end_bit,
	const void* const right,
	const std::size_t right_n,
	const std::size_t right_start_bit,
	const std::size_t right_end_bit
) {
	if (left_n > right_n) {
		for (std::size_t i = right_n; i < left_n; i++) {
			if (BitUtils::get(right, right_n, right_start_bit, right_end_bit, i))
				return false;
		}
	}
	else if (left_n < right_n) {
		for (std::size_t i = left_n; i < right_n; i++) {
			if (BitUtils::get(left, left_n, left_start_bit, left_end_bit, i))
				return false;
		}
	}

	for (std::size_t i = 0; i < (left_n < right_n ? left_n : right_n); i++) {
		if (BitUtils::get(left, left_n, left_start_bit, left_end_bit, i) != BitUtils::get(right, right_n, right_start_bit, right_end_bit, i))
			return false;
	}
	return true;
}

void BitUtils::str(const void* const src,
	const std::size_t src_n,
	const std::size_t src_start_bit,
	const std::size_t src_end_bit,
	char* const buf
) {
	if (!src_n || !strlen(buf))
		return;
	for (std::size_t i = 0; i < (src_n < strlen(buf) ? src_n : strlen(buf)) - 1; i++) {
		buf[i] = BitUtils::get(src, src_n, src_start_bit, src_end_bit, i) ? '1' : '0';
	}
	buf[(src_n < strlen(buf) ? src_n : strlen(buf))] = '\0';
}

void BitUtils::wstr(const void* const src,
	const std::size_t src_n,
	const std::size_t src_start_bit,
	const std::size_t src_end_bit,
	wchar_t* const buf,
	const std::size_t buf_n
) {
	if (!src_n || !buf_n)
		return;
	for (std::size_t i = 0; i < (src_n < buf_n ? src_n : buf_n) - 1; i++) {
		buf[i] = BitUtils::get(src, src_n, src_start_bit, src_end_bit, i) ? L'1' : L'0';
	}
	buf[(src_n < buf_n ? src_n : buf_n)] = L'\0';
}

void BitUtils::str(
	const void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit,
	std::ostream& os
) {
	if (!n)
		return;
	for (std::size_t i = 0; i < BitUtils::size(n) * CHAR_SIZE; i++) {
		os << BitUtils::get(src, n, start_bit, end_bit, i) ? '1' : '0';
	}
}

void BitUtils::wstr(
	const void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit,
	std::wostream& wos
) {
	if (!n)
		return;
	for (std::size_t i = 0; i < n; i++) {
		wos << BitUtils::get(src, n, start_bit, end_bit, i) ? L'1' : L'0';
	}
}

void BitUtils::for_each_byte(void* const begin, void* const end, void (*func)(char* pC)) {
	BitUtils::for_each_page<char>(begin, end, func);
}

void BitUtils::for_each_byte(void* const begin, void* const end, std::function<void(char*)>& func) {
	BitUtils::for_each_page<char>(begin, end, func);
}

void BitUtils::for_each_bit(const void* const begin, const void* const end, void (*func)(bool b)) {
	bool reverse = begin > end;

	for (char* it = (char*)(reverse ? end : begin); it != (reverse ? begin : end); it += reverse ? -1 : 1) {
		for (char i = 0; i < CHAR_SIZE; i++) {
			(*func)(BitUtils::get(it, 8, i));
		}
	}
	if (reverse) {
		for (char i = 0; i < CHAR_SIZE; i++) {
			(*func)(BitUtils::get(begin, 8, i));
		}
	}
}

void BitUtils::for_each_bit(const void* const begin, const void* const end, std::function<void(bool)>& func) {
	bool reverse = begin > end;

	for (char* it = (char*)(reverse ? end : begin); it != (reverse ? begin : end); it += reverse ? -1 : 1) {
		for (char i = 0; i < CHAR_SIZE; i++) {
			func(BitUtils::get(it, 8, i));
		}
	}
	if (reverse) {
		for (char i = 0; i < CHAR_SIZE; i++) {
			func(BitUtils::get(begin, 8, i));
		}
	}
}

#undef _BITUTILS_IS_LITTLE_ENDIAN