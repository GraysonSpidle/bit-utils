#ifndef __BITUTILS17_H__
#define __BITUTILS17_H__


#if __cplusplus >= 201700 // C++17

namespace BitUtils {

	template <
		const std::size_t _n,
		const std::size_t _start_bit = 0,
		const std::size_t _end_bit = _n,
		std::enable_if_t <
		(_n > 0) &&
		(_end_bit > 0) &&
		(_start_bit < _n) &&
		(_end_bit <= _n) &&
		(_start_bit < _end_bit),
		bool
		> = true
		>
		class BitUtils {
#define _BITUTILS_MIN(left, right) ((left) < (right) ? (left) : (right))

		template <
			class BitUtils_left = BitUtils,
			class BitUtils_right = BitUtils,
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_left>, bool > = true,
			std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_right>, bool > = true
		>
			constexpr static const bool do_bounds_overlap(const void* const left, const void* const right) {
			return
				reinterpret_cast<const unsigned char*>(left) + (BitUtils_left::end_bit - BitUtils_left::start_bit) >= right ||
				reinterpret_cast<const unsigned char*>(right) + (BitUtils_right::end_bit - BitUtils_right::start_bit) >= left;
		}

		// Thank you stack overflow! (adapted code from there)

		constexpr static std::size_t floorlog2(std::size_t  x) {
#if _BITUTILS_IS_LITTLE_ENDIAN
			return x == 1 ? 0 : 1 + floorlog2(x >> 1);
#else
			return x == 1 ? 0 : 1 + floorlog2(x << 1);
#endif
		}

		constexpr static std::size_t ceillog2(std::size_t x) {
			return x == 1 ? 0 : floorlog2(x - 1) + 1;
		}

		public:
			constexpr static const std::size_t n = _end_bit - _start_bit; // The number of bits we're working with.
			constexpr static const std::size_t size = floorlog2(_n); // The number of bytes that would be allocated.
			constexpr static const std::size_t start_bit = _start_bit; // The index of the bit to start on (inclusive).
			constexpr static const std::size_t end_bit = _end_bit; // The index of the bit to end on (exclusive).
			constexpr static const bool is_bounded = start_bit != 0 || end_bit != _n;
			constexpr static const bool is_soft_bounded = !is_bounded && size * CHAR_SIZE != _n;

			// ========== TYPE DEFS ==========

			/* A typedef that sets bounds for the BitUtils relative to the current bounds (if any are there).
			This guarantees:
			* the safe versions of functions (where available) are forcibly used.
			*/
			template <
				const std::size_t start,
				const std::size_t end,
				std::enable_if_t <
				start >= start_bit &&
				end <= end_bit &&
				start < end,
				bool
				> = true
				>
				using bound = BitUtils<
				_n,
				start,
				end
				>;

			/* A typedef that removes all bounds for the BitUtils.
			It guarantees:
			* _n will be a log of 2.
			* the safe versions of functions (where available) are not forcibly used.
			*/
			typedef BitUtils<size* CHAR_SIZE> unbound;

			/* A typedef that, given a type, makes an unbounded BitUtils class that accomodates for its size. */
			template <
				class Type
			>
				using of = BitUtils<sizeof(Type)* CHAR_SIZE>;

			// ========== CORE FUNCTIONS ==========

			static unsigned char* const getPage(void* const block, const std::size_t i) {
				if constexpr (n == CHAR_SIZE)
					return reinterpret_cast<unsigned char*>(block);
				else if constexpr (is_bounded)
					return reinterpret_cast<unsigned char*>(block) + ((i + start_bit) / CHAR_SIZE);
				else
					return reinterpret_cast<unsigned char*>(block) + (i / CHAR_SIZE);
			}
			static const unsigned char* const getPage(const void* const block, const std::size_t i) {
				if constexpr (n == CHAR_SIZE)
					return reinterpret_cast<const unsigned char*>(block);
				else if constexpr (is_bounded)
					return reinterpret_cast<const unsigned char*>(block) + ((i + start_bit) / CHAR_SIZE);
				else
					return reinterpret_cast<const unsigned char*>(block) + (i / CHAR_SIZE);
			}

