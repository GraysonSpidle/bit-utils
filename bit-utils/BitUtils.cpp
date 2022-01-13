#include "BitUtils.h"

#if __cplusplus >= 201100 // C++11
#ifdef CHAR_BIT
constexpr const std::size_t CHAR_SIZE = CHAR_BIT;
#else
#ifdef __CHAR_BIT__
constexpr const std::size_t CHAR_SIZE = __CHAR_BIT__;
#endif // __CHAR_BIT__
#endif // CHAR_BIT

// Checks if the current machine is big endian or little endian. 
#define _BITUTILS_IS_LITTLE_ENDIAN (1 << 1) > 1

inline bool is_bounded(
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	return start_bit != 0 || end_bit != n;
}

inline bool is_soft_bounded(
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	return !is_bounded(n, start_bit, end_bit) && BitUtils::size(n) * CHAR_SIZE != n;
}

inline bool do_bounds_overlap(
	const void* const left,
	const std::size_t left_start_bit,
	const std::size_t left_end_bit,
	const void* const right,
	const std::size_t right_start_bit,
	const std::size_t right_end_bit
) {
	if (left == right)
		return true;
	return (unsigned char*)left + (left_end_bit - left_start_bit) >= right ||
		(unsigned char*)right + (right_end_bit - right_start_bit) >= left;
}

