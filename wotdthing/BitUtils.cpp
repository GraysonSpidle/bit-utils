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

inline bool use_safe_function(
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit,
	const char* const func_name
) {
	std::size_t len = strlen(func_name);
	bool safe = std::string(func_name).substr(len - 2) == "_s";

	if (is_bounded(n, start_bit, end_bit))
		return true;
	else if (is_soft_bounded(n, start_bit, end_bit))
		return safe;
	else // not bounded at all
		return false;
}


inline void _validateBounds(const std::size_t n, const std::size_t start_bit, const std::size_t end_bit, const std::size_t i) {
	// 0 <= start_bit < end_bit
	// end_bit - start_bit <= n
	// i < end_bit - start_bit
	if (n == 0) // guarantees n > 0 (unsigned numbers only)
		throw std::invalid_argument("n cannot be == 0.");
	if (start_bit >= end_bit) // guarantees start_bit < end_bit
		throw std::invalid_argument("start_bit cannot be >= end_bit");
	if (end_bit - start_bit > n) // guarantees end_bit - start_bit <= n
		throw std::invalid_argument("end_bit cannot be > n");
	
	// i is the local index for the unbounded/bounded bit array.

	if (i >= end_bit - start_bit) {
		if (is_bounded(n, start_bit, end_bit))
			throw std::out_of_range("i is out of range for a bounded memory block with " + std::to_string(end_bit - start_bit) + " bits to work with.");
		else if (is_soft_bounded(n, start_bit, end_bit))
			throw std::out_of_range("i is out of range for a soft bounded memory block with " + std::to_string(end_bit - start_bit) + " bits to work with.");
		else
			throw std::out_of_range("i is out of range for an unbounded memory block with " + std::to_string(end_bit - start_bit) + " bits to work with.");
	}
}

unsigned char* const getPage(void* const src, const std::size_t _N, const std::size_t start_bit, const std::size_t end_bit, const std::size_t i) {
	_validateBounds(_N, start_bit, end_bit, i);
	if (!is_bounded(_N, start_bit, end_bit) && end_bit - start_bit == CHAR_SIZE)
		return (unsigned char*)src;
	else
		return (unsigned char*)src + ((i + start_bit) / CHAR_SIZE);
}

const unsigned char* const getPage(const void* const src, const std::size_t _N, const std::size_t start_bit, const std::size_t end_bit, const std::size_t i) {
	_validateBounds(_N, start_bit, end_bit, i);
	if (!is_bounded(_N, start_bit, end_bit) && end_bit - start_bit == CHAR_SIZE)
		return (unsigned char*)src;
	else
		return (unsigned char*)src + ((i + start_bit) / CHAR_SIZE);
}

void* BitUtils::create(const std::size_t n) {
	return calloc(size(n), 1);
}

// ============ CORE FUNCTIONS ============

inline std::size_t BitUtils::size(const std::size_t n) {
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
#if _BITUTILS_IS_LITTLE_ENDIAN
	return *getPage(src, n, start_bit, end_bit, i) & ((std::size_t)1 << ((i + start_bit) % CHAR_SIZE));
#else
	return *getPage(src, n, start_bit, end_bit, i) & ((std::size_t)1 >> ((i + start_bit) % CHAR_SIZE));
#endif
}

bool BitUtils::get(const void* const src,
	const std::size_t n,
	const std::size_t i
) {
	return get(src, n, 0, n, i);
}

void BitUtils::flip(void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit,
	const std::size_t i
) {
#if _BITUTILS_IS_LITTLE_ENDIAN
	*getPage(src, n, start_bit, end_bit, i) ^= ((std::size_t)1 << ((i + start_bit) % CHAR_SIZE));
#else
	*getPage(src, n, start_bit, end_bit, i) ^= ((std::size_t)1 >> ((i + start_bit) % CHAR_SIZE));
#endif
}

void BitUtils::flip(void* const src,
	const std::size_t n,
	const std::size_t i
) {
	flip(src, n, 0, n, i);
}

