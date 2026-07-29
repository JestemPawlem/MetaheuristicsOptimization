#pragma once

#include <array>

#include <mh/algorithms/sga/crossover.h>
#include <mh/algorithms/sga/mutation.h>
#include <mh/algorithms/sga/selection.h>
#include <mh/algorithms/sga/sga_hyperparams.h>
#include <mh/algorithms/sga/simple_genetic_algorithm.h>
#include <mh/objectives/rastrigin.h>
#include <mh/utils/series.h>

#include "runner.h"


namespace experiments::sga
{
	inline void run_all()
	{
		namespace sga = mh::algorithms::sga;

		using T = float;
		constexpr std::size_t dimension = 3;
		constexpr std::size_t population_size = 50;
		constexpr std::size_t num_iterations = 150;

		constexpr auto tournament_2_sel = sga::tournament_selection_k2<T, dimension, population_size>;
		constexpr auto tournament_3_sel = sga::tournament_selection_k3<T, dimension, population_size>;

		constexpr auto arithmetic_cross = sga::arithmetic_crossover<T, dimension>;
		constexpr auto blend_cross = sga::blend_crossover<T, dimension>;

		constexpr auto gaussian_mut = sga::gaussian_mutation<T, dimension>;
		constexpr auto uniform_mut = sga::uniform_mutation<T, dimension>;

		constexpr auto crossover_chances = mh::utils::series::exponential<50>(T{ 0.4 }, T{ 1.0 });
		constexpr auto mutation_chances = mh::utils::series::exponential<50>(T{ 0.02 }, T{ 0.3 });

		
		auto params3 = sga::make_hyperparams<population_size, num_iterations>(
			mh::objectives::rastrigin_objective<T, dimension>(),
			std::array{ tournament_2_sel, tournament_3_sel },
			std::array{ arithmetic_cross, blend_cross },
			std::array{ gaussian_mut, uniform_mut },
			crossover_chances, mutation_chances);

		experiments::run_experiment<sga::real_coded_genetic_algorithm>("SGA", "benchmark", params3);
	}
}