std::size_t log2l(const std::size_t n) {
	std::size_t i = 0;
#if _BITUTILS_IS_LITTLE_ENDIAN
	while (((std::size_t)1 << i) < n) {
#else
	while (((std::size_t)1 >> i) < n) {
#endif
		i++;
	}
	return i;
}

inline void _validateBounds(const std::size_t start_bit, const std::size_t end_bit, const std::size_t i) {
	// 0 <= start_bit < end_bit
	// end_bit - start_bit <= n
	// i < end_bit - start_bit
	if (start_bit >= end_bit) // guarantees start_bit < end_bit
		throw std::invalid_argument("start_bit cannot be >= end_bit");

	// i is the local index for the unbounded/bounded bit array.

	if (i >= end_bit - start_bit)
		throw std::out_of_range("i is out of range for a bounded memory block with " + std::to_string(end_bit - start_bit) + " bits to work with.");
}

inline void _validateBounds(const std::size_t n, const std::size_t i) {
	// 0 <= i < n
	if (n == 0) // guarantees n > 0 (unsigned numbers only)
		throw std::invalid_argument("n cannot be == 0.");
	if (i >= n) {
		if (n != BitUtils::size(n) * CHAR_SIZE)
			throw std::out_of_range("i is out of range for a soft bounded memory block with " + std::to_string(n) + " bits to work with.");
		else
			throw std::out_of_range("i is out of range for an unbounded memory block with " + std::to_string(n) + " bits to work with.");
	}
}

unsigned char* const getPage(void* const src, const std::size_t start_bit, const std::size_t end_bit, const std::size_t i) {
	_validateBounds(start_bit, end_bit, i);
	if (end_bit - start_bit == CHAR_SIZE)
		return (unsigned char*)src;
	else
		return (unsigned char*)src + ((i + start_bit) / CHAR_SIZE);
}

const unsigned char* const getPage(const void* const src, const std::size_t start_bit, const std::size_t end_bit, const std::size_t i) {
	_validateBounds(start_bit, end_bit, i);
	if (end_bit - start_bit == CHAR_SIZE)
		return (unsigned char*)src;
	else
		return (unsigned char*)src + ((i + start_bit) / CHAR_SIZE);
}

unsigned char* const getPage(void* const src, const std::size_t n, const std::size_t i) {
	_validateBounds(n, i);
	if (n == CHAR_SIZE)
		return (unsigned char*)src;
	else
		return (unsigned char*)src + (i / CHAR_SIZE);
}

const unsigned char* const getPage(const void* const src, const std::size_t n, const std::size_t i) {
	_validateBounds(n, i);
	if (n == CHAR_SIZE)
		return (unsigned char*)src;
	else
		return (unsigned char*)src + (i / CHAR_SIZE);
}

void* BitUtils::create(const std::size_t n) {
	return calloc(size(n), 1);
}

// ============ CORE FUNCTIONS ============

inline std::size_t BitUtils::size(const std::size_t n) {
	if (n <= CHAR_SIZE)
		return 1;
	auto temp = (n / CHAR_SIZE) * CHAR_SIZE;
	return (temp / CHAR_SIZE) + (temp < n);
}

std::size_t BitUtils::size(const std::size_t start_bit, const std::size_t end_bit) {
	return size(start_bit) + size(end_bit - start_bit);
}

bool BitUtils::get(const void* const block,
	const std::size_t start_bit,
	const std::size_t end_bit,
	const std::size_t i
) {
#if _BITUTILS_IS_LITTLE_ENDIAN
	return *getPage(block, start_bit, end_bit, i) & ((std::size_t)1 << ((i + start_bit) % CHAR_SIZE));
#else
	return *getPage(block, start_bit, end_bit, i) & ((std::size_t)1 >> ((i + start_bit) % CHAR_SIZE));
#endif
}

bool BitUtils::get(const void* const block,
	const std::size_t n,
	const std::size_t i
) {
#if _BITUTILS_IS_LITTLE_ENDIAN
	return *getPage(block, n, i) & ((std::size_t)1 << (i % CHAR_SIZE));
#else
	return *getPage(block, n, i) & ((std::size_t)1 >> (i % CHAR_SIZE));
#endif
}

void BitUtils::flip(void* const block,
	const std::size_t start_bit,
	const std::size_t end_bit,
	const std::size_t i
) {
#if _BITUTILS_IS_LITTLE_ENDIAN
	*getPage(block, start_bit, end_bit, i) ^= ((std::size_t)1 << ((i + start_bit) % CHAR_SIZE));
#else
	*getPage(block, start_bit, end_bit, i) ^= ((std::size_t)1 >> ((i + start_bit) % CHAR_SIZE));
#endif
}

void BitUtils::flip(void* const block,
	const std::size_t n,
	const std::size_t i
) {
#if _BITUTILS_IS_LITTLE_ENDIAN
	*getPage(block, n, i) ^= ((std::size_t)1 << (i % CHAR_SIZE));
#else
	*getPage(block, n, i) ^= ((std::size_t)1 >> (i  % CHAR_SIZE));
#endif
}

void BitUtils::set(void* const block,
	const std::size_t start_bit,
	const std::size_t end_bit,
	const std::size_t i,
	const bool b
) {
	// Setting the bit to true regardless of its state
#if _BITUTILS_IS_LITTLE_ENDIAN
	*getPage(block, start_bit, end_bit, i) |= (std::size_t)1 << ((i + start_bit) % CHAR_SIZE);
#else
	*getPage(block, start_bit, end_bit, i) |= (std::size_t)1 >> ((i + start_bit) % CHAR_SIZE);
#endif
	// Flipping the bit if user wants it to be false
	if (!b)
		flip(block, start_bit, end_bit, i);
}

void BitUtils::set(void* const block,
	const std::size_t n,
	const std::size_t i,
	const bool b
) {
	// Setting the bit to true regardless of its state
#if _BITUTILS_IS_LITTLE_ENDIAN
	*getPage(block, n, i) |= (std::size_t)1 << (i % CHAR_SIZE);
#else
	*getPage(block, n, i) |= (std::size_t)1 >> (i  % CHAR_SIZE);
#endif
	// Flipping the bit if user wants it to be false
	if (!b)
		flip(block, n, i);
}

// ============ FUNCTIONS ============

void BitUtils::fill(void* const src,
	const std::size_t start_bit,
	const std::size_t end_bit,
	const bool b
) {
	for (std::size_t i = 0; i < end_bit - start_bit; i++) {
		set(src, start_bit, end_bit, i, b);
	}
}

void BitUtils::fill(void* const src,
	const std::size_t n,
	const bool b
) {
	if (size(n) == 1)
		*((unsigned char*)src) = b ? -1 : 0;
	else
		memset(src, b ? (unsigned char)-1 : 0, size(n));
}

void BitUtils::copy(const void* const src,
	const std::size_t src_start_bit,
	const std::size_t src_end_bit,
	void* const dst,
	const std::size_t dst_start_bit,
	const std::size_t dst_end_bit
) {
	if (src == dst &&
		src_start_bit == dst_start_bit &&
		src_end_bit == dst_end_bit
	)
		return;

	std::size_t min_n = ((src_end_bit - src_start_bit) < (dst_end_bit - dst_start_bit))
		? (src_end_bit - src_start_bit)
		: (dst_end_bit - dst_start_bit);

	if (do_bounds_overlap(src, src_start_bit, src_end_bit, dst, dst_start_bit, dst_end_bit)) {
		if (src_start_bit < dst_start_bit) {
			for (std::size_t i = min_n; i > 0; i--) {
				set(dst, dst_start_bit, dst_end_bit, i - 1, get(src, src_start_bit, src_end_bit, i - 1));
			}
		}
		else {
			for (std::size_t i = 0; i < min_n; i++) {
				set(dst, dst_start_bit, dst_end_bit, i, get(src, src_start_bit, src_end_bit, i));
			}
		}
	}
	else {
		fill(dst, dst_start_bit, dst_start_bit + min_n, 0);
		bitwise_or(
			src, src_start_bit, src_start_bit + min_n,
			dst, dst_start_bit, dst_start_bit + min_n,
			dst, dst_start_bit, dst_start_bit + min_n
		);
	}
}

void BitUtils::copy(const void* const src,
	void* const dst,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	if (src == dst)
		return;
	_validateBounds(start_bit, end_bit, 0);
#if defined(__GNUG__) || defined(_CRT_SECURE_NO_WARNINGS) // if you're using gcc or don't want to use memcpy_s
	memcpy(dst, src, size(n));
#else // if you're using vc++
	memcpy_s(dst, size(start_bit, end_bit), src, size(start_bit, end_bit));
#endif
}

void BitUtils::copy(
	const void* const src,
	void* const dst,
	const std::size_t n
) {
	copy(src, dst, 0, n);
}

void BitUtils::bitwise_and(
	const void* const left,
	const std::size_t left_start_bit,
	const std::size_t left_end_bit,
	const void* const right,
	const std::size_t right_start_bit,
	const std::size_t right_end_bit,
	void* const dst,
	const std::size_t dst_start_bit,
	const std::size_t dst_end_bit
) {
	if (left == right &&
		left_start_bit == right_start_bit &&
		left_end_bit == right_end_bit
	) {
		if (left == dst &&
			left_start_bit == dst_start_bit &&
			left_end_bit == dst_end_bit
		)
			return;
	}
	std::size_t min_n = ((left_end_bit - left_start_bit) < (right_end_bit - right_start_bit))
		? (left_end_bit - left_start_bit)
		: (right_end_bit - right_start_bit);
	min_n = (min_n < (dst_end_bit - dst_start_bit))
		? min_n
		: (dst_end_bit - dst_start_bit);

	// I guess I should explain a little bit about what's going on.
	/* So in the event that the user wants to do this function on the same memory block, but
	* in different places, then we might have to do something special.
	* 
	* IFF the destination range is behind the left or right range then we MUST do this in reverse
	* or else we will get incorrect results.
	*/

	std::size_t i = 0;
	int step = 1;
	if (do_bounds_overlap(left, left_start_bit, left_end_bit, right, right_start_bit, right_end_bit) &&
		do_bounds_overlap(left, left_start_bit, left_end_bit, dst, dst_start_bit, dst_end_bit)
	) {
		if (left_start_bit < dst_start_bit || right_start_bit < dst_start_bit) {
			i = min_n;
			step = -1;
		}
	}
	bool l, r;
	for (; (step < 0 ? i > 0 : i < min_n); i += step) {
		l = get(left, left_start_bit, left_start_bit + min_n, (step < 0 ? i - 1 : i));
		r = get(right, right_start_bit, right_start_bit + min_n, (step < 0 ? i - 1 : i));
		set(
			dst, dst_start_bit, dst_start_bit + min_n,
			(step < 0 ? i - 1 : i),
			l & r
		);
	}
}

void BitUtils::bitwise_and(
	const void* const left,
	const std::size_t left_start_bit,
	const std::size_t left_end_bit,
	const bool right,
	void* const dst,
	const std::size_t dst_start_bit,
	const std::size_t dst_end_bit
) {
	// I guess I should explain a little bit about what's going on.
	/* So in the event that the user wants to do this function on the same memory block, but
	* in different places, then we might have to do something special.
	*
	* IFF the destination range is behind the left or right range then we MUST do this in reverse
	* or else we will get incorrect results.
	*/

	std::size_t i = 0;
	int step = 1;
	if (do_bounds_overlap(left, left_start_bit, left_end_bit, dst, dst_start_bit, dst_end_bit)) {
		if (left_start_bit < dst_start_bit) {
			i = left_end_bit - left_start_bit;
			step = -1;
		}
	}
	auto n = left_end_bit - left_start_bit;
	for (; (step < 0 ? i > 0 : i < n); i += step) {
		set(
			dst, dst_start_bit, dst_end_bit,
			(step < 0 ? i - 1 : i),
			get(left, left_start_bit, left_end_bit, (step < 0 ? i - 1 : i)) & right
		);
	}
}

void BitUtils::bitwise_and(
	const void* const left,
	const void* const right,
	void* const dst,
	const std::size_t n
) {

	if (left == right) {
		if (left == dst)
			return;
		copy(
			left, 0, n,
			dst, 0, n
		);
		return;
	}

	for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
		*getPage(dst, n, i) =
			*getPage(left, n, i) &
			*getPage(right, n, i);
	}
}

