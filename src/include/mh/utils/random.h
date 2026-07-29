#pragma once

#include <bit>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <random>
#include <type_traits>

#include <mh/traits/float_traits.h>
#include <mh/utils/math.h>


namespace mh::utils::random
{
	template <std::unsigned_integral T = uint64_t>
	class xoshiro;

	template <>
	class xoshiro<uint64_t>
	{
	public:
		using result_type = uint64_t;

		explicit xoshiro(uint64_t seed) noexcept
		{
			seed_state(seed);
		}

		static constexpr uint64_t min() noexcept
		{
			return std::numeric_limits<uint64_t>::min();
		}

		static constexpr uint64_t max() noexcept
		{
			return std::numeric_limits<uint64_t>::max();
		}

		uint64_t operator()() noexcept
		{
			const uint64_t result = std::rotl(s_[1] * 5, 7) * 9;
			const uint64_t t = s_[1] << 17;

			s_[2] ^= s_[0];
			s_[3] ^= s_[1];
			s_[1] ^= s_[2];
			s_[0] ^= s_[3];

			s_[2] ^= t;
			s_[3] = std::rotl(s_[3], 45);

			return result;
		}

	private:
		uint64_t s_[4];
		
		void seed_state(uint64_t seed) noexcept
		{
			for (int i = 0; i < 4; ++i) {
				seed += 0x9e3779b97f4a7c15ULL;
				uint64_t z = seed;
				z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
				z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
				s_[i] = z ^ (z >> 31);
			}
		}
	};

	template <>
	class xoshiro<uint32_t>
	{
	public:
		using result_type = uint32_t;

		explicit xoshiro(uint32_t seed) noexcept
		{
			seed_state(seed);
		}

		static constexpr uint32_t min() noexcept
		{
			return std::numeric_limits<uint32_t>::min();
		}

		static constexpr uint32_t max() noexcept
		{
			return std::numeric_limits<uint32_t>::max();
		}

		uint32_t operator()() noexcept
		{
			const uint32_t result = std::rotl(s_[1] * 5, 7) * 9;
			const uint32_t t = s_[1] << 9;

			s_[2] ^= s_[0];
			s_[3] ^= s_[1];
			s_[1] ^= s_[2];
			s_[0] ^= s_[3];

			s_[2] ^= t;
			s_[3] = std::rotl(s_[3], 11);

			return result;
		}

	private:
		uint32_t s_[4];

		void seed_state(uint32_t seed) noexcept
		{
			for (int i = 0; i < 4; ++i) {
				seed += 0x9e3779b9U;
				uint32_t z = seed;
				z = (z ^ (z >> 16)) * 0x85ebca6bU;
				z = (z ^ (z >> 13)) * 0xc2b2ae35U;
				s_[i] = z ^ (z >> 16);
			}
		}
	};

	template <std::unsigned_integral T>
	inline thread_local xoshiro<T> rng(std::random_device{}());

	using size_t_rng_t = std::conditional_t<sizeof(std::size_t) == 8, std::uint64_t, std::uint32_t>;


	template <traits::ieee754_fp T>
	__forceinline T value()
	{
		using tr = traits::ieee754_traits<T>;
		using uint_t = typename tr::uint_t;

		constexpr auto shift = (sizeof(uint_t) * std::numeric_limits<unsigned char>::digits) - (tr::mantissa_bits + 1);
		constexpr T scale = std::bit_cast<T>(static_cast<uint_t>(tr::bias - (tr::mantissa_bits + 1)) << tr::mantissa_bits);

		return static_cast<T>(rng<uint_t>() >> shift) * scale;
	}


	__forceinline std::size_t index(std::size_t N)
	{
		std::uniform_int_distribution<std::size_t> dist(0, N - 1);

		return dist(rng<size_t_rng_t>);
	}


	template <std::size_t N, traits::ieee754_fp T>
	__forceinline std::size_t binomial(T p)
	{
		T r = value<T>();
		T pdf = std::pow(T{ 1 } - p, N);
		T cdf = pdf;

		const T ratio = p / (T{ 1 } - p);

		for (std::size_t k{}; k < N; ++k)
		{
			if (r < cdf)
				return k;
			pdf *= (T(N - k) / T(k + 1)) * ratio;
			cdf += pdf;
		}

		return N;
	}


	template <traits::ieee754_fp T>
	__forceinline T normal()
	{
		thread_local bool has_saved = false;
		thread_local T saved_val{};

		if (has_saved)
		{
			has_saved = false;
			return saved_val;
		}

		const T u1 = value<T>();
		const T u2 = value<T>();

		const T radius = std::sqrt(T{ -2 } * math::log(u1));
		const T theta = T{ 2 * 3.14159265358979323846 } * u2;

		saved_val = radius * std::sin(theta);
		has_saved = true;

		return radius * std::cos(theta);
	}
}