			/// <summary>
			/// Gets the designated bit.
			/// </summary>
			/// <param name="block">the pointer to the memory block.</param>
			/// <param name="i">the index of the bit to get.</param>
			/// <returns>true if the bit is 1 and false if the bit 0.</returns>
			static bool get(const void* const block, std::size_t i) {
				if constexpr (_BITUTILS_IS_LITTLE_ENDIAN)
					return *BitUtils::getPage(block, i) & ((std::size_t)1 << ((i + start_bit) % CHAR_SIZE));
				else
					return *BitUtils::getPage(block, i) & ((std::size_t)1 >> ((i + start_bit) % CHAR_SIZE));
			}

			/// <summary>
			/// Flips the designated bit (ie from 0 to 1 or from 1 to 0). It is worth noting this is faster than set().
			/// </summary>
			/// <param name="src">the pointer to the memory block.</param>
			/// <param name="i">the index of the bit to flip.</param>
			static void flip(void* const block, std::size_t i) {
				if constexpr (_BITUTILS_IS_LITTLE_ENDIAN)
					*BitUtils::getPage(block, i) ^= ((std::size_t)1 << ((i + start_bit) % CHAR_SIZE));
				else
					*BitUtils::getPage(block, i) ^= ((std::size_t)1 >> ((i + start_bit) % CHAR_SIZE));
			}

			/// <summary>
			/// Sets the designated bit to reflect the supplied boolean.
			/// </summary>
			/// <param name="src">the pointer to the memory block.</param>
			/// <param name="i">the index of the bit to set.</param>
			/// <param name="b">the boolean that reflects what to set the bit to (ie 1 for true and 0 for false).</param>
			static void set(void* const src, std::size_t i, bool b) {
				if constexpr (_BITUTILS_IS_LITTLE_ENDIAN)
					*BitUtils::getPage(src, i) |= ((std::size_t)1 << ((i + start_bit) % CHAR_SIZE));
				else
					*BitUtils::getPage(src, i) |= ((std::size_t)1 >> ((i + start_bit) % CHAR_SIZE));

				// Flipping the bit if user wants it to be false
				if (!b)
					flip(src, i);
			}

			// ========== FUNCTIONS ==========

			/// <summary>
			/// Dynamically allocates enough bytes to accomodate as designated by n.
			/// This is literally just utilizes calloc()
			/// </summary>
			/// <returns>Returns a void* to the memory block.</returns>
			static void* create() {
				return calloc(size, 1);
			}

			/// <summary>
			/// Fills the memory block to reflect the supplied boolean.
			/// If the BitUtils has bounds, then this function will always default to fill_s().
			/// </summary>
			/// <param name="block">the pointer to the memory block.</param>
			/// <param name="b">the boolean that reflects what to fll the memory block with (ie 1 for true and 0 false).</param>
			static void fill(void* const block, bool b) {
				if constexpr (is_bounded) {
					for (std::size_t i = 0; i < n; i++) {
						set(block, i, b);
					}
				}
				else if constexpr (size == 1)
					*(reinterpret_cast<unsigned char*>(block)) = b ? -1 : 0;
				else
					memset(block, b ? (unsigned char)-1 : 0, size);
			}

			/// <summary>
			/// Copies the memory block into another.
			/// </summary>
			/// <typeparam name="BitUtils_src">the BitUtils class to use for src. Defaults to the current BitUtils class.</typeparam>
			/// <typeparam name="BitUtils_dst">the BitUtils class to use for dst. Defaults to the current BitUtils class.</typeparam>
			/// <param name="src">the pointer to the source memory block. If this equals dst, this function does nothing.</param>
			/// <param name="dst">the pointer to the destination memory block. If the equals src, this function does nothing.</param>
			template <
				class BitUtils_src = BitUtils,
				class BitUtils_dst = BitUtils,
				std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_src>, bool > = true,
				std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_dst>, bool > = true
			>
				static void copy(const void* const src, void* const dst) {
				if constexpr (
					BitUtils_src::start_bit == BitUtils_dst::start_bit &&
					BitUtils_src::end_bit == BitUtils_dst::end_bit
					) {
					if (src == dst)
						return;
				}

				if constexpr (BitUtils_src::is_bounded || BitUtils_dst::is_bounded) {
					constexpr const std::size_t min_n = (BitUtils_src::n < BitUtils_dst::n)
						? BitUtils_src::n
						: BitUtils_dst::n;
					if (do_bounds_overlap<BitUtils_src, BitUtils_dst>(src, dst)) {
						if constexpr (BitUtils_src::start_bit < BitUtils_dst::start_bit) {
							for (std::size_t i = min_n; i > 0; i--) {
								BitUtils_dst::set(dst, i - 1, BitUtils_src::get(src, i - 1));
							}
						}
						else {
							for (std::size_t i = 0; i < min_n; i++) {
								BitUtils_dst::set(dst, i, BitUtils_src::get(src, i));
							}
						}
					}
					else {
						BitUtils<
							min_n, BitUtils_dst::start_bit, BitUtils_dst::start_bit + min_n
						>::fill(dst, 0);
						bitwise_or<
							BitUtils<BitUtils_src::n, BitUtils_src::start_bit, BitUtils_src::start_bit + min_n>,
							BitUtils<BitUtils_dst::n, BitUtils_dst::start_bit, BitUtils_dst::start_bit + min_n>,
							BitUtils<BitUtils_dst::n, BitUtils_dst::start_bit, BitUtils_dst::start_bit + min_n>
						>(src, dst, dst);
					}
				}
				else { // unbounded
					if (src == dst)
						return;
#if defined(__GNUG__) || defined(_CRT_SECURE_NO_WARNINGS)
					memcpy(dst, src, BitUtils_src::size);
#else
					memcpy_s(dst, BitUtils_dst::size, src, BitUtils_src::size);
#endif
				}
			}

