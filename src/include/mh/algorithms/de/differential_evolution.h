#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <utility>
#include <mh/core/hyperparams.h>
#include <mh/utils/random.h>

namespace mh::algorithms::de
{
	template <core::hyperparams Hyperparams>
	struct differential_evolution
	{
		using T = typename Hyperparams::value_type;

		static constexpr std::size_t Dim = Hyperparams::dimension;
		static constexpr std::size_t population_size = Hyperparams::population_size;
		static constexpr std::size_t num_iterations = Hyperparams::num_iterations;

		static constexpr std::size_t population_size_flat = population_size * Dim;

		static T run(const Hyperparams& params, std::size_t id)
		{
			namespace random = utils::random;

			const auto& objective = params.objective();
			const auto [lower_bound, upper_bound] = objective.bounds;
			const T range = upper_bound - lower_bound;

			const auto& mutation = params.mutation(id);

			const T F = params.scaling_factor(id);
			const T CR = params.crossover_rate(id);

			std::array<T, population_size_flat> population{};
			std::array<T, population_size> fitnesses{};

			std::array<T, population_size_flat> next_population{};
			std::array<T, population_size> next_fitnesses{};

			std::size_t best_global_idx{};
			T best_global_fitness = std::numeric_limits<T>::max();


			for (std::size_t i{}; i < population_size; ++i)
			{
				const std::size_t offset = i * Dim;

				for (std::size_t d{}; d < Dim; ++d)
				{
					population[offset + d] = lower_bound + random::value<T>() * range;
				}

				const T fit = objective(population.data() + offset);
				fitnesses[i] = fit;

				if (fit < best_global_fitness)
				{
					best_global_fitness = fit;
					best_global_idx = i;
				}
			}


			std::array<T, Dim> mutant{};
			std::array<T, Dim> trial{};

			for (std::size_t iter{}; iter < num_iterations; ++iter)
			{
				for (std::size_t i{}; i < population_size; ++i)
				{
					const std::size_t offset = i * Dim;

					mutation(i, best_global_idx, population.data(),
						mutant.data(), F, lower_bound, upper_bound);

					const std::size_t forced_dim = random::index(Dim);

					for (std::size_t d{}; d < Dim; ++d)
						if (d == forced_dim || random::value<T>() < CR)
							trial[d] = mutant[d];
						else
							trial[d] = population[offset + d];

					const T trial_fitness = objective(trial.data());

					if (trial_fitness <= fitnesses[i])
					{
						std::copy_n(trial.data(), Dim, next_population.data() + offset);
						next_fitnesses[i] = trial_fitness;
					}
					else
					{
						std::copy_n(population.data() + offset, Dim, next_population.data() + offset);
						next_fitnesses[i] = fitnesses[i];
					}
				}

				std::swap(population, next_population);
				std::swap(fitnesses, next_fitnesses);

				const auto min_it = std::min_element(fitnesses.begin(), fitnesses.end());
				best_global_idx = static_cast<std::size_t>(min_it - fitnesses.begin());
				best_global_fitness = *min_it;
			}

			return best_global_fitness;
		}
	};
}