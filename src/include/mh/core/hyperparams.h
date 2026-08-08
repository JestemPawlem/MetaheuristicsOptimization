#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>


namespace mh::core
{
	template <typename T>
	concept hyperparams = requires(const T & hp, std::size_t id)
	{
		typename T::objective_t;
		typename T::value_type;

		T::dimension;
		T::grid_size;

		T::n_csv_params;
		T::n_csv_meta_params;

		requires std::same_as<
			std::remove_cvref_t<decltype(T::csv_headers)>,
			std::array<std::string_view, T::n_csv_params>
		>;
		requires std::same_as<
			std::remove_cvref_t<decltype(T::csv_meta_headers)>,
			std::array<std::string_view, T::n_csv_meta_params>
		>;

		{ hp.objective() } ->
			std::same_as<const typename T::objective_t&>;

		{ hp.csv_params(id) } ->
			std::same_as<std::array<std::string, T::n_csv_params>>;
		{ hp.csv_meta_params() } ->
			std::same_as<std::array<std::string, T::n_csv_meta_params>>;

		{ hp.print_log(id) } -> std::same_as<void>;
	};
}