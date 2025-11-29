#include <plog/Log.h>

#include "Config.h"

std::optional<std::pair<std::string, std::string>> sub::config::Configuration::GetVariable(const std::string& name) {
	if (!config.loadable_config.has_value()) {
		LOGW << "Trying to add variable '" << name << "' without an active configuration" << std::endl;
	}
}