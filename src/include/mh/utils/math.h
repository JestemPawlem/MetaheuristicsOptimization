#pragma once

#include <bit>
#include <cmath>
#include <cstdint>
#include <numbers>

#include <mh/traits/float_traits.h>


namespace mh::utils::math
{
	template <traits::ieee754_fp T>
	constexpr T sqrt_2 = std::numbers::sqrt2_v<T>;

	template <traits::ieee754_fp T>
	constexpr T e = std::numbers::e_v<T>;

	template <traits::ieee754_fp T>
	constexpr T pi = std::numbers::pi_v<T>;

	template <traits::ieee754_fp T>
	constexpr T ln_2 = std::numbers::ln2_v<T>;

	template <traits::ieee754_fp T>
	constexpr T two_pi = T{ 2 } * std::numbers::pi_v<T>;

	template <traits::ieee754_fp T>
	constexpr T one = T{ 1 };

	template <traits::ieee754_fp T>
	constexpr T pow2(T a)
	{
		return a * a;
	}

	template <traits::ieee754_fp T>
	constexpr T cos_two_pi(T x) noexcept
	{
		x -= std::floor(x + T{ 0.5 });

		T theta = two_pi<T> *x;
		T t2 = theta * theta;
		return T{ 1 } + t2 * (T{ -0.5 } + t2 *
			(T{ 0.041666666666666664 } - T{ 0.001388888888888889 } * t2));
	}

	template <traits::ieee754_fp T>
	inline T log(T x) noexcept
	{
		using tr = traits::ieee754_traits<T>;
		using uint_t = typename tr::uint_t;
		using int_t = typename tr::int_t;

		uint_t bx = std::bit_cast<uint_t>(x);

		const int_t ex = static_cast<int_t>(bx >> tr::mantissa_bits) - tr::bias;

		constexpr uint_t one_exponent = static_cast<uint_t>(tr::bias) << tr::mantissa_bits;
		bx = (bx & tr::mantissa_mask) | one_exponent;

		T m = std::bit_cast<T>(bx);

		const T y = (m - T{ 1 }) / (m + T{ 1 });
		const T y2 = y * y;

		T log_m = y * (T{ 2.0 } + y2 * (T{ 0.6666666666666666 } + y2 * (T{ 0.4 } + y2 * T{ 0.2857142857142857 })));

		return static_cast<T>(ex) * ln_2<T> +log_m;
	}
}