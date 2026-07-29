#pragma once

#include <vector>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <cmath>

#include <mh/utils/timer.h>
#include <mh/traits/float_traits.h>


namespace mh::utils
{
	template <traits::ieee754_fp T>
	struct statistics_result
	{
		T best_value{};
		T average_value_top_percent{};
		T median_value{};
		T stddev_value{};
		double average_time_ms{};
	};


	template <typename Func, typename... Args>
	auto statistics(Func&& func, std::size_t n_trials, float top_percent, Args&&... args)
	{
		using value_type = decltype(std::invoke(std::forward<Func>(func), std::forward<Args>(args)...));
		using timer_result_type = timer_result<value_type>;

		if (n_trials == 0)
			throw std::invalid_argument("n must be greater than 0");
		if (top_percent <= 0.0f || top_percent > 1.0f)
			throw std::invalid_argument("top_percents values must be in (0, 1]");


		std::vector<timer_result_type> results(n_trials);
		for (std::size_t i{}; i < n_trials; ++i)
			results[i] = timer(std::forward<Func>(func), std::forward<Args>(args)...);

		std::sort(results.begin(), results.end(),
			[](const timer_result_type& a, const timer_result_type& b)
			{
				return a.value < b.value;
			});


		value_type best_value = results.front().value;

		value_type average_value_top_percent{};

		std::size_t top_n = std::max<std::size_t>(1, static_cast<std::size_t>(n_trials * top_percent));
		value_type sum = std::accumulate(results.begin(), results.begin() + top_n, value_type{ 0 },
			[](value_type acc, const timer_result_type& r)
			{
				return acc + r.value;
			});
		average_value_top_percent = sum / top_n;

		value_type average_value = std::accumulate(results.begin(), results.end(), value_type{ 0 },
			[](value_type acc, const timer_result_type& r)
			{
				return acc + r.value;
			}) / n_trials;

		value_type median_value = (n_trials % 2 == 0) ?
			(results[n_trials / 2 - 1].value + results[n_trials / 2].value) / 2 :
			results[n_trials / 2].value;

		value_type variance = std::accumulate(results.begin(), results.end(), value_type{ 0 },
			[average_value](value_type acc, const timer_result_type& r)
			{
				value_type diff = r.value - average_value;
				return acc + diff * diff;
			}) / n_trials;

		value_type stddev = std::sqrt(variance);


		double average_time_ms = std::accumulate(results.begin(), results.end(), 0.0,
			[](double acc, const timer_result_type& r)
			{
				return acc + r.time_ms;
			}) / n_trials;


		return statistics_result<value_type>{
			best_value,
				average_value_top_percent,
				median_value,
				stddev,
				average_time_ms,
		};
	}
}