			/// <summary>
			///	Performs the & operation on every bit between the left and the right memory blocks and puts the result in the destination memory block.
			/// </summary>
			/// <typeparam name="BitUtils_left">the BitUtils class to use for the left variable. Defaults to the current BitUtils class.</typeparam>
			/// <typeparam name="BitUtils_right">the BitUtils class to use for the right variable. Defaults to the current BitUtils class.</typeparam>
			/// <typeparam name="BitUtils_dst">the BitUtils class to use for the dst variable. Defaults to the current BitUtils class.</typeparam>
			/// <param name="left">the pointer to the memory block on the left side of the &. This can be the same as the value for the right or dst parameter.</param>
			/// <param name="right">the pointer to the memory block on the right side of the &. This can be the same as the value for the left or dst parameter.</param>
			/// <param name="dst">the pointer to the memory block that gets the result of the operation. This can be the same as the value for the left or right parameter.</param>
			template <
				class BitUtils_left = BitUtils,
				class BitUtils_right = BitUtils,
				class BitUtils_dst = BitUtils,
				std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_left>, bool > = true,
				std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_right>, bool > = true,
				std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_dst>, bool > = true
			>
				static void bitwise_and(const void* const left, const void* const right, void* const dst) {
				if constexpr (BitUtils_left::is_bounded || BitUtils_right::is_bounded || BitUtils_dst::is_bounded) {
					if constexpr (
						BitUtils_left::n == BitUtils_right::n &&
						BitUtils_left::start_bit == BitUtils_right::start_bit &&
						BitUtils_left::end_bit == BitUtils_right::end_bit
						) {
						if (left == right) {
							if (left == dst)
								return;
							copy<BitUtils_left, BitUtils_dst>(left, dst);
							return;
						}
					}
					constexpr const std::size_t min_n = _BITUTILS_MIN(_BITUTILS_MIN(BitUtils_left::n, BitUtils_right::n), BitUtils_dst::n);

					std::size_t i = 0;
					int step = 1;
					if constexpr (
						do_bounds_overlap<BitUtils_left, BitUtils_right>(left, right) &&
						do_bounds_overlap<BitUtils_left, BitUtils_dst>(left, dst)
						) {
						if constexpr (
							BitUtils_left::start_bit < BitUtils_dst::start_bit ||
							BitUtils_right::start_bit < BitUtils_dst::start_bit
							) {
							i = min_n;
							step = -1;
						}
					}
					bool l, r;
					for (; (step < 0 ? i > 0 : i < min_n); i += step) {
						l = BitUtils<
							BitUtils_left::n,
							BitUtils_left::start_bit,
							BitUtils_left::start_bit + min_n
						>::get(left, (step < 0 ? i - 1 : i));
						r = BitUtils<
							BitUtils_right::n,
							BitUtils_right::start_bit,
							BitUtils_right::start_bit + min_n
						>::get(right, (step < 0 ? i - 1 : i));
						BitUtils<
							BitUtils_dst::n,
							BitUtils_dst::start_bit,
							BitUtils_dst::start_bit + min_n
						>::set(dst, (step < 0 ? i - 1 : i), l & r);
					}
				}
				else {
					if (left == right) {
						if (left == dst)
							return;
						copy<BitUtils_left, BitUtils_dst>(left, dst);
						return;
					}

					for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
						*BitUtils_dst::getPage(dst, i) =
							*BitUtils_left::getPage(left, i) &
							*BitUtils_right::getPage(right, i);
					}
				}
			}

