#pragma once

#include <array>
#include <mh/algorithms/de/differential_evolution.h>
#include <mh/algorithms/de/hyperparams.h>
#include <mh/algorithms/de/mutation.h>
#include <mh/objectives/rastrigin.h>
#include <mh/objectives/rosenbrock.h>
#include <mh/utils/series.h>

#include "runner.h"

namespace experiments::de
{
	inline void run_all()
	{
		namespace de = mh::algorithms::de;

		using T = float;
		constexpr std::size_t dimension = 3;
		constexpr std::size_t population_size = 100;
		constexpr std::size_t num_iterations = 150;

		constexpr auto rand_mut = de::random_mutation<T, dimension, population_size>;
		constexpr auto best_mut = de::best_mutation<T, dimension, population_size>;

		constexpr auto scaling_factors = mh::utils::series::exponential<25>(T{ 0.01 }, T{ 3.0 });
		constexpr auto crossover_rates = mh::utils::series::exponential<25>(T{ 0.01 }, T{ 1.0 });


		auto params3 = de::make_hyperparams<population_size, num_iterations>(
			mh::objectives::rosenbrock_objective<T, dimension>(),
			std::array{ rand_mut, best_mut },
			scaling_factors,
			crossover_rates);

		experiments::run_experiment<de::differential_evolution>("DE", "rosenbrock", params3);
	}
}