void BitUtils::set(void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit,
	const std::size_t i,
	const bool b
) {
	_validateBounds(n, start_bit, end_bit, i);
	// Setting the bit to true regardless of its state
#if _BITUTILS_IS_LITTLE_ENDIAN
	*getPage(src, n, start_bit, end_bit, i) |= ((std::size_t)1 << ((i + start_bit) % CHAR_SIZE));
#else
	*getPage(src, n, start_bit, end_bit, i) |= ((std::size_t)1 >> ((i + start_bit) % CHAR_SIZE));
#endif
	// Flipping the bit if user wants it to be false
	if (!b)
		flip(src, n, start_bit, end_bit, i);
}

void BitUtils::set(void* const src,
	const std::size_t n,
	const std::size_t i,
	const bool b
) {
	set(src, n, 0, n, i, b);
}

// ============ FUNCTIONS ============

void BitUtils::fill(void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit,
	const bool b
) {
	if (use_safe_function(n, start_bit, end_bit, __func__) || is_soft_bounded(n, start_bit, end_bit)) {
		fill_s(src, n, start_bit, end_bit, b);
		return;
	}

	_validateBounds(n, start_bit, end_bit, 0);
	if (n == CHAR_SIZE)
		*((unsigned char*)src) = b ? -1 : 0;
	else
		memset(src, b ? (unsigned char)-1 : 0, size(n));
}

void BitUtils::fill(void* const src,
	const std::size_t n,
	const bool b
) {
	fill(src, n, 0, n, b);
}

void BitUtils::fill_s(void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit,
	const bool b
) {
	if (use_safe_function(n, start_bit, end_bit, __func__)) {
		for (std::size_t i = 0; i < end_bit - start_bit; i++) {
			set(src, n, start_bit, end_bit, i, b);
		}
	}
	else
		fill(src, n, start_bit, end_bit, b);
}

void BitUtils::fill_s(void* const src,
	const std::size_t n,
	const bool b
) {
	fill_s(src, n, 0, n, b);
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
		src_end_bit == dst_end_bit
	) {
		_validateBounds(src_n, src_start_bit, src_end_bit, 0);
#if defined(__GNUG__) || defined(_CRT_SECURE_NO_WARNINGS) // if you're using gcc or don't want to use memcpy_s
		memcpy(dst, src, size(src_n));
		return;
#else // if you're using vc++
		memcpy_s(dst, size(dst_n), src, size(src_n));
		return;
#endif
	}
	std::size_t min_n = ((src_end_bit - src_start_bit) < (dst_end_bit - dst_start_bit))
		? (src_end_bit - src_start_bit)
		: (dst_end_bit - dst_start_bit);

	fill(dst, min_n, dst_start_bit, dst_start_bit + min_n, 0);
	bitwise_or(
		src, src_n, src_start_bit, src_start_bit + min_n,
		dst, dst_n, dst_start_bit, dst_start_bit + min_n,
		dst, dst_n, dst_start_bit, dst_start_bit + min_n
	);
}

void BitUtils::copy(const void* const src,
	void* const dst,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	copy(
		src, n, start_bit, end_bit,
		dst, n, start_bit, end_bit
	);
}

void BitUtils::copy(
	const void* const src,
	void* const dst,
	const std::size_t n
) {
	copy(src, dst, n, 0, n);
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
	if (
		use_safe_function(left_n, left_start_bit, left_end_bit, __func__) ||
		use_safe_function(right_n, right_start_bit, right_end_bit, __func__) ||
		use_safe_function(dst_n, dst_start_bit, dst_end_bit, __func__)
	) {
		bitwise_and_s(
			left, left_n, left_start_bit, left_end_bit,
			right, right_n, right_start_bit, right_end_bit,
			dst, dst_n, dst_start_bit, dst_end_bit
		);
	}
	else {
		if (left == right) {
			if (left == dst)
				return;
			copy(
				left, left_n, left_start_bit, left_end_bit,
				dst, dst_n, dst_start_bit, dst_end_bit
			);
			return;
		}
		// Finding the minimum n
		std::size_t n = ((left_end_bit - left_start_bit) < (right_end_bit - right_start_bit))
			? (left_end_bit - left_start_bit)
			: (right_end_bit - right_start_bit);
		n = ((dst_end_bit - dst_start_bit) < n)
			? (dst_end_bit - dst_start_bit)
			: n;
		for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
			*getPage(dst, dst_n, dst_start_bit, dst_start_bit + n, i) =
				*getPage(left, left_n, left_start_bit, left_start_bit + n, i) &
				*getPage(right, right_n, right_start_bit, right_start_bit + n, i);
		}
	}
}

