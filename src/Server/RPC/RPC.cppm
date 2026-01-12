module;

#include <cstdint>
#include <thread>

#include <plog/Log.h>
#include <rpc/rpc_error.h>
#include <rpc/server.h>

#include "Td/Td.h"
#include "Utils/Macros.h"

export module sub3_rpc;

void rpc_server_loop(std::uint16_t port) {
	LOGI << "Starting RPC server on port " << port << std::endl;
	try {
		rpc::server server("127.0.0.1", port);
		server.bind("sub.ping", [] { return "PONG"; });
		server.bind("sub.echo", [](const std::string& msg) { return msg; });
		server.bind("sub.version", [] { return SUB_GET_VERSION(); });

		server.bind("sub.getvar", [](const std::string& name) -> std::string {
			if (auto variable = sub::config::Configuration::GetVariable(name); variable.has_value()) {
				return variable.value().second;
			}
			return "";
		});

		server.bind("sub.addvar", [](const std::string& name, const std::string& value, bool overwrite) {
			if (auto variable = sub::config::Configuration::GetVariable(name); variable.has_value() && overwrite) {
				sub::config::Configuration::AddVariable(name, value, sub::config::Configuration::WriteOptions::OVERWRITE_EXISING);
				return "";
			}
			return "ERROR_VARIABLE_ALREADY_EXISTS";
		});

		server.bind("sub.rmvar", [](const std::string& name) {
			if (auto variable = sub::config::Configuration::GetVariable(name); variable.has_value()) {
				LOGW << "Deleting variable '" << name << "'" << std::endl;
				sub::config::Configuration::RemoveVariable(name);
			}
		});

		server.bind("sub.subscribe", [](const std::string& update, const std::string& callback_address, const std::uint16_t& callback_port) {
			sub::td::TdWrapper::AddSubscriber(sub::td::TdWrapper::SubscriberInfo(update, callback_address, callback_port));
		});

		server.bind("sub.unsubscribe", [](const std::string& update, const std::string& callback_address, const std::uint16_t& callback_port) {
			sub::td::TdWrapper::RemoveSubscriber(sub::td::TdWrapper::SubscriberInfo(update, callback_address, callback_port));
		});

		server.bind("sub.send", [](const std::string& request) {
			auto response = sub::td::TdWrapper::SendBlockingRequest(request);
			if (response.has_value()) {
				return response.value();
			}
			return sub::format_error(response.error());
		});

		server.bind("sub.execute", [](const std::string& request) {
			auto response = sub::td::TdWrapper::ExecuteRequest(request);
			if (response.has_value()) {
				return response.value();
			}
			return sub::format_error(response.error());
		});

		server.run();

	} catch (rpc::system_error& rpcerr) {
		LOGE << "Error in RPC server thread(rpc::system_error): " << rpcerr.what() << std::endl;
		std::terminate(); /* TODO: CALL SAFE EXIT FUNCTION */
	} catch (std::exception& err) {
		LOGE << "Error in RPC server thread(std::exception): " << err.what() << std::endl;
		std::terminate(); /* TODO: CALL SAFE EXIT FUNCTION */
	}
}

export namespace sub::server::rpc {
	void start_rpc_server(std::uint16_t port) {
		std::thread server_thread([port] { rpc_server_loop(port); });
		server_thread.detach();
	}
} // namespace sub::server::rpc