void BitUtils::bitwise_and(
	const void* const left,
	const bool right,
	void* const dst,
	const std::size_t n
) {
	bitwise_and(left, 0, n, right, dst, 0, n);
}

void BitUtils::bitwise_and(
	const void* const left,
	const void* const right,
	void* const dst,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	bitwise_and(
		left, start_bit, end_bit,
		right, start_bit, end_bit,
		dst, start_bit, end_bit
	);
}

void BitUtils::bitwise_and(
	const void* const left,
	const bool right,
	void* const dst,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	bitwise_and(left, start_bit, end_bit, right, dst, start_bit, end_bit);
}

void BitUtils::bitwise_or(
	const void* const left,
	const std::size_t left_start_bit,
	const std::size_t left_end_bit,
	const void* const right,
	const std::size_t right_start_bit,
	const std::size_t right_end_bit,
	void* const dst,
	const std::size_t dst_start_bit,
	const std::size_t dst_end_bit
) {
	if (left == right) {
		if (left == dst)
			return;
		copy(
			left, left_start_bit, left_end_bit,
			dst, dst_start_bit, dst_end_bit
		);
		return;
	}
	std::size_t min_n = ((left_end_bit - left_start_bit) < (right_end_bit - right_start_bit))
		? (left_end_bit - left_start_bit)
		: (right_end_bit - right_start_bit);
	min_n = (min_n < (dst_end_bit - dst_start_bit))
		? min_n
		: (dst_end_bit - dst_start_bit);
	for (std::size_t i = 0; i < min_n; i++) {
		set(
			dst, dst_start_bit, dst_start_bit + min_n,
			i,
			get(left, left_start_bit, left_start_bit + min_n, i) || get(right, right_start_bit, right_start_bit + min_n, i)
		);
	}
}

