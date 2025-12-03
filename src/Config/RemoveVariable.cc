#include <plog/Log.h>

#include "Config.h"

void sub::config::Configuration::RemoveVariable(const std::string& name) {
	if (!config.loadable_config.has_value()) {
		LOGW << "Trying to remove variable '" << name << "' without an active configuration" << std::endl;
		return;
	}

	if (!config.loadable_config.value().variables.contains(name)) return;
	config.loadable_config.value().variables.erase(name);
}
