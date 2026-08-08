#pragma once

#include <cmath>
#include <cstddef>

#include <mh/core/objective.h>
#include <mh/traits/float_traits.h>
#include <mh/utils/math.h>


namespace mh::objectives
{
	template <traits::ieee754_fp T, std::size_t Dim>
	struct griewank
	{
		constexpr T operator()(const T* __restrict x) const noexcept
		{
			T sum_sq{};
			T prod_cos = T{ 1 };

			for (std::size_t i{}; i < Dim; ++i)
			{
				T xi = x[i] - sqrt_2<T>;
				sum_sq += utils::math::pow2(xi);
				prod_cos *= std::cos(xi / std::sqrt(static_cast<T>(i + 1)));
			}

			return sum_sq * T{ 1 } / T{ 4000 } - prod_cos + T{ 1 };
		}
	};

	template <traits::ieee754_fp T, std::size_t Dim>
	auto griewank_objective()
	{
		return objective<griewank<T, Dim>, T, Dim>
		{
			{ T{ -600 + sqrt_2<T> }, T{ 600 + sqrt_2<T> } },
				"griewank"
		};
	}
}