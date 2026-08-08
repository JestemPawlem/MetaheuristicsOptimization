#pragma once

#include <cstddef>
#include <utility>
#include <string_view>


namespace mh::core
{
	template <typename Func, typename T, std::size_t Dim>
	struct objective : private Func
	{
		using value_type = T;
		static constexpr std::size_t dimension = Dim;

		std::pair<T, T> bounds{};
		std::string_view name{};

		constexpr objective(std::pair<T, T> bounds, std::string_view name) :
			bounds(bounds),
			name(name)
		{}

		constexpr T operator()(const T* __restrict x) const
		{
			return Func::operator()(x);
		}
	};
}