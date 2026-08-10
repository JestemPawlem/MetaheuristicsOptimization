#pragma once

#include <array>
#include <cstddef>
#include <format>
#include <print>
#include <string>
#include <string_view>
#include <mh/algorithms/de/mutation.h>

namespace mh::algorithms::de
{
	template <
		std::size_t PopulationSize,
		std::size_t NumIterations,
		typename Objective,
		std::size_t MutationsSize,
		std::size_t ScalingFactorsSize,
		std::size_t CrossoverRatesSize>
	class hyperparams
	{
	public:
		using objective_t = Objective;
		using value_type = typename Objective::value_type;

		static constexpr std::size_t dimension = Objective::dimension;
		static constexpr std::size_t grid_size =
			MutationsSize * ScalingFactorsSize * CrossoverRatesSize;

		static constexpr std::array<std::string_view, 3> csv_headers =
		{
			"mutation",
			"scaling_factor",
			"crossover_rate"
		};

		static constexpr std::array<std::string_view, 4> csv_meta_headers =
		{
			"objective",
			"dimensionality",
			"population_size",
			"num_iterations"
		};

		std::array<std::string, csv_headers.size()> csv_params(std::size_t id) const
		{
			return
			{
				std::string(mutation(id).name),
				std::format("{:.5f}", scaling_factor(id)),
				std::format("{:.5f}", crossover_rate(id))
			};
		}

		std::array<std::string, csv_meta_headers.size()> csv_meta_params() const
		{
			return
			{
				std::string(objective_.name),
				std::format("{}", dimension),
				std::format("{}", population_size),
				std::format("{}", num_iterations)
			};
		}

		void print_log(std::size_t id) const
		{
			std::println("[{:>6}/{}] mutation: {:<12} | scaling_factor (F): {:<7.5f} | crossover_rate (CR): {:<7.5f}",
				id + 1, grid_size,
				mutation(id).name,
				scaling_factor(id),
				crossover_rate(id));
		}


		static constexpr std::size_t population_size = PopulationSize;
		static constexpr std::size_t num_iterations = NumIterations;

		constexpr hyperparams(
			objective_t objective,
			std::array<de::mutation_operator<value_type, dimension, population_size>, MutationsSize> mutations,
			std::array<value_type, ScalingFactorsSize> scaling_factors,
			std::array<value_type, CrossoverRatesSize> crossover_rates) :
			objective_(objective),
			mutations_(mutations),
			scaling_factors_(scaling_factors),
			crossover_rates_(crossover_rates)
		{}

		constexpr const objective_t& objective() const noexcept
		{
			return objective_;
		}

		constexpr const de::mutation_operator<value_type, dimension, population_size>& mutation(std::size_t id) const noexcept
		{
			const std::size_t idx = id / (ScalingFactorsSize * CrossoverRatesSize);
			return mutations_[idx];
		}

		constexpr value_type scaling_factor(std::size_t id) const noexcept
		{
			const std::size_t idx = (id / CrossoverRatesSize) % ScalingFactorsSize;
			return scaling_factors_[idx];
		}

		constexpr value_type crossover_rate(std::size_t id) const noexcept
		{
			const std::size_t idx = id % CrossoverRatesSize;
			return crossover_rates_[idx];
		}

	private:
		objective_t objective_;
		std::array<de::mutation_operator<value_type, dimension, population_size>, MutationsSize> mutations_;
		std::array<value_type, ScalingFactorsSize> scaling_factors_;
		std::array<value_type, CrossoverRatesSize> crossover_rates_;
	};


	template <
		std::size_t PopulationSize,
		std::size_t NumIterations,
		typename Objective,
		std::size_t N1,
		std::size_t N2,
		std::size_t N3>
	constexpr auto make_hyperparams(
		Objective objective,
		std::array<de::mutation_operator<typename Objective::value_type, Objective::dimension, PopulationSize>, N1> mutations,
		std::array<typename Objective::value_type, N2> scaling_factors,
		std::array<typename Objective::value_type, N3> crossover_rates)
	{
		return hyperparams<PopulationSize, NumIterations, Objective, N1, N2, N3>
		{
			objective,
			mutations,
			scaling_factors,
			crossover_rates
		};
	}
}