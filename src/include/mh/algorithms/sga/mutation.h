#pragma once

#include <algorithm>
#include <cstddef>
#include <string_view>
#include <utility>

#include <mh/traits/float_traits.h>
#include <mh/utils/random.h>


namespace mh::algorithms::sga
{
	template <traits::ieee754_fp T, std::size_t Dim>
	using mutation_function_t = void(*)(
		T* __restrict chromosome,
		T mutation_prob,
		T lower_bound,
		T upper_bound);


	template <traits::ieee754_fp T, std::size_t Dim>
	struct mutation_operator
	{
		std::string_view name;
		mutation_function_t<T, Dim> function;

		template <typename... Args>
		auto operator()(Args&&... args) const
			-> decltype(function(std::forward<Args>(args)...))
		{
			return function(std::forward<Args>(args)...);
		}
	};


	template <traits::ieee754_fp T, std::size_t Dim>
	void gaussian_mutation_fn(
		T* __restrict chromosome,
		T mutation_prob,
		T lower_bound,
		T upper_bound)
	{
		namespace random = utils::random;

		const std::size_t k = random::binomial<Dim>(mutation_prob);
		if (k == 0)
			return;

		const T range = upper_bound - lower_bound;
		const T stddev = range * T{ 0.1 };

		for (std::size_t i{}; i < k; ++i)
		{
			const std::size_t d = random::index(Dim);
			const T noise = random::normal<T>() * stddev;
			T val = chromosome[d] + noise;
			chromosome[d] = std::min(std::max(val, lower_bound), upper_bound);
		}
	}

	template <traits::ieee754_fp T, std::size_t Dim>
	void uniform_mutation_fn(
		T* __restrict chromosome,
		T mutation_prob,
		T lower_bound,
		T upper_bound)
	{
		namespace random = utils::random;

		const std::size_t k = random::binomial<Dim>(mutation_prob);
		if (k == 0)
			return;

		const T range = upper_bound - lower_bound;

		for (std::size_t i{}; i < k; ++i)
		{
			const std::size_t d = random::index(Dim);
			chromosome[d] = lower_bound + random::value<T>() * range;
		}
	}


	template <traits::ieee754_fp T, std::size_t Dim>
	constexpr mutation_operator<T, Dim> gaussian_mutation
	{
		"Gaussian",
		&gaussian_mutation_fn<T, Dim>
	};

	template <traits::ieee754_fp T, std::size_t Dim>
	constexpr mutation_operator<T, Dim> uniform_mutation
	{
		"Uniform",
		&uniform_mutation_fn<T, Dim>
	};
}