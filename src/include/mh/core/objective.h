#pragma once

#include <array>
#include <utility>
#include <string_view>


namespace mh::core
{
	template <typename Func, typename T, size_t DIM>
	struct objective : private Func
	{
		using value_type = T;
		static constexpr std::size_t dimension = DIM;

		std::pair<T, T> bounds{};
		std::string name{};

		constexpr objective(std::pair<T, T> bounds, std::string_view name) :
			bounds(bounds),
			name(name)
		{}

		constexpr T operator()(const std::array<T, DIM>& x) const
		{
			return Func::operator()(x);
		}
	};
}