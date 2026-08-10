#pragma once

#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <limits>

#include <mh/core/hyperparams.h>
#include <mh/utils/random.h>


namespace mh::algorithms::pso
{
	template <core::hyperparams Hyperparams>
	struct particle_swarm_optimization
	{
		using T = typename Hyperparams::value_type;

		static constexpr std::size_t Dim = Hyperparams::dimension;
		static constexpr std::size_t population_size = Hyperparams::population_size;
		static constexpr std::size_t num_iterations = Hyperparams::num_iterations;

		static constexpr T vmax_factor = T{ 0.2 };


		static T run(const Hyperparams& params, std::size_t id)
		{
			namespace random = utils::random;

			const auto& objective = params.objective();
			const auto [lower_bound, upper_bound] = objective.bounds;

			const T inertia = params.inertia(id);
			const T acc_coef_1 = params.acc_coef_1(id);
			const T acc_coef_2 = params.acc_coef_2(id);

			const T vmax = (upper_bound - lower_bound) * vmax_factor;


			std::array<T, population_size* Dim> positions{};
			std::array<T, population_size* Dim> velocities{};
			std::array<T, population_size* Dim> best_personal_positions{};
			std::array<T, population_size> best_personal_fitnesses{};

			std::array<T, Dim> best_global_position{};

			T best_global_fitness = std::numeric_limits<T>::max();


			for (std::size_t p{}; p < population_size; ++p)
			{
				const std::size_t offset = p * Dim;

				for (std::size_t d{}; d < Dim; ++d)
				{
					positions[offset + d] =
						lower_bound +
						random::value<T>() *
						(upper_bound - lower_bound);

					velocities[offset + d] =
						random::value<T>() * 2 * vmax - vmax;

					best_personal_positions[offset + d] =
						positions[offset + d];
				}

				const T fitness = objective(positions.data() + offset);

				best_personal_fitnesses[p] = fitness;

				if (fitness < best_global_fitness)
				{
					best_global_fitness = fitness;
					std::copy_n(positions.data() + offset, Dim,
						best_global_position.begin());
				}
			}


			for (std::size_t i{}; i < num_iterations; ++i)
			{
				for (std::size_t p{}; p < population_size; ++p)
				{
					const std::size_t offset = p * Dim;

					for (std::size_t d{}; d < Dim; ++d)
					{
						const T r1 = random::value<T>();
						const T r2 = random::value<T>();

						T& velocity = velocities[offset + d];
						T& position = positions[offset + d];

						velocity =
							velocity * inertia
							+ (best_personal_positions[offset + d] - position)
							* (r1 * acc_coef_1)
							+ (best_global_position[d] - position)
							* (r2 * acc_coef_2);

						velocity = std::clamp(
							velocity,
							-vmax,
							vmax);

						position += velocity;

						position = std::clamp(
							position,
							lower_bound,
							upper_bound);
					}

					const T fitness = objective(positions.data() + offset);

					if (fitness < best_personal_fitnesses[p])
					{
						best_personal_fitnesses[p] = fitness;

						for (std::size_t d{}; d < Dim; ++d)
							best_personal_positions[offset + d] = positions[offset + d];


						if (fitness < best_global_fitness)
						{
							best_global_fitness = fitness;
							std::copy_n(positions.data() + offset, Dim,
								best_global_position.begin());
						}
					}
				}
			}

			return best_global_fitness;
		}
	};
}