#pragma once

#include <cstddef>
#include <string_view>
#include <utility>

#include <mh/traits/float_traits.h>


namespace mh::algorithms::pso
{
	template <traits::ieee754_fp T>
	using inertia_function_t = T(*)(
		std::size_t curr_iter,
		std::size_t num_iterations,
		T initial_inertia);


	template <traits::ieee754_fp T>
	struct inertia_policy
	{
		std::string_view name;
		inertia_function_t<T> function;

		template <typename... Args>
		auto operator()(Args&&... args) const
			-> decltype(function(std::forward<Args>(args)...))
		{
			return function(std::forward<Args>(args)...);
		}
	};


	template <traits::ieee754_fp T>
	T constant_inertia_fn(
		std::size_t curr_iter,
		std::size_t num_iterations,
		T initial_inertia)
	{
		return initial_inertia;
	}

	template <traits::ieee754_fp T>
	T linear_decay_inertia_fn(
		std::size_t curr_iter,
		std::size_t num_iterations,
		T initial_inertia)
	{
		const T progress = static_cast<T>(curr_iter) / static_cast<T>(num_iterations);
		return initial_inertia - progress * initial_inertia;
	}

	template <traits::ieee754_fp T>
	T exponential_decay_inertia_fn(
		std::size_t curr_iter,
		std::size_t num_iterations,
		T initial_inertia)
	{
		constexpr T target_inertia = T{ 0.1 };

		const T progress = static_cast<T>(curr_iter) / static_cast<T>(num_iterations - 1);
		return target_inertia + (initial_inertia - target_inertia) * std::exp(-T{ 3.0 } * progress);
	}


	template <traits::ieee754_fp T>
	constexpr inertia_policy<T> constant_inertia
	{
		"Constant",
		&constant_inertia_fn<T>
	};

	template <traits::ieee754_fp T>
	constexpr inertia_policy<T> linear_decay_inertia
	{
		"Linear",
		&linear_decay_inertia_fn<T>
	};

	template <traits::ieee754_fp T>
	constexpr inertia_policy<T> exponential_decay_inertia
	{
		"Exponential",
		&exponential_decay_inertia_fn<T>
	};
}