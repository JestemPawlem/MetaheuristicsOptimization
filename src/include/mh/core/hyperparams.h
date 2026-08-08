#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <string>
#include <string_view>

#include <mh/core/field.h>


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
		T::n_log_params;

		T::csv_headers;
		T::csv_meta_headers;

		requires T::csv_headers.size() == T::n_csv_params;
		requires T::csv_meta_headers.size() == T::n_csv_meta_params;

		{ hp.objective() } ->
			std::same_as<const typename T::objective_t&>;
		{ hp.csv_params(id) } ->
			std::same_as<std::array<std::string, T::n_csv_params>>;
		{ hp.csv_meta_params() } ->
			std::same_as<std::array<std::string, T::n_csv_meta_params>>;
		{ hp.log_params(id) } ->
			std::same_as<std::array<core::field_t, T::n_log_params>>;
	};
}