#include "Config.h"

#include <plog/Log.h>
#include <subprocess.hpp>

void sub::config::Configuration::RunProcesses()
{
	if (!config.loadable_config.has_value())
	{
		LOGE << "Failed to run processes: loadable config is not loaded(please call sub::config::Configuration::Load() first or check previous logs)" << std::endl;
		return;
	}

	for (auto& module : config.loadable_config->modules)
	{
		if (!std::filesystem::exists(module.file))
		{
			LOGE << "Failed to run module '" << module.file.string() << "': file doesn't exist" << std::endl;
			continue;
		}

		if (module.prefix.has_value())
		{
			LOGD << "Running module '" << module.file.string() << "' with these parameters(prefix): '" << module.prefix.value() << "'" << std::endl;
			config.processes->emplace_back(std::make_unique<subprocess::Popen>(subprocess::RunBuilder({module.prefix.value(), module.file.string()}).popen()));
		}
		else
		{
			LOGD << "Running module '" << module.file.string() << "' without parameters(prefix)" << std::endl;
			config.processes->emplace_back(std::make_unique<subprocess::Popen>(subprocess::RunBuilder({module.file.string()}).popen()));
		}
	}
}