void BitUtils::bitwise_or(
	const void* const left,
	const void* const right,
	void* const dst,
	const std::size_t n
) {
	if (left == right) {
		if (left == dst)
			return;
		copy(
			left, 0, n,
			dst, 0, n
		);
		return;
	}
	for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
		*getPage(dst, n, i) =
			*getPage(left, n, i) |
			*getPage(right, n, i);
	}
}

void BitUtils::bitwise_or(const void* const left,
	const void* const right,
	void* const dst,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	bitwise_or(
		left, start_bit, end_bit,
		right, start_bit, end_bit,
		dst, start_bit, end_bit
	);
}

void BitUtils::bitwise_xor(
	const void* const left,
	const std::size_t left_start_bit,
	const std::size_t left_end_bit,
	const void* const right,
	const std::size_t right_start_bit,
	const std::size_t right_end_bit,
	void* const dst,
	const std::size_t dst_start_bit,
	const std::size_t dst_end_bit
) {
	if (left == right &&
		left_start_bit == right_start_bit &&
		left_end_bit == right_end_bit
	) {
		fill(dst, dst_start_bit, dst_end_bit, 0);
		return;
	}

	// I guess I should explain a little bit about what's going on.
	/* So in the event that the user wants to do this function on the same memory block, but
	* in different places, then we might have to do something special.
	* 
	* IFF the destination range is behind the left or right range then we MUST do this in reverse
	* or else we will get incorrect results.
	*/

	std::size_t min_n = ((left_end_bit - left_start_bit) < (right_end_bit - right_start_bit))
		? (left_end_bit - left_start_bit)
		: (right_end_bit - right_start_bit);
	min_n = (min_n < (dst_end_bit - dst_start_bit))
		? min_n
		: (dst_end_bit - dst_start_bit);

	std::size_t i = 0;
	int step = 1;
	if (do_bounds_overlap(left, left_start_bit, left_end_bit, right, right_start_bit, right_end_bit) &&
		do_bounds_overlap(left, left_start_bit, left_end_bit, dst, dst_start_bit, dst_end_bit)
	) {
		if (left_start_bit < dst_start_bit || right_start_bit < dst_start_bit) {
			i = min_n;
			step = -1;
		}
	}
	bool l, r;
	for (; (step < 0 ? i > 0 : i < min_n); i += step) {
		l = get(left, left_start_bit, left_start_bit + min_n, (step < 0 ? i - 1 : i));
		r = get(right, right_start_bit, right_start_bit + min_n, (step < 0 ? i - 1 : i));
		set(
			dst, dst_start_bit, dst_start_bit + min_n,
			(step < 0 ? i - 1 : i),
			l ^ r
		);
	}
}

