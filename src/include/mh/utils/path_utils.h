#pragma once

#include <filesystem>


namespace mh::utils
{
	inline std::filesystem::path get_project_root()
	{
		std::filesystem::path path = std::filesystem::current_path();

		while (path.has_parent_path() && !std::filesystem::exists(path / "src"))
			path = path.parent_path();

		if (!std::filesystem::exists(path / "src"))
			return std::filesystem::current_path();

		return path;
	}
}