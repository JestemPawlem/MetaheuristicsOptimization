#pragma once

#include <chrono>
#include <cstddef>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>

#include <mh/utils/path_utils.h>


namespace mh::utils
{
	class csv_writer
	{
	public:

		explicit csv_writer(
			const std::string& base_filename,
			std::span<const std::string_view> headers,
			std::span<const std::string_view> meta_headers,
			std::span<const std::string> meta_values)
		{
			auto now = std::chrono::floor<std::chrono::seconds>(
				std::chrono::system_clock::now());
			std::string filename = std::format(
				"{}_{:%Y%m%d_%H%M%S}.csv", base_filename, now);

			std::filesystem::path project_root = get_project_root();
			std::filesystem::path output_dir = project_root / "results";

			if (!std::filesystem::exists(output_dir))
			{
				std::filesystem::create_directories(output_dir);
			}

			std::filesystem::path full_path = output_dir / filename;

			file_.open(full_path, std::ios::trunc);
			if (!file_.is_open())
				throw std::runtime_error("Failed to open CSV file: " + full_path.string());

			for (std::size_t i{}; i < meta_headers.size(); ++i)
			{
				file_ << meta_headers[i];
				if (i + 1 < meta_headers.size())
					file_ << ',';
			}
			file_ << '\n';

			for (std::size_t i{}; i < meta_values.size(); ++i)
			{
				file_ << meta_values[i];
				if (i + 1 < meta_values.size())
					file_ << ',';
			}
			file_ << '\n';

			for (std::size_t i{}; i < headers.size(); ++i)
			{
				file_ << headers[i];
				if (i + 1 < headers.size())
					file_ << ',';
			}
			file_ << '\n';

			file_.flush();
		}

		void write_row(std::span<const std::string> values)
		{
			for (std::size_t i{}; i < values.size(); ++i)
			{
				file_ << values[i];
				if (i + 1 < values.size())
					file_ << ',';
			}
			file_ << '\n';
			file_.flush();
		}


	private:
		std::ofstream file_;
	};
}