void BitUtils::bitwise_xor(
	const void* const left,
	const void* const right,
	void* const dst,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	bitwise_xor(
		left, start_bit, end_bit,
		right, start_bit, end_bit,
		dst, start_bit, end_bit
	);
}

void BitUtils::bitwise_xor(const void* const left,
	const void* const right,
	void* const dst,
	const std::size_t n
) {
	if (left == right) {
		fill(dst, n, 0);
		return;
	}
	for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
		*getPage(dst, n, i) =
			*getPage(left, n, i) ^
			*getPage(right, n, i);
	}
}

void BitUtils::bitwise_not(
	const void* const src,
	const std::size_t src_start_bit,
	const std::size_t src_end_bit,
	void* const dst,
	const std::size_t dst_start_bit,
	const std::size_t dst_end_bit
) {
	if (do_bounds_overlap(src, src_start_bit, src_end_bit, dst, dst_start_bit, dst_end_bit)) {
		std::size_t min_n = (dst_end_bit - dst_start_bit) < (src_end_bit - src_start_bit)
			? dst_end_bit - dst_start_bit
			: src_end_bit - src_start_bit;

		std::size_t i = 0;
		int step = 1;
		if (src_start_bit < dst_start_bit) {
			i = min_n;
			step = -1;
		}

		for (; (step < 0 ? i > 0 : i < min_n); i += step) {
			flip(dst, dst_start_bit, dst_end_bit, (step < 0 ? i - 1 : i));
		}
	}
	else {
		fill(dst, dst_start_bit, dst_end_bit, 1);
		bitwise_xor(
			src, src_start_bit, src_end_bit,
			dst, dst_start_bit, dst_end_bit,
			dst, dst_start_bit, dst_end_bit
		);
	}
}

