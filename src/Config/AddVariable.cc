#include "Config.h"

#include <plog/Log.h>

void sub::config::Configuration::AddVariable(const std::string& name, const std::string& value, const WriteOptions& wropts) {
	if (!config.loadable_config.has_value()) {
		LOGW << "Trying to add variable '" << name << "' without an active configuration" << std::endl;
		return;
	}

	if (auto variable = GetVariable(name); variable.has_value() && wropts != OVERWRITE_EXISING) {
		LOGW << "Trying to add existing variable '" << name << "': '" << variable.value().second << "' ==> '" << value << "' but overwrite flag is not set" << std::endl;
		return;
	} else {
		LOGW << "Adding/Overwriting variable '" << name << "': '" << variable.value().second << "' ==> '" << value << "'" << std::endl;
		config.loadable_config.value().variables[name] = value;
	}
}