void BitUtils::bitwise_and(const void* const left,
	const void* const right,
	void* const dst,
	const std::size_t n
) {
	bitwise_and(
		left, n, 0, n,
		right, n, 0, n,
		dst, n, 0, n
	);
}

void BitUtils::bitwise_and_s(
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
	if (
		!use_safe_function(left_n, left_start_bit, left_end_bit, __func__) &&
		!use_safe_function(right_n, right_start_bit, right_end_bit, __func__) &&
		!use_safe_function(dst_n, dst_start_bit, dst_end_bit, __func__)
	) {
		bitwise_and(
			left, left_n, left_start_bit, left_end_bit,
			right, right_n, right_start_bit, right_end_bit,
			dst, dst_n, dst_start_bit, dst_end_bit
		);
	}
	else {
		if (left == right) {
			if (left == dst)
				return;
			copy(
				left, left_n, left_start_bit, left_end_bit,
				dst, dst_n, dst_start_bit, dst_end_bit
			);
			return;
		}
		std::size_t min_n = ((left_end_bit - left_start_bit) < (right_end_bit - right_start_bit))
			? (left_end_bit - left_start_bit)
			: (right_end_bit - right_start_bit);
		min_n = (min_n < (dst_end_bit - dst_start_bit))
			? min_n
			: (dst_end_bit - dst_start_bit);
		bool l, r;
		for (std::size_t i = 0; i < min_n; i++) {
			l = get(left, left_n, left_start_bit, left_start_bit + min_n, i);
			r = get(right, right_n, right_start_bit, right_start_bit + min_n, i);
			set(
				dst, dst_n, dst_start_bit, dst_start_bit + min_n,
				i,
				l & r
			);
		}
	}
}

void BitUtils::bitwise_and_s(
	const void* const left,
	const void* const right,
	void* const dst,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	bitwise_and_s(
		left, n, start_bit, end_bit,
		right, n, start_bit, end_bit,
		dst, n, start_bit, end_bit
	);
}

void BitUtils::bitwise_and_s(
	const void* const left,
	const void* const right,
	void* const dst,
	const std::size_t n
) {
	bitwise_and_s(left, right, dst, n, 0, n);
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
	if (
		use_safe_function(left_n, left_start_bit, left_end_bit, __func__) ||
		use_safe_function(right_n, right_start_bit, right_end_bit, __func__) ||
		use_safe_function(dst_n, dst_start_bit, dst_end_bit, __func__)
	) {
		bitwise_or_s(
			left, left_n, left_start_bit, left_end_bit,
			right, right_n, right_start_bit, right_end_bit,
			dst, dst_n, dst_start_bit, dst_end_bit
		);
	}
	else {
		if (left == right) {
			if (left == dst)
				return;
			copy(
				left, left_n, left_start_bit, left_end_bit,
				dst, dst_n, dst_start_bit, dst_end_bit
			);
			return;
		}
		std::size_t n = ((left_end_bit - left_start_bit) < (right_end_bit - right_start_bit))
			? (left_end_bit - left_start_bit)
			: (right_end_bit - right_start_bit);
		n = ((dst_end_bit - dst_start_bit) < n)
			? (dst_end_bit - dst_start_bit)
			: n;
		for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
			*getPage(dst, dst_n, dst_start_bit, dst_start_bit + n, i) =
				*getPage(left, left_n, left_start_bit, left_start_bit + n, i) |
				*getPage(right, right_n, right_start_bit, right_start_bit + n, i);
		}
	}
}

