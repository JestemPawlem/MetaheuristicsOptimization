#pragma once

#include <string>
#include <string_view>

namespace mh::core
{
	struct field_t
	{
		std::string_view name;
		std::string value;
	};
}