void BitUtils::bitwise_not(
	const void* const src,
	void* const dst,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	bitwise_not(
		src, start_bit, end_bit,
		dst, start_bit, end_bit
	);
}

void BitUtils::bitwise_not(const void* const src,
	void* const dst,
	const std::size_t n
) {
	for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
		*getPage(dst, n, i) = ~(*getPage(src, n, i));
	}
}

void BitUtils::bitwise_not(void* const src,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	bitwise_not(
		src, start_bit, end_bit,
		src, start_bit, end_bit
	);
}

void BitUtils::bitwise_not(void* const src,
	const std::size_t n
) {
	bitwise_not(src, 0, n);
}

bool BitUtils::bool_op(const void* const src,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	// Evaluating each individual bit. Slow, but necessary.
	for (std::size_t i = 0; i < end_bit - start_bit; i++) {
		if (get(src, start_bit, end_bit, i))
			return true;
	}
	return false;
}

bool BitUtils::bool_op(const void* const src,
	const std::size_t n
) {
	for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
		if (*getPage(src, n, i))
			return true;
	}
	return false;
}

bool BitUtils::equals(const void* const left,
	const std::size_t left_start_bit,
	const std::size_t left_end_bit,
	const void* const right,
	const std::size_t right_start_bit,
	const std::size_t right_end_bit
) {
	return 0 == compare(
		left, left_start_bit, left_end_bit,
		right, right_start_bit, right_end_bit
	);
}

bool BitUtils::equals(
	const void* const left,
	const void* const right,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	return equals(left, start_bit, end_bit, right, start_bit, end_bit);
}

bool BitUtils::equals(const void* left,
	const void* const right,
	const std::size_t n
) {
	for (std::size_t i = 0; i < size(n); i++) {
		if (*getPage(left, n, i * CHAR_SIZE) != *getPage(right, n, i * CHAR_SIZE))
			return false;
	}
	return true;
}

void BitUtils::shift_left(
	void* const block,
	const std::size_t start_bit,
	const std::size_t end_bit,
	const std::size_t by
) {
	if (by == 0)
		return;
	if (by >= end_bit - start_bit) {
		fill(block, start_bit, end_bit, 0);
		return;
	}

	copy(
		block, start_bit + by, end_bit,
		block, start_bit, end_bit - by
	);
	fill(block, end_bit - by, end_bit, 0);
}

void BitUtils::shift_left(
	void* const block,
	const std::size_t n,
	const std::size_t by
) {
	shift_left(block, 0, n, by);
}

void BitUtils::shift_right(
	void* const block,
	const std::size_t start_bit,
	const std::size_t end_bit,
	const std::size_t by
) {
	if (by == 0)
		return;
	if (by >= end_bit - start_bit) {
		fill(block, start_bit, end_bit, 0);
		return;
	}

	copy(
		block, start_bit, end_bit - by,
		block, start_bit + by, end_bit
	);
	fill(block, start_bit, start_bit + by, 0);
}

void BitUtils::shift_right(
	void* const block,
	const std::size_t n,
	const std::size_t by
) {
	shift_right(block, 0, n, by);
}

bool BitUtils::all(
	const void* const block,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	for (std::size_t i = 0; i < end_bit - start_bit; i++) {
		if (!get(block, start_bit, end_bit, i))
			return false;
	}
	return true;
}

bool BitUtils::all(const void* const block, const std::size_t n) {
	for (std::size_t i = 0; i < size(n); i++) {
		if (*getPage(block, n, i * CHAR_SIZE) != (unsigned char)-1)
			return false;
	}
	return true;
}

