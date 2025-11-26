#include "Loop.h"

#include "Config/Config.h"
#include "Server/RPC/RPC.h"
#include "Logger/Logger.h"
#include "Td/Td.h"

void sub::main::sub_main()
{
	logger::InitializeLogger(); // Initialize logging

	server::rpc::start_rpc_server(5000); // Start RPC server

	td::TdWrapper::CreateInstance(); // Create td json client
	td::TdWrapper::StartRequestsSenderThread(); // Start requests sender thread
	td::TdWrapper::StartUpdatesBroadcasterThread(); // Start updates broadcaster thread
	td::TdWrapper::ExecuteRequest(R"({ "@type": "setLogVerbosityLevel", "new_verbosity_level": 1 })");

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
