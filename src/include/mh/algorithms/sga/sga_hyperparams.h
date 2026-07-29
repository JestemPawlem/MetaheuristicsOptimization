#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <format>
#include <string_view>

#include <mh/core/field.h>


namespace mh::algorithms::sga
{
	template <
		std::size_t PopulationSize,
		std::size_t NumIterations,
		typename Objective,
		std::size_t SelectionsSize,
		std::size_t CrossoversSize,
		std::size_t MutationsSize,
		std::size_t CrossoverProbsSize,
		std::size_t MutationProbsSize>
	class hyperparams
	{
	public:
		using objective_t = Objective;
		using value_type = typename Objective::value_type;

		static constexpr std::size_t dimension = Objective::dimension;
		static constexpr std::size_t population_size = PopulationSize;
		static constexpr std::size_t num_iterations = NumIterations;

		static constexpr std::size_t grid_size =
			SelectionsSize * CrossoversSize * MutationsSize *
			CrossoverProbsSize * MutationProbsSize;

		static constexpr std::size_t n_csv_params = 5;
		static constexpr std::size_t n_csv_meta_params = 4;
		static constexpr std::size_t n_log_params = 7;

	private:
		objective_t objective_;

		std::array<sga::selection_operator<value_type, dimension, population_size>, SelectionsSize> selections_;
		std::array<sga::crossover_operator<value_type, dimension>, CrossoversSize> crossovers_;
		std::array<sga::mutation_operator<value_type, dimension>, MutationsSize> mutations_;
		std::array<value_type, CrossoverProbsSize> crossover_probs_;
		std::array<value_type, MutationProbsSize> mutation_probs_;

	public:
		static constexpr std::array<std::string_view, n_csv_params> csv_headers =
		{
			"selection",
			"crossover",
			"mutation",
			"crossover_prob",
			"mutation_prob"
		};

		static constexpr std::array<std::string_view, n_csv_meta_params> csv_meta_headers =
		{
			"objective",
			"dimensionality",
			"population_size",
			"num_iterations"
		};

		constexpr hyperparams(
			objective_t objective,
			std::array<sga::selection_operator<value_type, dimension, population_size>, SelectionsSize> selections,
			std::array<sga::crossover_operator<value_type, dimension>, CrossoversSize> crossovers,
			std::array<sga::mutation_operator<value_type, dimension>, MutationsSize> mutations,
			std::array<value_type, CrossoverProbsSize> crossover_probs,
			std::array<value_type, MutationProbsSize> mutation_probs)
			: objective_(objective),
			selections_(selections),
			crossovers_(crossovers),
			mutations_(mutations),
			crossover_probs_(crossover_probs),
			mutation_probs_(mutation_probs)
		{}

		constexpr const objective_t& objective() const noexcept
		{
			return objective_;
		}

		constexpr const sga::selection_operator<value_type, dimension, population_size>& selection(std::size_t id) const noexcept
		{
			const std::size_t idx =
				id / (CrossoversSize * MutationsSize * CrossoverProbsSize * MutationProbsSize);

			return selections_[idx];
		}

		constexpr const sga::crossover_operator<value_type, dimension>& crossover(std::size_t id) const noexcept
		{
			const std::size_t idx =
				(id / (MutationsSize * CrossoverProbsSize * MutationProbsSize)) % CrossoversSize;

			return crossovers_[idx];
		}

		constexpr const sga::mutation_operator<value_type, dimension>& mutation(std::size_t id) const noexcept
		{
			const std::size_t idx =
				(id / (CrossoverProbsSize * MutationProbsSize)) % MutationsSize;

			return mutations_[idx];
		}

		constexpr value_type crossover_prob(std::size_t id) const noexcept
		{
			const std::size_t idx =
				(id / MutationProbsSize) % CrossoverProbsSize;

			return crossover_probs_[idx];
		}

		constexpr value_type mutation_prob(std::size_t id) const noexcept
		{
			const std::size_t idx =
				id % MutationProbsSize;

			return mutation_probs_[idx];
		}

		std::array<std::string, n_csv_params> csv_params(std::size_t id) const
		{
			return
			{
				std::string(selection(id).name),
				std::string(crossover(id).name),
				std::string(mutation(id).name),
				std::format("{:.5f}", crossover_prob(id)),
				std::format("{:.5f}", mutation_prob(id))
			};
		}

		std::array<std::string, n_csv_meta_params> csv_meta_params() const
		{
			return
			{
				std::string(objective_.name),
				std::format("{}", dimension),
				std::format("{}", population_size),
				std::format("{}", num_iterations)
			};
		}

		std::array<core::field_t, n_log_params> log_params(std::size_t id) const
		{
			return
			{
				core::field_t{ "objective", objective_.name },
				core::field_t{ "dimensionality", std::to_string(dimension) },
				core::field_t{ "selection", std::format("{:<16}", selection(id).name) },
				core::field_t{ "crossover", std::format("{:<10}", crossover(id).name) },
				core::field_t{ "mutation", std::format("{:<10}", mutation(id).name) },
				core::field_t{ "crossover_prob", std::format("{:<7.5f}", crossover_prob(id)) },
				core::field_t{ "mutation_prob", std::format("{:<7.5f}", mutation_prob(id)) }
			};
		}
	};

	template <
		std::size_t PopulationSize,
		std::size_t NumIterations,
		typename Objective,
		std::size_t N1,
		std::size_t N2,
		std::size_t N3,
		std::size_t N4,
		std::size_t N5>
	constexpr auto make_hyperparams(
		Objective objective,
		std::array<sga::selection_operator<typename Objective::value_type, Objective::dimension, PopulationSize>, N1> selections,
		std::array<sga::crossover_operator<typename Objective::value_type, Objective::dimension>, N2> crossovers,
		std::array<sga::mutation_operator<typename Objective::value_type, Objective::dimension>, N3> mutations,
		std::array<typename Objective::value_type, N4> crossover_probs,
		std::array<typename Objective::value_type, N5> mutation_probs)
	{
		return hyperparams<
			PopulationSize,
			NumIterations,
			Objective,
			N1, N2, N3, N4, N5>
		{
			objective,
				selections,
				crossovers,
				mutations,
				crossover_probs,
				mutation_probs
		};
	}
}