			/// <summary>
			///	Performs the | operation on every bit between the left and the right memory blocks and puts the result in the destination memory block.
			/// </summary>
			/// <typeparam name="BitUtils_left">the BitUtils class to use for the left variable. Defaults to the current BitUtils class.</typeparam>
			/// <typeparam name="BitUtils_right">the BitUtils class to use for the right variable. Defaults to the current BitUtils class.</typeparam>
			/// <typeparam name="BitUtils_dst">the BitUtils class to use for the dst variable. Defaults to the current BitUtils class.</typeparam>
			/// <param name="left">the pointer to the memory block on the left side of the |. This can be the same as the value for the right or dst parameter.</param>
			/// <param name="right">the pointer to the memory block on the right side of the |. This can be the same as the value for the left or dst parameter.</param>
			/// <param name="dst">the pointer to the memory block that gets the result of the operation. This can be the same as the value for the left or right parameter.</param>
			template <
				class BitUtils_left = BitUtils,
				class BitUtils_right = BitUtils,
				class BitUtils_dst = BitUtils,
				std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_left>, bool > = true,
				std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_right>, bool > = true,
				std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_dst>, bool > = true
			>
				static void bitwise_or(const void* const left, const void* const right, void* const dst) {
				if (left == right) {
					if (left == dst)
						return;
					copy<BitUtils_left, BitUtils_dst>(left, dst);
					return;
				}

				if constexpr (BitUtils_left::is_bounded || BitUtils_right::is_bounded || BitUtils_dst::is_bounded) {
					constexpr const std::size_t min_n = _BITUTILS_MIN(_BITUTILS_MIN(BitUtils_left::n, BitUtils_right::n), BitUtils_dst::n);
					bool l, r;
					for (std::size_t i = 0; i < min_n; i++) {
						l = BitUtils<
							BitUtils_left::n,
							BitUtils_left::start_bit,
							BitUtils_left::start_bit + min_n
						>::get(left, i);
						r = BitUtils<
							BitUtils_right::n,
							BitUtils_right::start_bit,
							BitUtils_right::start_bit + min_n
						>::get(right, i);
						BitUtils<
							BitUtils_dst::n,
							BitUtils_dst::start_bit,
							BitUtils_dst::start_bit + min_n
						>::set(dst, i, l | r);
					}
				}
				else {
					for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
						*BitUtils_dst::getPage(dst, i) =
							*BitUtils_left::getPage(left, i) |
							*BitUtils_right::getPage(right, i);
					}
				}
			}

			/// <summary>
			///	Performs the ^ operation on every bit between the left and the right memory blocks and puts the result in the destination memory block.
			/// </summary>
			/// <typeparam name="BitUtils_left">the BitUtils class to use for the left variable. Defaults to the current BitUtils class.</typeparam>
			/// <typeparam name="BitUtils_right">the BitUtils class to use for the right variable. Defaults to the current BitUtils class.</typeparam>
			/// <typeparam name="BitUtils_dst">the BitUtils class to use for the dst variable. Defaults to the current BitUtils class.</typeparam>
			/// <param name="left">the pointer to the memory block on the left side of the ^. This can be the same as the value for the right or dst parameter.</param>
			/// <param name="right">the pointer to the memory block on the right side of the ^. This can be the same as the value for the left or dst parameter.</param>
			/// <param name="dst">the pointer to the memory block that gets the result of the operation. This can be the same as the value for the left or right parameter.</param>
			template <
				class BitUtils_left = BitUtils,
				class BitUtils_right = BitUtils,
				class BitUtils_dst = BitUtils,
				std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_left>, bool > = true,
				std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_right>, bool > = true,
				std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_dst>, bool > = true
			>
				static void bitwise_xor(const void* const left, const void* const right, void* const dst) {
				if constexpr (BitUtils_left::is_bounded || BitUtils_right::is_bounded || BitUtils_dst::is_bounded) {
					constexpr const std::size_t min_n = _BITUTILS_MIN(_BITUTILS_MIN(BitUtils_left::n, BitUtils_right::n), BitUtils_dst::n);

					std::size_t i = 0;
					int step = 1;
					if constexpr (
						(
							do_bounds_overlap<BitUtils_left, BitUtils_right>(left, right) &&
							do_bounds_overlap<BitUtils_left, BitUtils_dst>(left, dst)
							) &&
						(
							BitUtils_left::start_bit < BitUtils_dst::start_bit ||
							BitUtils_right::start_bit < BitUtils_dst::start_bit
							)
						) {
						i = min_n;
						step = -1;
					}

					bool l, r;
					for (; (step < 0 ? i > 0 : i < min_n); i += step) {
						l = BitUtils<
							BitUtils_left::n,
							BitUtils_left::start_bit,
							BitUtils_left::start_bit + min_n
						>::get(left, (step < 0 ? i - 1 : i));
						r = BitUtils<
							BitUtils_right::n,
							BitUtils_right::start_bit,
							BitUtils_right::start_bit + min_n
						>::get(right, (step < 0 ? i - 1 : i));
						BitUtils<
							BitUtils_dst::n,
							BitUtils_dst::start_bit,
							BitUtils_dst::start_bit + min_n
						>::set(dst, (step < 0 ? i - 1 : i), l ^ r);
					}
				}
				else {
					if (left == right) {
						BitUtils_dst::fill(dst, 0);
						return;
					}
					for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
						*BitUtils_dst::getPage(dst, i) =
							*BitUtils_left::getPage(left, i) ^ *BitUtils_right::getPage(right, i);
					}
				}
			}

			/// <summary>
			/// Performs the ~ operation on every bit in the src memory block and puts the result in the destinatiion memory block.
			/// </summary>
			/// <typeparam name="BitUtils_src">the BitUtils class to use for the src variable. Defaults to the current BitUtils class.</typeparam>
			/// <typeparam name="BitUtils_dst">the BitUtils class to use for the dst variable. Defaults to the current BitUtils class.</typeparam>
			/// <param name="src">the pointer to the source memory block. This can be the same as the value for the dst parameter.</param>
			/// <param name="dst">the pointer to the destination memory block. This can be the same as the value for the src parameter.</param>
			template <
				class BitUtils_src = BitUtils,
				class BitUtils_dst = BitUtils,
				std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_src>, bool > = true,
				std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_dst>, bool > = true
			>
				static void bitwise_not(const void* const src, void* const dst) {
				if constexpr (BitUtils_src::is_bounded || BitUtils_dst::is_bounded) {
					if constexpr (do_bounds_overlap<BitUtils_src, BitUtils_dst>(src, dst)) {

						constexpr std::size_t min_n = _BITUTILS_MIN(BitUtils_src::n, BitUtils_dst::n);

						std::size_t i = (BitUtils_src::start_bit < BitUtils_dst::start_bit) ? min_n : 0;
						constexpr int step = (BitUtils_src::start_bit < BitUtils_dst::start_bit) ? -1 : 1;

						for (; (step < 0 ? i > 0 : i < min_n); i += step) {
							BitUtils_dst::flip(dst, (step < 0 ? i - 1 : i));
						}
					}
					else {
						BitUtils_dst::fill(dst, 1);

						bitwise_xor<
							BitUtils_src,
							BitUtils_dst,
							BitUtils_dst
						>(src, dst, dst);
					}
				}
				else { // unbounded
					for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
						*BitUtils_dst::getPage(dst, i) = ~(*BitUtils_dst::getPage(src, i));
					}
				}
			}

			/// <summary>
			/// Performs the ~ operation on every bit in the src memory block and puts the result in the same memory block.
			/// This is the same as bitwise_not(src, src)
			/// </summary>
			/// <param name="src">the pointer to the source and destination memory block.</param>
			static void bitwise_not(void* const src) {
				bitwise_not(src, src);
			}

			static void shift_left(void* const block, const std::size_t amount) {
				if (amount == 0)
					return;
				if (amount >= n) {
					fill(block, 0);
					return;
				}

				copy<
					BitUtils<n, start_bit + amount, end_bit>,
					BitUtils<n, start_bit, end_bit - amount>
				>(block, block);
				BitUtils<n, end_bit - amount, end_bit>::fill(block, 0);
			}

			static void shift_right(void* const block, const std::size_t amount) {
				if (amount == 0)
					return;
				if (amount >= n) {
					fill(block, 0);
					return;
				}

				copy<
					BitUtils<n, start_bit, end_bit - amount>,
					BitUtils<n, start_bit + amount, end_bit>
				>(block, block);
				BitUtils<n, start_bit, start_bit + amount>::fill(block, 0);
			}

			/// <summary>
			/// Performs the bool() operation on the entire memory block (unless BitUtils is bounded, then it defaults to the safe version).
			/// </summary>
			/// <param name="src">the pointer to the source memory block.</param>
			/// <returns>true if any of the bits are 1 and false if all the bits are 0.</returns>
			static bool bool_op(const void* const block) {
				if constexpr (is_bounded) {
					for (std::size_t i = 0; i < n; i++) {
						if (get(block, i))
							return true;
					}
					return false;
				}
				else { // unbounded
					for (std::size_t i = 0; i < n; i += CHAR_SIZE) {
						if (*BitUtils::getPage(block, i))
							return true;
					}
					return false;
				}
			}

			template <
				class BitUtils_left = BitUtils,
				class BitUtils_right = BitUtils,
				std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_left>, bool > = true,
				std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_right>, bool > = true
			>
				static bool equals(const void* const left, const void* const right) {
				if constexpr (BitUtils_left::is_bounded || BitUtils_right::is_bounded)
					return 0 == compare<BitUtils_left, BitUtils_right>(left, right);
				else { // unbounded
					for (std::size_t i = 0; i < size; i++) {
						if (bool(*BitUtils_left::getPage(left, i * CHAR_SIZE)) != bool(*BitUtils_right::getPage(right, i * CHAR_SIZE)))
							return false;
					}
					return true;
				}
			}

			template <
				class BitUtils_left = BitUtils,
				class BitUtils_right = BitUtils,
				std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_left>, bool > = true,
				std::enable_if_t < std::is_convertible_v<BitUtils, BitUtils_right>, bool > = true
			>
				static int compare(const void* const left, const void* const right) {
				if constexpr (BitUtils_left::is_bounded || BitUtils_right::is_bounded) {
					constexpr const std::size_t min_n = _BITUTILS_MIN(BitUtils_left::n, BitUtils_right::n);
					bool l, r;
					for (std::size_t i = 0; i < min_n; i++) {
						l = BitUtils_left::get(left, i);
						r = BitUtils_right::get(right, i);
						if (l ^ r)
							return 0 + (l && !r ? 1 : -1);
					}
					return 0;
				}
				else { // unbounded
					return memcmp(left, right, _BITUTILS_MIN(BitUtils_left::size, BitUtils_right::size));
				}
			}

			static bool all(const void* const block) {
				if constexpr (is_bounded) {
					for (std::size_t i = 0; i < n; i++) {
						if (!get(block, i))
							return false;
					}
					return true;
				}
				else { // unbounded
					for (std::size_t i = 0; i < size; i++) {
						if (*BitUtils::getPage(block, i * CHAR_SIZE) != (unsigned char)-1)
							return false;
					}
					return true;
				}
			}

			static void str(const void* const arr_ptr, char* const buf) {
				if (!strlen(buf))
					return;
				for (std::size_t i = 0; i < (n < strlen(buf) ? n : strlen(buf)) - 1; i++) {
					buf[i] = get(arr_ptr, i) ? '1' : '0';
				}
				buf[(n < strlen(buf) ? n : strlen(buf))] = '\0';
			}

			static void wstr(const void* const arr, wchar_t* const buf, const std::size_t buf_n) {
				if (!buf_n)
					return;
				for (std::size_t i = 0; i < (n < buf_n ? n : buf_n) - 1; i++) {
					buf[i] = get(arr, i) ? L'1' : L'0';
				}
				buf[(n < buf_n ? n : buf_n)] = L'\0';
			}

			static void str(const void* const arr_ptr, std::ostream& os) {
				std::size_t count;
				if constexpr (n != _n)
					count = n;
				else
					count = size * CHAR_SIZE;

				for (std::size_t i = 0; i < count; i++) {
					os << get(arr_ptr, i) ? '1' : '0';
				}
			}

			static void wstr(const void* const arr_ptr, std::wostream& wos) {
				for (std::size_t i = 0; i < n; i++) {
					wos << get(arr_ptr, i) ? L'1' : L'0';
				}
			}

			static std::string str(const void* const arr_ptr) {
				std::stringstream ss;
				str(arr_ptr, ss);
				return ss.str();
			}

			static std::wstring wstr(const void* const arr_ptr) {
				std::wstringstream ss;
				wstr(arr_ptr, ss);
				return ss.str();
			}

			static void from_str(void* const block, const std::string& s) {
				std::size_t min_n = n < s.length() ? n : s.length();

				char c;
				for (std::size_t i = 0; i < min_n; i++) {
					c = s.at(i);
					switch (c) {
					case '0':
						set(block, i, 0);
						break;
					case '1':
						set(block, i, 1);
						break;
					default:
						throw std::invalid_argument("unrecognized char: " + c);
					}
				}
			}

			static void from_wstr(void* const block, const std::wstring& s) {
				std::size_t min_n = n < s.length() ? n : s.length();

				wchar_t c;
				for (std::size_t i = 0; i < min_n; i++) {
					c = s.at(i);
					switch (c) {
					case L'0':
						set(block, i, 0);
						break;
					case L'1':
						set(block, i, 1);
						break;
					default:
						throw std::invalid_argument(L"unrecognized char: " + c);
					}
				}
			}

			template <
				typename FF,
				std::enable_if_t < std::is_invocable_v <FF, unsigned char* const>, bool > = true
			>
				static void for_each_byte(void* const block, const FF& func) {
				for (std::size_t i = 0; i < size; i++) {
					std::invoke(func, BitUtils::getPage(block, i * CHAR_SIZE));
				}
			}

			template <
				typename FF,
				std::enable_if_t < std::is_invocable_v <FF, unsigned char* const>, bool > = true
			>
				static void rfor_each_byte(void* const block, const FF& func) {
				for (std::size_t i = size; i > 0; i--) {
					std::invoke(func, BitUtils::getPage(block, (i - 1) * CHAR_SIZE));
				}
			}

			template <
				typename FF,
				std::enable_if_t < std::is_invocable_v <FF, const unsigned char* const>, bool > = true
			>
				static void for_each_byte(const void* const block, const FF& func) {
				for (std::size_t i = 0; i < size; i++) {
					std::invoke(func, BitUtils::getPage(block, i * CHAR_SIZE));
				}
			}

			template <
				typename FF,
				std::enable_if_t < std::is_invocable_v <FF, const unsigned char* const>, bool > = true
			>
				static void rfor_each_byte(const void* const block, const FF& func) {
				for (std::size_t i = size; i > 0; i--) {
					std::invoke(func, BitUtils::getPage(block, (i - 1) * CHAR_SIZE));
				}
			}

			template <
				typename FF,
				std::enable_if_t < std::is_invocable_v <FF, bool>, bool > = true
			>
				static void for_each_bit(const void* const block, const FF& func) {
				for (std::size_t i = 0; i < n; i++) {
					std::invoke(func, get(block, i));
				}
			}

			template <
				typename FF,
				std::enable_if_t < std::is_invocable_v <FF, bool>, bool > = true
			>
				static void rfor_each_bit(const void* const block, const FF& func) {
				for (std::size_t i = n; i > 0; i--) {
					std::invoke(func, get(block, i - 1));
				}
			}

			// =============================================

			/// <summary>
			/// Cast operator overload to aid in our SFINAE exploits. This allows us to use the std::is_convertible type trait to
			/// figure out at compile time if another BitUtils class is compatible with this one. This also allows us to effectively cast
			/// classes by casting objects made from the class and just using the static functions.
			/// </summary>
			/// <param name="other_n">this is the value you'd put in the first spot if you were specifying a BitUtils class. This must be greater than 0.</param>
			/// <param name="other_start_bit">this is the value you'd use to specify the begining bound.</param>
			/// <param name="other_end_bit">this is the value you'd use to specify the end bound.</param>
			template <
				const std::size_t other_n,
				const std::size_t other_start_bit = 0,
				const std::size_t other_end_bit = other_n,
				std::enable_if_t <
				(other_n > 0) &&
				(other_start_bit < other_n) &&
				(other_end_bit <= other_n) &&
				(other_start_bit < other_end_bit) &&
				(other_end_bit - other_start_bit) >= n,
				bool
				> = true
				>
				operator BitUtils<other_n, other_start_bit, other_end_bit>() const {
				return BitUtils<other_n, other_start_bit, other_end_bit>();
			}
	};
}
#undef _BITUTILS_MIN
#endif // C++17
#undef _BITUTILS_IS_LITTLE_ENDIAN
#undef _BITUTILS_MIN
#endif
