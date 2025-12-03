#include <plog/Log.h>
#include <subprocess.hpp>

#include "Config.h"

void sub::config::Configuration::RunProcesses() {
	if (!config.loadable_config.has_value()) {
		LOGE << "Failed to run processes: loadable config is not loaded(please call sub::config::Configuration::Load() first or check previous logs)" << std::endl;
		return;
	}

	if (config.loadable_config->modules.empty()) {
		LOGW << "Modules array is empty" << std::endl;
		return;
	}

	auto processes = std::make_shared<std::vector<std::unique_ptr<subprocess::Popen>>>();

	for (auto& module: config.loadable_config->modules) {
		if (!std::filesystem::exists(module.file)) {
			LOGE << "Failed to run module '" << module.file.string() << "': file doesn't exist" << std::endl;
			continue;
		}

		if (module.prefix.has_value()) {
			LOGD << "Running module '" << module.file.string() << "' with these parameters(prefix): '" << module.prefix.value() << "'" << std::endl;
			auto popen_ptr = std::make_unique<subprocess::Popen>(subprocess::RunBuilder({module.prefix.value(), module.file.string()}).popen());
			if (!popen_ptr) {
				LOGE << "Failed to create popen ptr for module '" << module.file.string() << "'" << std::endl;
				continue;
			}
			processes->emplace_back(std::move(popen_ptr));
		} else {
			LOGD << "Running module '" << module.file.string() << "' without parameters(prefix)" << std::endl;
			processes->emplace_back(std::make_unique<subprocess::Popen>(subprocess::RunBuilder({module.file.string()}).popen()));
		}
	}
	LOGI << "Loaded " << processes->size() << " processes" << std::endl;
	config.processes = std::move(processes);
}