int BitUtils::compare(
	const void* const left,
	const std::size_t left_start_bit,
	const std::size_t left_end_bit,
	const void* const right,
	const std::size_t right_start_bit,
	const std::size_t right_end_bit
) {
	std::size_t min_n = ((left_end_bit - left_start_bit) < (right_end_bit - right_start_bit))
		? (left_end_bit - left_start_bit)
		: (right_end_bit - right_start_bit);
	bool l, r;
	for (std::size_t i = 0; i < min_n; i++) {
		l = get(left, left_start_bit, left_end_bit, i);
		r = get(right, right_start_bit, right_end_bit, i);
		if (l ^ r) { // if l and r are the same then don't prematurely return
			return 0 + (l && !r ? 1 : -1);
		}
	}
	return 0;
}

int BitUtils::compare(const void* const left,
	const void* const right,
	const std::size_t n
) {
	return memcmp(left, right, size(n));
}

int BitUtils::compare(const void* const left,
	const void* const right,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	return compare(
		left, start_bit, end_bit,
		right, start_bit, end_bit);
}

void BitUtils::str(const void* const src,
	const std::size_t src_start_bit,
	const std::size_t src_end_bit,
	char* const buf
) {
	if (!(src_end_bit - src_start_bit) || !strlen(buf))
		return;
	for (std::size_t i = 0; i < ((src_end_bit - src_start_bit) < strlen(buf) ? (src_end_bit - src_start_bit) : strlen(buf)) - 1; i++) {
		buf[i] = get(src, src_start_bit, src_end_bit, i) ? '1' : '0';
	}
	buf[((src_end_bit - src_start_bit) < strlen(buf) ? (src_end_bit - src_start_bit) : strlen(buf))] = '\0';
}	

void BitUtils::wstr(const void* const src,
	const std::size_t src_start_bit,
	const std::size_t src_end_bit,
	wchar_t* const buf
) {
	if (!(src_end_bit - src_start_bit) || !wcslen(buf))
		return;
	for (std::size_t i = 0; i < ((src_end_bit - src_start_bit) < wcslen(buf) ? (src_end_bit - src_start_bit) : wcslen(buf)) - 1; i++) {
		buf[i] = get(src, src_start_bit, src_end_bit, i) ? '1' : '0';
	}
	buf[((src_end_bit - src_start_bit) < wcslen(buf) ? (src_end_bit - src_start_bit) : wcslen(buf))] = L'\0';
}

void BitUtils::str(
	const void* const src,
	const std::size_t start_bit,
	const std::size_t end_bit,
	std::ostream& os
) {
	if (!(end_bit - start_bit))
		return;
	for (std::size_t i = 0; i < end_bit - start_bit; i++) {
		os << get(src, start_bit, end_bit, i) ? '1' : '0';
	}
}

std::string BitUtils::str(
	const void* const src,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	std::stringstream ss;
	str(src, start_bit, end_bit, ss);
	return ss.str();
}

std::string BitUtils::str(const void* const src, const std::size_t n) {
	return str(src, 0, n);
}

void BitUtils::wstr(
	const void* const src,
	const std::size_t start_bit,
	const std::size_t end_bit,
	std::wostream& wos
) {
	if (!(end_bit - start_bit))
		return;
	for (std::size_t i = 0; i < end_bit - start_bit; i++) {
		wos << get(src, start_bit, end_bit, i) ? L'1' : L'0';
	}
}

std::wstring BitUtils::wstr(const void* const src,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	std::wstringstream wss;
	wstr(src, start_bit, end_bit, wss);
	return wss.str();
}

std::wstring BitUtils::wstr(const void* const src, const std::size_t n) {
	return wstr(src, 0, n);
}

void BitUtils::from_str(
	void* const block, 
	const std::size_t start_bit,
	const std::size_t end_bit,
	const std::string& s
) {
	std::size_t min_n = (end_bit - start_bit) < s.length()
		? end_bit - start_bit
		: s.length();

	char c;
	for (std::size_t i = 0; i < min_n; i++) {
		c = s.at(i);
		if (c == '0')
			set(block, start_bit, end_bit, i, 0);
		else if (c == '1')
			set(block, start_bit, end_bit, i, 1);
		else
			throw std::invalid_argument("unrecognized char: " + c );
	}
}

