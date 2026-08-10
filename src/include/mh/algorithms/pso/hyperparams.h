#pragma once

#include <array>
#include <cstddef>
#include <format>
#include <print>
#include <string>
#include <string_view>


namespace mh::algorithms::pso
{
	template <
		std::size_t PopulationSize,
		std::size_t NumIterations,
		typename Objective,
		std::size_t InertiasSize,
		std::size_t AccCoefs1Size,
		std::size_t AccCoefs2Size>
	class hyperparams
	{
	public:
		using objective_t = Objective;
		using value_type = typename Objective::value_type;

		static constexpr std::size_t dimension = Objective::dimension;
		static constexpr std::size_t grid_size =
			InertiasSize * AccCoefs1Size * AccCoefs2Size;

		static constexpr std::array<std::string_view, 3> csv_headers =
		{
			"inertia",
			"acc_coef_1",
			"acc_coef_2"
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
				std::format("{:.2f}", inertia(id)),
				std::format("{:.5f}", acc_coef_1(id)),
				std::format("{:.5f}", acc_coef_2(id))
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
			std::println("[{:>6}/{}] inertia: {:.5f} | acc coef 1: {:.5f} | acc coef 2: {:.5f}",
				id + 1, grid_size,
				inertia(id),
				acc_coef_1(id),
				acc_coef_2(id));
		}


		static constexpr std::size_t population_size = PopulationSize;
		static constexpr std::size_t num_iterations = NumIterations;

		constexpr hyperparams(
			objective_t objective,
			std::array<value_type, InertiasSize> inertias,
			std::array<value_type, AccCoefs1Size> acc_coefs_1,
			std::array<value_type, AccCoefs2Size> acc_coefs_2) :
			objective_(objective),
			inertias_(inertias),
			acc_coefs_1_(acc_coefs_1),
			acc_coefs_2_(acc_coefs_2)
		{}

		constexpr const objective_t& objective() const noexcept
		{
			return objective_;
		}

		constexpr value_type inertia(std::size_t id) const noexcept
		{
			const std::size_t idx = id / (AccCoefs1Size * AccCoefs2Size);
			return inertias_[idx];
		}

		constexpr value_type acc_coef_1(std::size_t id) const noexcept
		{
			const std::size_t idx = (id / AccCoefs2Size) % AccCoefs1Size;
			return acc_coefs_1_[idx];
		}

		constexpr value_type acc_coef_2(std::size_t id) const noexcept
		{
			const std::size_t idx = id % AccCoefs2Size;
			return acc_coefs_2_[idx];
		}

	private:
		objective_t objective_;

		std::array<value_type, InertiasSize> inertias_;
		std::array<value_type, AccCoefs1Size> acc_coefs_1_;
		std::array<value_type, AccCoefs2Size> acc_coefs_2_;
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
		std::array<typename Objective::value_type, N1> inertias,
		std::array<typename Objective::value_type, N2> acc_coefs_1,
		std::array<typename Objective::value_type, N3> acc_coefs_2)
	{
		return hyperparams<PopulationSize, NumIterations, Objective, N1, N2, N3>
		{
			objective,
			inertias,
			acc_coefs_1,
			acc_coefs_2
		};
	}
}