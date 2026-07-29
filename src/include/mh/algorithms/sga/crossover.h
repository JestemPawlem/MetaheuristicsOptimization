#pragma once

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <utility>

#include <mh/traits/float_traits.h>
#include <mh/utils/random.h>


namespace mh::algorithms::sga
{
	template <traits::ieee754_fp T, std::size_t Dim>
	using crossover_function_t = void(*)(
		T* __restrict parent1,
		T* __restrict parent2,
		T crossover_prob,
		T lower_bound,
		T upper_bound);


	template <traits::ieee754_fp T, std::size_t Dim>
	struct crossover_operator
	{
		std::string_view name;
		crossover_function_t<T, Dim> function;

		template <typename... Args>
		auto operator()(Args&&... args) const
			-> decltype(function(std::forward<Args>(args)...))
		{
			return function(std::forward<Args>(args)...);
		}
	};


	template <traits::ieee754_fp T, std::size_t Dim>
	void arithmetic_crossover_fn(
		T* __restrict parent1,
		T* __restrict parent2,
		T crossover_prob,
		T lower_bound,
		T upper_bound)
	{
		namespace random = utils::random;

		if (random::value<T>() > crossover_prob)
			return;

		const T alpha = random::value<T>();

		for (std::size_t d{}; d < Dim; ++d)
		{
			const T p1 = parent1[d];
			const T p2 = parent2[d];

			parent1[d] = std::clamp(alpha * p1 + (T{ 1 } - alpha) * p2,
				lower_bound, upper_bound);
			parent2[d] = std::clamp((T{ 1 } - alpha) * p1 + alpha * p2,
				lower_bound, upper_bound);
		}
	}

	template <traits::ieee754_fp T, std::size_t Dim>
	void blend_crossover_fn(
		T* __restrict parent1,
		T* __restrict parent2,
		T crossover_prob,
		T lower_bound,
		T upper_bound)
	{
		namespace random = utils::random;

		if (random::value<T>() > crossover_prob)
			return;

		constexpr T alpha = T{ 0.5 };
		constexpr T range_factor = T{ 1 } + T{ 2 } * alpha;

		for (std::size_t d{}; d < Dim; ++d)
		{
			const T p1 = parent1[d];
			const T p2 = parent2[d];

			const T min_val = std::min(p1, p2);
			const T diff = std::abs(p1 - p2);

			const T lower = min_val - alpha * diff;
			const T range = diff * range_factor;

			parent1[d] = std::clamp(lower + random::value<T>() * range, lower_bound, upper_bound);
			parent2[d] = std::clamp(lower + random::value<T>() * range, lower_bound, upper_bound);
		}
	}


	template <traits::ieee754_fp T, std::size_t Dim>
	constexpr crossover_operator<T, Dim> arithmetic_crossover
	{
		"Arithmetic",
		&arithmetic_crossover_fn<T, Dim>
	};

	template <traits::ieee754_fp T, std::size_t Dim>
	constexpr crossover_operator<T, Dim> blend_crossover
	{
		"Blend",
		&blend_crossover_fn<T, Dim>
	};
}