#pragma once

#include <array>
#include <cstddef>

#include <mh/core/objective.h>
#include <mh/traits/float_traits.h>
#include <mh/utils/math.h>


namespace mh::objectives
{
	template <traits::ieee754_fp T, std::size_t Dim>
	struct rastrigin
	{
		constexpr T operator()(const std::array<T, Dim>& x) const noexcept
		{
			using namespace utils::math;

			T result{ 10 * Dim };

			for (std::size_t i{}; i < Dim; ++i)
			{
				T xi = x[i] - sqrt_2<T>;
				result += pow2(xi) - 10 * cos_two_pi(xi);
			}

			return result;
		}
	};


	template <traits::ieee754_fp T, std::size_t Dim>
	auto rastrigin_objective()
	{
		using namespace utils::math;

		return core::objective<rastrigin<T, Dim>, T, Dim>
		{
			{ T{ -5.12 + sqrt_2<T> }, T{ 5.12 + sqrt_2<T> } },
			"rastrigin"
		};
	}
}