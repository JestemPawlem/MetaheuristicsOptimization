#pragma once

#include <chrono>
#include <utility>
#include <functional>

#include <mh/traits/float_traits.h>


namespace mh::utils
{
	template <traits::ieee754_fp T>
	struct timer_result
	{
		T value{};
		double time_ms{};
	};


	template <typename Func, typename... Args>
	auto timer(Func&& func, Args&&... args)
	{
		using return_type = decltype(std::invoke(std::forward<Func>(func), std::forward<Args>(args)...));

		const auto start = std::chrono::steady_clock::now();

		return_type result = std::invoke(
			std::forward<Func>(func),
			std::forward<Args>(args)...);

		const auto end = std::chrono::steady_clock::now();

		const double time_ms = std::chrono::duration<double, std::milli>(end - start).count();

		return timer_result{ std::move(result), time_ms };
	}
}