void BitUtils::bitwise_or(const void* const left,
	const std::size_t left_n,
	const void* const right,
	const std::size_t right_n,
	void* const dst,
	const std::size_t dst_n
) {
	bitwise_or(
		left, left_n, 0, left_n,
		right, right_n, 0, right_n,
		dst, dst_n, 0, dst_n
	);
}

void BitUtils::bitwise_or_s(
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
	if (
		!use_safe_function(left_n, left_start_bit, left_end_bit, __func__) &&
		!use_safe_function(right_n, right_start_bit, right_end_bit, __func__) &&
		!use_safe_function(dst_n, dst_start_bit, dst_end_bit, __func__)
	) {
		bitwise_or(
			left, left_n, left_start_bit, left_end_bit,
			right, right_n, right_start_bit, right_end_bit,
			dst, dst_n, dst_start_bit, dst_end_bit
		);
	}
	else {
		if (left == right) {
			if (left == dst)
				return;
			copy(
				left, left_n, left_start_bit, left_end_bit,
				dst, dst_n, dst_start_bit, dst_end_bit
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
				dst, dst_n, dst_start_bit, dst_start_bit + min_n,
				i,
				get(left, left_n, left_start_bit, left_start_bit + min_n, i) || get(right, right_n, right_start_bit, right_start_bit + min_n, i)
			);
		}
	}
}

void BitUtils::bitwise_or(
	const void* const left,
	const void* const right,
	void* const dst,
	const std::size_t n
) {
	bitwise_or(
		left, n,
		right, n,
		dst, n
	);
}

void BitUtils::bitwise_or_s(
	const void* const left,
	const void* const right,
	void* const dst,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	bitwise_or_s(
		left, n, start_bit, end_bit,
		right, n, start_bit, end_bit,
		dst, n, start_bit, end_bit
	);
}

void BitUtils::bitwise_or_s(
	const void* const left,
	const void* const right,
	void* const dst,
	const std::size_t n
) {
	bitwise_or_s(left, right, dst, n, 0, n);
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
	if (use_safe_function(left_n, left_start_bit, left_end_bit, __func__) ||
		use_safe_function(right_n, right_start_bit, right_end_bit, __func__) ||
		use_safe_function(dst_n, dst_start_bit, dst_end_bit, __func__)
	) {
		bitwise_xor_s(left, left_n, left_start_bit, left_end_bit,
			right, right_n, right_start_bit, right_end_bit,
			dst, dst_n, dst_start_bit, dst_end_bit);
	}
	else {
		if (left == right) {
			fill(dst, dst_n, dst_start_bit, dst_end_bit, 0);
			return;
		}
		std::size_t n = ((left_end_bit - left_start_bit) < (right_end_bit - right_start_bit))
			? (left_end_bit - left_start_bit)
			: (right_end_bit - right_start_bit);
		n = ((dst_end_bit - dst_start_bit) < n)
			? (dst_end_bit - dst_start_bit)
			: n;
		for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
			*getPage(dst, dst_n, dst_start_bit, dst_start_bit + n, i) =
				*getPage(left, left_n, left_start_bit, left_start_bit + n, i) ^
				*getPage(right, right_n, right_start_bit, right_start_bit + n, i);
		}
	}
}

void BitUtils::bitwise_xor(const void* const left,
	const std::size_t left_n,
	const void* const right,
	const std::size_t right_n,
	void* const dst,
	const std::size_t dst_n
) {
	bitwise_xor(
		left, left_n, 0, left_n,
		right, right_n, 0, right_n,
		dst, dst_n, 0, dst_n
	);
}

void BitUtils::bitwise_xor(const void* const left,
	const void* const right,
	void* const dst,
	const std::size_t n
) {
	bitwise_xor(left, n, 0, n, right, n, 0, n, dst, n, 0, n);
}

