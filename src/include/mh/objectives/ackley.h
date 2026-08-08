#pragma once

#include <cmath>
#include <cstddef>

#include <mh/core/objective.h>
#include <mh/traits/float_traits.h>
#include <mh/utils/math.h>


namespace mh::objectives
{
	template <traits::ieee754_fp T, std::size_t Dim>
	struct ackley
	{
		constexpr T operator()(const T* __restrict x) const noexcept
		{
			T sum_sq{};
			T sum_cos{};

			for (std::size_t i{}; i < Dim; ++i)
			{
				T xi = x[i] - sqrt_2<T>;
				sum_sq += pow2(xi);
				sum_cos += core::cos_two_pi(xi);
			}

			T term1 = T{ -20 } * std::exp(-T{ 0.2 } * std::sqrt(sum_sq / Dim));
			T term2 = -std::exp(sum_cos / Dim);

			return term1 + term2 + T{ 20 } + e<T>;
		}
	};

	template <traits::ieee754_fp T, std::size_t Dim>
	auto ackley_objective()
	{
		return objective<ackley<T, Dim>, T, Dim>
		{
			{ T{ -32.768 + sqrt_2<T> }, T{ 32.768 + sqrt_2<T> } },
				"ackley"
		};
	}
}