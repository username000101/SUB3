#include <plog/Log.h>

#include "Config.h"

std::optional<std::pair<std::string, std::string>> sub::config::Configuration::GetVariable(const std::string& name) {
	if (!config.loadable_config.has_value()) {
		LOGW << "Trying to get variable '" << name << "' without an active configuration" << std::endl;
		return std::nullopt;
	}

	if (!config.loadable_config.value().variables.contains(name)) return std::nullopt;

	return std::make_pair(name, config.loadable_config.value().variables.at(name));
}