void BitUtils::bitwise_xor_s(
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
	if (!use_safe_function(left_n, left_start_bit, left_end_bit, __func__) &&
		!use_safe_function(right_n, right_start_bit, right_end_bit, __func__) &&
		!use_safe_function(dst_n, dst_start_bit, dst_end_bit, __func__)
	) {
		bitwise_xor(
			left, left_n, left_start_bit, left_end_bit,
			right, right_n, right_start_bit, right_end_bit,
			dst, dst_n, dst_start_bit, dst_end_bit
		);
	}
	else {
		if (left == right) {
			fill(dst, dst_n, dst_start_bit, dst_end_bit, 0);
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
				dst, dst_n, dst_start_bit, dst_start_bit + min_n,
				i,
				get(left, left_n, left_start_bit, left_start_bit + min_n, i) ^ get(right, right_n, right_start_bit, right_start_bit + min_n, i)
			);
		}
	}
}

void BitUtils::bitwise_xor_s(
	const void* const left,
	const void* const right,
	void* const dst,
	std::size_t n,
	std::size_t start_bit,
	std::size_t end_bit
) {
	bitwise_xor_s(
		left, n, start_bit, end_bit,
		right, n, start_bit, end_bit,
		dst, n, start_bit, end_bit
	);
}

void BitUtils::bitwise_xor_s(
	const void* const left,
	const void* const right,
	void* const dst,
	std::size_t n
) {
	bitwise_xor_s(left, n, 0, n, right, n, 0, n, dst, n, 0, n);
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
	if (use_safe_function(src_n, src_start_bit, src_end_bit, __func__) ||
		use_safe_function(dst_n, dst_start_bit, dst_end_bit, __func__)
	) {
		bitwise_not_s(src, src_n, src_start_bit, src_end_bit,
			dst, dst_n, dst_start_bit, dst_end_bit);
	}
	else {
		std::size_t min_n = ((src_end_bit - src_start_bit) < (dst_end_bit - dst_start_bit))
			? (src_end_bit - src_start_bit)
			: (dst_end_bit - dst_start_bit);
		if (src != dst)
			copy(
				src, src_n, src_start_bit, src_start_bit + min_n,
				dst, dst_n, dst_start_bit, dst_start_bit + min_n
			);
		for (std::size_t i = 0; i < min_n; i += CHAR_SIZE) {
			*getPage(dst, dst_n, dst_start_bit, dst_start_bit + min_n, i) = ~(*getPage(dst, dst_n, dst_start_bit, dst_start_bit + min_n, i));
		}
	}
}

void BitUtils::bitwise_not(const void* const src,
	void* const dst,
	const std::size_t n
) {
	bitwise_not(
		src, n, 0, n,
		dst, n, 0, n
	);
}

void BitUtils::bitwise_not(void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	bitwise_not(
		src, n, start_bit, end_bit,
		src, n, start_bit, end_bit
	);
}

void BitUtils::bitwise_not(void* const src,
	const std::size_t n
) {
	bitwise_not(src, n, 0, n);
}

void BitUtils::bitwise_not_s(
	const void* const src,
	const std::size_t src_n,
	const std::size_t src_start_bit,
	const std::size_t src_end_bit,
	void* const dst,
	const std::size_t dst_n,
	const std::size_t dst_start_bit,
	const std::size_t dst_end_bit
) {
	if (!use_safe_function(src_n, src_start_bit, src_end_bit, __func__) &&
		!use_safe_function(dst_n, dst_start_bit, dst_end_bit, __func__)
	) {
		bitwise_not(src, src_n, src_start_bit, src_end_bit, dst, dst_n, dst_start_bit, dst_end_bit);
	}
	else {
		if (src == dst) {
			for (std::size_t i = 0; i < (dst_end_bit - dst_start_bit); i++) {
				flip(dst, dst_n, dst_start_bit, dst_end_bit, i);
			}
		}
		else {
			fill(dst, dst_n, dst_start_bit, dst_end_bit, 1);
			bitwise_xor_s(
				src, src_n, src_start_bit, src_end_bit,
				dst, dst_n, dst_start_bit, dst_end_bit,
				dst, dst_n, dst_start_bit, dst_end_bit
			);
		}
	}
}

void BitUtils::bitwise_not_s(
	const void* const src,
	void* const dst,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	bitwise_not_s(src, n, start_bit, end_bit, dst, n, start_bit, end_bit);
}

void BitUtils::bitwise_not_s(
	void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	bitwise_not_s(src, n, start_bit, end_bit, src, n, start_bit, end_bit);
}

