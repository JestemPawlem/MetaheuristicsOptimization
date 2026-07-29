#pragma once

#include <array>
#include <cmath>
#include <cstddef>

#include <mh/core/objective.h>
#include <mh/traits/float_traits.h>
#include <mh/utils/math.h>


namespace mh::objectives
{
	template <traits::ieee754_fp T, std::size_t Dim>
	struct schwefel
	{
		constexpr T operator()(const std::array<T, Dim>& x) const noexcept
		{
			T sum{};

			for (std::size_t i{}; i < Dim; ++i)
			{
				T xi = x[i] - sqrt_2<T>;
				sum += xi * std::sin(std::sqrt(std::abs(xi)));
			}

			return T{ 418.9829 } * Dim - sum;
		}
	};

	template <traits::ieee754_fp T, std::size_t Dim>
	auto schwefel_objective() noexcept
	{
		return objective<schwefel<T, Dim>, T, Dim>
		{
			{ T{ -500 + sqrt_2<T> }, T{ 500 + sqrt_2<T> } },
				"schwefel"
		};
	}
}