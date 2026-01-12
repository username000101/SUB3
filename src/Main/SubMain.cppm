module;

#include <filesystem>
#include <optional>
#include <string>

#include "Config/Config.h"
#include "Safety/Safety.h"
#include "Td/Td.h"
#include "Utils/Macros.h"

import sub3_rpc;
import sub3_logging;

export module sub3_main;

export namespace sub::main {
	[[noreturn]] void sub_main(const std::filesystem::path& cfgfile, std::optional<std::string> spoofed_version) {
		if (!config::Configuration::Load(cfgfile)) {
			LOGF << "Failed to init SUB v" << SUB_GET_VERSION() << std::endl;
			safety::Safety::Shutdown(EXIT_FAILURE, "Check previous logs");
		}
		auto& cfg = config::Configuration::Get();
		cfg.spoofed_version = spoofed_version;

		LOGI << "Starting SUB3 v" << SUB_GET_VERSION() << std::endl;

		server::rpc::start_rpc_server(5000); // Start RPC server

		td::TdWrapper::CreateInstance();				// Create td json client
		td::TdWrapper::StartRequestsSenderThread();		// Start requests sender thread
		td::TdWrapper::ExecuteRequest(R"({ "@type": "setLogVerbosityLevel", "new_verbosity_level": 1 })");
		td::TdWrapper::StartUpdatesBroadcasterThread(); // Start updates broadcaster thread

		config::Configuration::RunProcesses();

		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}