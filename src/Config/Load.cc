#include <fstream>

#include <plog/Log.h>

#include "Config.h"

bool sub::config::Configuration::Load(const std::filesystem::path& config_file) {

	if (!std::filesystem::exists(config_file) || !std::filesystem::is_regular_file(config_file)) {
		LOGW << "Config file '" << config_file.string() << "' does not exist" << std::endl;
		return false;
	}

	std::string json;
	{
		std::ifstream ReadStream(config_file);
		if (!ReadStream.is_open())
			return false;

		std::string buffer;
		while (std::getline(ReadStream, buffer))
			json += buffer;
	}

	if (auto write_result = glz::read_json<SUBConfig>(json); !write_result.has_value()) {
		LOGF << "Failed to load config from \"" << config_file.string() << "\": \"" << glz::format_error(write_result.error()) << "\"" << std::endl;
		return false;
	} else {
		LOGI << "Update config from file \"" << config_file.string() << "\"" << std::endl;
		config.loadable_config = write_result.value();
		return true;
	}
}
