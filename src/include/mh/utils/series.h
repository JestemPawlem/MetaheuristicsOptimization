#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

#include <mh/traits/float_traits.h>
#include <mh/utils/math.h>


namespace mh::utils::series
{
	namespace detail
	{
		template <traits::ieee754_fp T>
		constexpr T compile_time_pow(T base, int exp)
		{
			if (exp == 0)
				return T{ 1 };
			if (exp < 0)
				return T{ 1 } / compile_time_pow(base, -exp);

			T result = T{ 1 };
			T current = base;
			while (exp > 0)
			{
				if (exp % 2 == 1)
					result *= current;
				current *= current;
				exp /= 2;
			}

			return result;
		}

		template <traits::ieee754_fp T>
		constexpr T compile_time_exp(T x)
		{
			int n = static_cast<int>(x);
			if (x < T{ 0 } && x != static_cast<T>(n))
				n -= 1;

			T r = x - static_cast<T>(n);

			T sum = T{ 1 };
			T term = T{ 1 };

			for (int i = 1; i < 8; ++i)
			{
				term *= r / static_cast<T>(i);
				sum += term;
			}

			return sum * compile_time_pow(math::e<T>, n);
		}

		template <traits::ieee754_fp T>
		constexpr void extract_exponent_and_mantissa(T value, int& out_exp, T& out_mantissa)
		{
			using tr = traits::ieee754_traits<T>;
			using bits_t = typename tr::uint_t;

			bits_t bits = std::bit_cast<bits_t>(value);

			int32_t raw_exp = static_cast<int32_t>((bits >> tr::mantissa_bits) &
				((bits_t{ 1 } << tr::exponent_bits) - 1));
			out_exp = raw_exp - tr::bias;

			bits_t raw_mantissa = bits & tr::mantissa_mask;
			bits_t explicit_mantissa = raw_mantissa | (bits_t{ 1 } << tr::mantissa_bits);

			out_mantissa = static_cast<T>(explicit_mantissa) /
				T{ bits_t{ 1 } << tr::mantissa_bits };
		}

		template <traits::ieee754_fp T>
		constexpr T compile_time_log(T x)
		{
			if (x <= T{ 0 })
				throw "Logarithm input must be greater than zero.";

			int base2_exp = 0;
			T mantissa = T{ 0 };
			extract_exponent_and_mantissa(x, base2_exp, mantissa);

			T y = (mantissa - T{ 1 }) / (mantissa + T{ 1 });

			for (int i = 0; i < 3; ++i)
			{
				T ey = compile_time_exp(y);
				T t = (mantissa - ey) / (mantissa + ey);
				y = y + T{ 2 } * t;
			}

			return y + static_cast<T>(base2_exp) * math::ln_2<T>;
		}
	}

	template <std::size_t Steps, traits::ieee754_fp T>
	constexpr std::array<T, Steps> linear(T min_val, T max_val)
	{
		static_assert(Steps > 1, "The number of steps must be greater than 1.");

		std::array<T, Steps> values{};
		T step = (max_val - min_val) / T{ Steps - 1 };

		for (std::size_t i = 0; i < Steps; ++i)
			values[i] = min_val + step * static_cast<T>(i);

		return values;
	}

	template <std::size_t Steps, traits::ieee754_fp T>
	constexpr std::array<T, Steps> exponential(T min_val, T max_val)
	{
		static_assert(Steps > 1, "The number of steps must be greater than 1.");

		std::array<T, Steps> values{};
		T log_min = detail::compile_time_log(min_val);
		T log_max = detail::compile_time_log(max_val);
		T step = (log_max - log_min) / T{ Steps - 1 };

		for (std::size_t i = 0; i < Steps; ++i)
			values[i] = detail::compile_time_exp(log_min + step * static_cast<T>(i));

		return values;
	}
}