#pragma once

#include <mh/algorithms/pso/particle_swarm_optimization.h>
#include <mh/algorithms/pso/hyperparams.h>
#include <mh/objectives/rastrigin.h>
#include <mh/utils/series.h>

#include "runner.h"

namespace experiments::pso
{
	inline void run_all()
	{
		namespace pso = mh::algorithms::pso;

		using T = float;

		constexpr auto inertias = mh::utils::series::linear<5>(T{ 0.4 }, T{ 0.8 });
		constexpr auto c1_series = mh::utils::series::exponential<25>(T{ 0.2 }, T{ 4.0 });
		constexpr auto c2_series = mh::utils::series::exponential<25>(T{ 0.2 }, T{ 4.0 });

		{
			constexpr std::size_t dimension = 3;
			constexpr std::size_t population_size = 50;
			constexpr std::size_t num_iterations = 150;

			auto params = pso::make_hyperparams<population_size, num_iterations>(
				mh::objectives::rastrigin_objective<T, dimension>(),
				inertias, c1_series, c2_series
			);

			run_experiment<pso::particle_swarm_optimization>(
				"PSO", "rastrigin", params);
		}
	}
}