void BitUtils::from_str(void* const block, const std::size_t n, const std::string& s) {
	from_str(block, 0, n, s);
}

void BitUtils::from_wstr(
	void* const block,
	const std::size_t start_bit,
	const std::size_t end_bit,
	const std::wstring& s
) {
	std::size_t min_n = (end_bit - start_bit) < s.length()
		? end_bit - start_bit
		: s.length();

	wchar_t c;
	for (std::size_t i = 0; i < min_n; i++) {
		c = s.at(i);
		if (c == L'0')
			set(block, start_bit, end_bit, i, 0);
		else if (c == L'1')
			set(block, start_bit, end_bit, i, 1);
		else
			throw std::invalid_argument("unrecognized char: " + c);
	}
}

void BitUtils::from_wstr(void* const block, const std::size_t n, const std::wstring& s) {
	from_wstr(block, 0, n, s);
}

template < class UnaryFunction >
void BitUtils::for_each_byte(void* const begin, void* const end, UnaryFunction f) {
	bool reverse = begin > end;

	for (unsigned char* it = (unsigned char*)(reverse ? end : begin); it != (reverse ? begin : end); it += reverse ? -1 : 1) {
		f(reverse ? it - 1 : it);
	}
}

template < class UnaryFunction >
void BitUtils::for_each_bit(const void* const begin, const void* const end, UnaryFunction f) {
	bool reverse = begin > end;

	for (unsigned char* it = (unsigned char*)(reverse ? end : begin); it != (reverse ? begin : end); it += reverse ? -1 : 1) {
		for (unsigned char i = (reverse ? CHAR_SIZE : 0); (reverse ? i > 0 : i < CHAR_SIZE); i += reverse ? -1 : 1) {
			f(get(it, CHAR_SIZE, (reverse ? i - 1 : i)));
		}
	}
}

template<class UnaryFunction>
void BitUtils::for_each_bit(
	const void* const begin,
	const std::size_t start_bit,
	const void* const end,
	const std::size_t end_bit,
	UnaryFunction f
) {

	// We are going to convert our parameters so we can represent this as one whole range (to make it easier to iterate over)
	// We accomplish this through a formula I have derived:
	// n = CHAR_SIZE * (real_end - real_begin) + real_end_bit - real_start_bit
	// If in the case of a reverse iteration, then we just swap the values of between the start/begin and end values respectively.

	auto real_begin = (unsigned char*)begin + (start_bit / CHAR_SIZE); 
	auto real_start_bit = start_bit % CHAR_SIZE;
	auto real_end = (unsigned char*)end + (end_bit / CHAR_SIZE);
	auto real_end_bit = end_bit % CHAR_SIZE;

	if (real_begin == real_end && real_start_bit == real_end_bit)
		throw std::invalid_argument("start_bit refers to the same bit as end_bit, and that isn't allowed.");

	bool reverse = real_begin > real_end;
	auto n = CHAR_SIZE * (reverse ? real_begin - real_end : real_end - real_begin)
		+ (reverse ? real_start_bit : real_end_bit)
		- (reverse ? real_end_bit : real_start_bit);

	// the bounds will be as follows:
	// Not reverse: n, real_start_bit, real_start_bit + n
	// Reverse:     n, real_end_bit, real_end_bit + n

	for (std::size_t i = (reverse ? n : 0); (reverse ? i > 0 : i < n); i += (reverse ? -1 : 1)) {
		f(get(
			reverse ? real_end : real_begin,
			reverse ? real_end_bit : real_start_bit,
			(reverse ? real_end_bit : real_start_bit) + n,
			reverse ? i - 1 : i
		));
	}
}

#undef _BITUTILS_IS_LITTLE_ENDIAN
#endif // C++11