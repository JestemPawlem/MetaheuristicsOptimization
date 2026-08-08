#pragma once

#include <cstddef>

#include <mh/core/objective.h>
#include <mh/traits/float_traits.h>
#include <mh/utils/math.h>


namespace mh::objectives
{
	template <traits::ieee754_fp T, std::size_t Dim>
	struct sphere
	{
		constexpr T operator()(const T* __restrict x) const noexcept
		{
			T sum{};

			for (std::size_t i{}; i < Dim; ++i)
			{
				T xi = x[i] - sqrt_2<T>;
				sum += pow2(xi);
			}

			return sum;
		}
	};

	template <traits::ieee754_fp T, std::size_t Dim>
	auto sphere_objective() noexcept
	{
		return objective<sphere<T, Dim>, T, Dim>
		{
			{ T{ -10 + sqrt_2<T> }, T{ 10 + sqrt_2<T> } },
				"sphere"
		};
	}
}