bool BitUtils::bool_op(const void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	if (use_safe_function(n, start_bit, end_bit, __func__))
		return bool_op_s(src, n, start_bit, end_bit);
	else {
		for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
			if (*getPage(src, n, 0, n, i))
				return true;
		}
		return false;
	}
}

bool BitUtils::bool_op(const void* const src, const std::size_t n) {
	return bool_op(src, n, 0, n);
}

bool BitUtils::bool_op_s(const void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	if (!use_safe_function(n, start_bit, end_bit, __func__))
		return bool_op(src, n, start_bit, end_bit);
	else {
		if (n % CHAR_SIZE) {
			if (bool_op(src, (n - CHAR_SIZE ? n - CHAR_SIZE : CHAR_SIZE))) // We're evaluating all the previous bytes as bools (as it is faster)
				return true;
		}

		// Now evaluating each individual bit. Slow, but necessary.
		for (std::size_t i = 0; i < end_bit - start_bit; i++) {
			if (get(src, n, start_bit, end_bit, i))
				return true;
		}
		return false;
	}
}

bool BitUtils::bool_op_s(const void* const src, const std::size_t n) {
	return bool_op_s(src, n, 0, n);
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
	if (use_safe_function(left_n, left_start_bit, left_end_bit, __func__) ||
		use_safe_function(right_n, right_start_bit, right_end_bit, __func__)
	) {
		return equals_s(left, left_n, left_start_bit, left_end_bit,
			right, right_n, right_start_bit, right_end_bit);
	}
	return 0 == compare(
		left, left_n, left_start_bit, left_end_bit,
		right, right_n, right_start_bit, right_end_bit
	);
}

bool BitUtils::equals(const void* left,
	const void* const right,
	const std::size_t n
) {
	return equals(
		left, n, 0, n,
		right, n, 0, n
	);
}

bool BitUtils::equals_s(
	const void* const left,
	const std::size_t left_n,
	const std::size_t left_start_bit,
	const std::size_t left_end_bit,
	const void* const right,
	const std::size_t right_n,
	const std::size_t right_start_bit,
	const std::size_t right_end_bit
) {
	if (!use_safe_function(left_n, left_start_bit, left_end_bit, __func__) &&
		!use_safe_function(right_n, right_start_bit, right_end_bit, __func__)
		) {
		return equals(
			left, left_n, left_start_bit, left_end_bit,
			right, right_n, right_start_bit, right_end_bit
		);
	}
	return 0 == compare_s(
		left, left_n, left_start_bit, left_end_bit,
		right, right_n, right_start_bit, right_end_bit
	);
}

bool BitUtils::equals_s(
	const void* const left,
	const void* const right,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	return equals_s(
		left, n, start_bit, end_bit,
		right, n, start_bit, end_bit
	);
}

int BitUtils::compare(const void* const left,
	const std::size_t left_n,
	const std::size_t left_start_bit,
	const std::size_t left_end_bit,
	const void* const right,
	const std::size_t right_n,
	const std::size_t right_start_bit,
	const std::size_t right_end_bit
) {
	if (use_safe_function(left_n, left_start_bit, left_end_bit, __func__) ||
		use_safe_function(right_n, right_start_bit, right_end_bit, __func__)
	) {
		return compare_s(
			left, left_n, left_start_bit, left_end_bit,
			right, right_n, right_start_bit, right_end_bit
		);
	}
	else if (
		is_soft_bounded(left_n, left_start_bit, left_end_bit) ||
		is_soft_bounded(right_n, right_start_bit, right_end_bit)
	)
		return memcmp(left, right, size(left_n < right_n ? left_n : right_n));
	else
		return memcmp(left, right, size(left_n));
}

int BitUtils::compare(const void* const left,
	const void* const right,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	return compare(
		left, n, start_bit, end_bit,
		right, n, start_bit, end_bit);
}

int BitUtils::compare(
	const void* const left,
	const void* const right,
	const std::size_t n
) {
	return compare(left, right, n, 0, n);
}

