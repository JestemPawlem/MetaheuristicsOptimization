#pragma once

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <utility>

#include <mh/traits/float_traits.h>
#include <mh/utils/random.h>


// Note: I deliberately allow potential duplicate indices (r1, r2, r3) without rejection sampling.
// For PopulationSize >= 100, collision probability is under 3%.
// The negligible algorithmic impact does not justify the branch overhead.
namespace mh::algorithms::de
{
	template <traits::ieee754_fp T, std::size_t Dim, std::size_t PopulationSize>
	using mutation_function_t = void(*)(
		std::size_t curr_idx,
		std::size_t best_idx,
		const T* __restrict population,
		T* __restrict mutant,
		T F,
		T lower_bound,
		T upper_bound);


	template <traits::ieee754_fp T, std::size_t Dim, std::size_t PopulationSize>
	struct mutation_operator
	{
		std::string_view name;
		mutation_function_t<T, Dim, PopulationSize> function;

		template <typename... Args>
		auto operator()(Args&&... args) const
			-> decltype(function(std::forward<Args>(args)...))
		{
			return function(std::forward<Args>(args)...);
		}
	};

	
	template <traits::ieee754_fp T, std::size_t Dim, std::size_t PopulationSize>
	void random_mutation_fn(
		std::size_t curr_idx,
		std::size_t best_idx,
		const T* __restrict population,
		T* __restrict mutant,
		T F,
		T lower_bound,
		T upper_bound)
	{
		namespace random = utils::random;

		const std::size_t r1 = random::index(PopulationSize);
		const std::size_t r2 = random::index(PopulationSize);
		const std::size_t r3 = random::index(PopulationSize);

		const T* x_r1 = population + r1 * Dim;
		const T* x_r2 = population + r2 * Dim;
		const T* x_r3 = population + r3 * Dim;

		for (std::size_t d{}; d < Dim; ++d)
		{
			const T val = x_r1[d] + F * (x_r2[d] - x_r3[d]);
			mutant[d] = std::clamp(val, lower_bound, upper_bound);
		}
	}

	template <traits::ieee754_fp T, std::size_t Dim, std::size_t PopulationSize>
	void best_mutation_fn(
		std::size_t curr_idx,
		std::size_t best_idx,
		const T* __restrict population,
		T* __restrict mutant,
		T F,
		T lower_bound,
		T upper_bound)
	{
		namespace random = utils::random;

		const std::size_t r1 = random::index(PopulationSize);
		const std::size_t r2 = random::index(PopulationSize);

		const T* x_best = population + best_idx * Dim;
		const T* x_r1 = population + r1 * Dim;
		const T* x_r2 = population + r2 * Dim;

		for (std::size_t d{}; d < Dim; ++d)
		{
			const T val = x_best[d] + F * (x_r1[d] - x_r2[d]);
			mutant[d] = std::clamp(val, lower_bound, upper_bound);
		}
	}


	template <traits::ieee754_fp T, std::size_t Dim, std::size_t PopulationSize>
	constexpr mutation_operator<T, Dim, PopulationSize> random_mutation
	{
		"Random",
		&random_mutation_fn<T, Dim, PopulationSize>
	};

	template <traits::ieee754_fp T, std::size_t Dim, std::size_t PopulationSize>
	constexpr mutation_operator<T, Dim, PopulationSize> best_mutation
	{
		"Best",
		&best_mutation_fn<T, Dim, PopulationSize>
	};
}