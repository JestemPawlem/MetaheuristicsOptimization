#pragma once

#include <cstddef>
#include <cstdint>
#include <concepts>
#include <stdfloat>


namespace mh::traits
{
	template <typename T>
	struct ieee754_traits;

	template <>
	struct ieee754_traits<float>
	{
		using uint_t = std::uint32_t;
		using int_t = std::int32_t;
		static constexpr std::int32_t bias = 127;
		static constexpr std::int32_t mantissa_bits = 23;
		static constexpr std::int32_t exponent_bits = 8;
		static constexpr std::uint32_t mantissa_mask = 0x007F'FFFFu;
		static constexpr std::uint32_t sign_mask = 0x8000'0000u;
	};

	template <>
	struct ieee754_traits<double>
	{
		using uint_t = std::uint64_t;
		using int_t = std::int64_t;
		static constexpr std::int32_t bias = 1023;
		static constexpr std::int32_t mantissa_bits = 52;
		static constexpr std::int32_t exponent_bits = 11;
		static constexpr std::uint64_t mantissa_mask = 0x000F'FFFF'FFFF'FFFFull;
		static constexpr std::uint64_t sign_mask = 0x8000'0000'0000'0000ull;
	};


	template <typename T>
	concept ieee754_fp =
		std::floating_point<T> &&
		requires { typename ieee754_traits<T>::uint_t; };
}