int BitUtils::compare_s(
	const void* const left,
	const std::size_t left_n,
	const std::size_t left_start_bit,
	const std::size_t left_end_bit,
	const void* const right,
	const std::size_t right_n,
	const std::size_t right_start_bit,
	const std::size_t right_end_bit
) {
	if (!use_safe_function(left_n, left_start_bit, left_end_bit, __func__) &&
		!use_safe_function(right_n, right_start_bit, right_end_bit, __func__)) {
		return compare(
			left, left_n, left_start_bit, left_end_bit,
			right, right_n, right_start_bit, right_end_bit
		);
	}
	std::size_t min_n = ((left_end_bit - left_start_bit) < (right_end_bit - right_start_bit))
		? (left_end_bit - left_start_bit)
		: (right_end_bit - right_start_bit);
	bool l, r;
	for (std::size_t i = 0; i < min_n; i++) {
		l = get(left, left_n, left_start_bit, left_end_bit, i);
		r = get(right, right_n, right_start_bit, right_end_bit, i);
		if (l ^ r) { // if l and r are the same then don't prematurely return
			return 0 + (l && !r ? 1 : -1);
		}
	}
	return 0;
}

int BitUtils::compare_s(
	const void* const left,
	const void* const right,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	return compare_s(
		left, n, start_bit, end_bit,
		right, n, start_bit, end_bit
	);
}

int BitUtils::compare_s(
	const void* const left,
	const void* const right,
	const std::size_t n
) {
	return compare_s(left, right, n, 0, n);
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
		buf[i] = get(src, src_n, src_start_bit, src_end_bit, i) ? '1' : '0';
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
		buf[i] = get(src, src_n, src_start_bit, src_end_bit, i) ? L'1' : L'0';
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
	for (std::size_t i = 0; i < end_bit - start_bit; i++) {
		os << get(src, n, start_bit, end_bit, i) ? '1' : '0';
	}
}

std::string BitUtils::str(
	const void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	std::stringstream ss;
	str(src, n, start_bit, end_bit, ss);
	return ss.str();
}

std::string BitUtils::str(const void* const src, const std::size_t n) {
	return str(src, n, 0, n);
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
		wos << get(src, n, start_bit, end_bit, i) ? L'1' : L'0';
	}
}

std::wstring BitUtils::wstr(const void* const src,
	const std::size_t n,
	const std::size_t start_bit,
	const std::size_t end_bit
) {
	std::wstringstream wss;
	wstr(src, n, start_bit, end_bit, wss);
	return wss.str();
}

std::wstring BitUtils::wstr(const void* const src, const std::size_t n) {
	return wstr(src, n, 0, n);
}

void BitUtils::for_each_byte(void* const begin, void* const end, void (*func)(unsigned char* pC)) {
	for_each_page<unsigned char>(begin, end, func);
}

void BitUtils::for_each_byte(void* const begin, void* const end, std::function<void(unsigned char*)>& func) {
	for_each_page<unsigned char>(begin, end, func);
}

void BitUtils::for_each_bit(const void* const begin, const void* const end, void (*func)(bool b)) {
	bool reverse = begin > end;

	for (char* it = (char*)(reverse ? end : begin); it != (reverse ? begin : end); it += reverse ? -1 : 1) {
		for (char i = 0; i < CHAR_SIZE; i++) {
			(*func)(get(it, 8, i));
		}
	}
	if (reverse) {
		for (char i = 0; i < CHAR_SIZE; i++) {
			(*func)(get(begin, 8, i));
		}
	}
}

void BitUtils::for_each_bit(const void* const begin, const void* const end, std::function<void(bool)>& func) {
	bool reverse = begin > end;

	for (char* it = (char*)(reverse ? end : begin); it != (reverse ? begin : end); it += reverse ? -1 : 1) {
		for (char i = 0; i < CHAR_SIZE; i++) {
			func(get(it, 8, i));
		}
	}
	if (reverse) {
		for (char i = 0; i < CHAR_SIZE; i++) {
			func(get(begin, 8, i));
		}
	}
}

#undef _BITUTILS_IS_LITTLE_ENDIAN
#endif // C++11