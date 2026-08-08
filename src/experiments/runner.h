#pragma once

#include <array>
#include <chrono>
#include <execution>
#include <format>
#include <mutex>
#include <ranges>
#include <print>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include <mh/core/hyperparams.h>
#include <mh/utils/csv_writer.h>
#include <mh/utils/statistics.h>

namespace experiments
{
	constexpr std::size_t num_trials = 100;
	constexpr float top_percent = 0.8f;

	static constexpr std::size_t n_stats_csv_params = 5;
	static constexpr std::array<std::string_view, n_stats_csv_params> stat_headers =
	{
		"best_value",
		"average_value_top_80",
		"median_value",
		"stddev_value",
		"average_time_ms"
	};

	template <template <typename> typename Algorithm, mh::core::hyperparams Hyperparams>
	void run_single(
		const Hyperparams& params,
		std::size_t id,
		mh::utils::csv_writer& csv_writer,
		std::mutex& write_mutex)
	{
		auto result = mh::utils::statistics(
			[&]()
			{
				return Algorithm<Hyperparams>::run(params, id);
			},
			num_trials,
			top_percent);

		constexpr std::size_t n_params =
			Hyperparams::n_csv_params + n_stats_csv_params;
		std::array<std::string, n_params> row{};

		const auto algorithm_params = params.csv_params(id);

		std::size_t i{};
		for (const std::string& param : algorithm_params)
			row[i++] = param;

		row[i++] = std::format("{:012.8f}", result.best_value);
		row[i++] = std::format("{:012.8f}", result.average_value_top_percent);
		row[i++] = std::format("{:012.8f}", result.median_value);
		row[i++] = std::format("{:012.8f}", result.stddev_value);
		row[i++] = std::format("{:06.2f}", result.average_time_ms);

		{
			std::lock_guard lock(write_mutex);
			csv_writer.write_row(std::span<const std::string>(row));
		}

		params.print_log(id);
	}

	template <template <typename> typename Algorithm, mh::core::hyperparams Hyperparams>
	void run_hyperparam_grid_parallel(
		const Hyperparams& params,
		mh::utils::csv_writer& csv_writer)
	{
		std::mutex write_mutex;

		auto ids = std::views::iota(std::size_t{}, Hyperparams::grid_size);

		std::for_each(
			std::execution::par,
			ids.begin(),
			ids.end(),
			[&](std::size_t id)
			{
				run_single<Algorithm>(
					params,
					id,
					csv_writer,
					write_mutex);
			});
	}

	template <template <typename> typename Algorithm, mh::core::hyperparams Hyperparams>
	void run_experiment(
		std::string_view algo_name,
		std::string_view objective_name,
		const Hyperparams& params)
	{
		constexpr std::size_t n_headers =
			Hyperparams::n_csv_params + n_stats_csv_params;
		std::array<std::string_view, n_headers> headers{};

		std::size_t i{};
		for (auto header : Hyperparams::csv_headers)
			headers[i++] = header;

		for (auto header : stat_headers)
			headers[i++] = header;

		std::string filename = std::format("{}_results_{}",
			algo_name, objective_name);

		const auto meta_params = params.csv_meta_params();

		mh::utils::csv_writer writer(filename,
			std::span<const std::string_view>(headers),
			std::span<const std::string_view>(Hyperparams::csv_meta_headers),
			std::span<const std::string>(meta_params));

		std::println("Starting experiment: {} on {}...",
			algo_name, objective_name);

		auto start = std::chrono::steady_clock::now();

		run_hyperparam_grid_parallel<Algorithm>(
			params,
			writer);

		auto end = std::chrono::steady_clock::now();

		double duration =
			std::chrono::duration<double>(end - start).count();

		std::println("Finished {} in {:.2f}s", algo_name, duration);
	}
}