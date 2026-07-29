#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <string_view>
#include <utility>

#include <mh/traits/float_traits.h>
#include <mh/utils/random.h>


namespace mh::algorithms::sga
{
	template <traits::ieee754_fp T, std::size_t Dim, std::size_t PopulationSize>
	using selection_function_t = void(*)(
		const std::array<T, PopulationSize* Dim>& old_population,
		const std::array<T, PopulationSize>& old_fitnesses,
		std::array<T, PopulationSize* Dim>& new_population,
		std::array<T, PopulationSize>& new_fitnesses,
		T worst_fitness,
		T best_fitness);

	template <traits::ieee754_fp T, std::size_t Dim, std::size_t PopulationSize>
	struct selection_operator
	{
		std::string_view name;
		selection_function_t<T, Dim, PopulationSize> function;

		template <typename... Args>
		auto operator()(Args&&... args) const
			-> decltype(function(std::forward<Args>(args)...))
		{
			return function(std::forward<Args>(args)...);
		}
	};


	template <traits::ieee754_fp T, std::size_t Dim, std::size_t PopulationSize, std::size_t K = 2>
	void tournament_selection_fn(
		const std::array<T, PopulationSize* Dim>& old_population,
		const std::array<T, PopulationSize>& old_fitnesses,
		std::array<T, PopulationSize* Dim>& new_population,
		std::array<T, PopulationSize>& new_fitnesses,
		T worst_fitness,
		T best_fitness)
	{
		namespace random = utils::random;

		for (std::size_t i{}; i < PopulationSize; ++i)
		{
			std::size_t best = random::index(PopulationSize);

			for (std::size_t j{}; j < K - 1; ++j)
			{
				const std::size_t competitor = random::index(PopulationSize);
				const bool is_better = old_fitnesses[competitor] < old_fitnesses[best];
				best = is_better ? competitor : best;
			}

			std::copy_n(old_population.data() + best * Dim, Dim,
				new_population.data() + i * Dim);
			new_fitnesses[i] = old_fitnesses[best];
		}
	}

	template <traits::ieee754_fp T, std::size_t Dim, std::size_t PopulationSize>
	constexpr selection_operator<T, Dim, PopulationSize> tournament_selection_k2
	{
		"Tournament (k=2)",
		&tournament_selection_fn<T, Dim, PopulationSize, 2>
	};

	template <traits::ieee754_fp T, std::size_t Dim, std::size_t PopulationSize>
	constexpr selection_operator<T, Dim, PopulationSize> tournament_selection_k3
	{
		"Tournament (k=3)",
		&tournament_selection_fn<T, Dim, PopulationSize, 3>
	};
}