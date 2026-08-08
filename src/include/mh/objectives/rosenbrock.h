#pragma once

#include <cstddef>

#include <mh/core/objective.h>
#include <mh/traits/float_traits.h>
#include <mh/utils/math.h>


namespace mh::objectives
{
	template <traits::ieee754_fp T, std::size_t Dim>
	struct rosenbrock
	{
		constexpr T operator()(const T* __restrict x) const noexcept
		{
			T result{};

			for (std::size_t i{}; i < Dim - 1; ++i)
			{
				T xi = x[i] - sqrt_2<T>;
				T xi1 = x[i + 1] - sqrt_2<T>;
				T term1 = xi1 - pow2(xi);
				T term2 = T{ 1 } - xi;

				result += T{ 100 } * pow2(term1) + pow2(term2);
			}

			return result;
		}
	};

	template <traits::ieee754_fp T, std::size_t Dim>
	auto rosenbrock_objective()
	{
		return objective<rosenbrock<T, Dim>, T, Dim>
		{
			{ T{ -5 + sqrt_2<T> }, T{ 10 + sqrt_2<T> } },
				"rosenbrock"
		};
	}
}