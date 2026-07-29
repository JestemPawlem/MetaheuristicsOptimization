#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <utility>

#include <mh/algorithms/sga/crossover.h>
#include <mh/algorithms/sga/mutation.h>
#include <mh/algorithms//sga/selection.h>
#include <mh/utils/random.h>


namespace mh::algorithms::sga
{
	template <typename Hyperparams>
	struct real_coded_genetic_algorithm
	{
		using T = typename Hyperparams::value_type;

		static constexpr std::size_t Dim = Hyperparams::dimension;
		static constexpr std::size_t population_size = Hyperparams::population_size;
		static constexpr std::size_t num_iterations = Hyperparams::num_iterations;

		static constexpr std::size_t population_size_flat =
			population_size * Dim;


		static void evaluate(
			const std::array<T, population_size_flat>& population,
			std::array<T, population_size>& fitnesses,
			std::size_t index,
			const auto& objective)
		{
			std::array<T, Dim> point{};
			const std::size_t offset = index * Dim;

			for (std::size_t d{}; d < Dim; ++d)
				point[d] = population[offset + d];

			fitnesses[index] = objective(point);
		}


		static T run(const Hyperparams& params, std::size_t id)
		{
			namespace random = utils::random;

			const auto& objective = params.objective();

			const auto& selection = params.selection(id);
			const auto& crossover = params.crossover(id);
			const auto& mutation = params.mutation(id);

			const T crossover_prob = params.crossover_prob(id);
			const T mutation_prob = params.mutation_prob(id);
			const auto [lower_bound, upper_bound] = objective.bounds;

			std::array<T, population_size_flat> population{};
			std::array<T, population_size> fitnesses{};

			std::array<T, population_size_flat> next_population{};
			std::array<T, population_size> next_fitnesses{};


			const T range = upper_bound - lower_bound;

			for (std::size_t i{}; i < population_size; ++i)
			{
				for (std::size_t d{}; d < Dim; ++d)
					population[i * Dim + d] =
					lower_bound + random::value<T>() * range;

				evaluate(population, fitnesses, i, objective);
			}


			for (std::size_t iter{}; iter < num_iterations; ++iter)
			{
				const auto [min_it, max_it] =
					std::minmax_element(fitnesses.begin(), fitnesses.end());

				const T best_global_fitness = *min_it;
				const T worst_global_fitness = *max_it;

				const std::size_t best_global_idx =
					static_cast<std::size_t>(
						std::distance(fitnesses.begin(), min_it));


				selection(
					population,
					fitnesses,
					next_population,
					next_fitnesses,
					worst_global_fitness,
					best_global_fitness);


				std::array<T, Dim> best_individual{};

				std::copy_n(
					population.data() + best_global_idx * Dim,
					Dim,
					best_individual.data());


				for (std::size_t j{}; j < population_size - 1; j += 2)
				{
					T* parent1 = next_population.data() + j * Dim;
					T* parent2 = next_population.data() + (j + 1) * Dim;

					crossover(parent1, parent2, crossover_prob, lower_bound, upper_bound);

					mutation(parent1, mutation_prob, lower_bound, upper_bound);
					mutation(parent2, mutation_prob, lower_bound, upper_bound);

					evaluate(next_population, next_fitnesses, j, objective);
					evaluate(next_population, next_fitnesses, j + 1, objective);
				}


				const auto worst_it =
					std::max_element(next_fitnesses.begin(), next_fitnesses.end());

				const std::size_t worst_idx =
					static_cast<std::size_t>(
						std::distance(next_fitnesses.begin(), worst_it));


				std::copy_n(
					best_individual.data(),
					Dim,
					next_population.data() + worst_idx * Dim);

				next_fitnesses[worst_idx] = best_global_fitness;


				std::swap(population, next_population);
				std::swap(fitnesses, next_fitnesses);
			}


			return *std::min_element(fitnesses.begin(), fitnesses.end());
		}
	};
}