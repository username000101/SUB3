#include "Config/Config.h"
#include "Td.h"

std::expected<std::string, sub::Error> sub::td::TdWrapper::ExecuteRequest(const std::string& request) {
	if (!td_client) {
		LOGE << "Failed to execute request '" << request << "': this->TdClient == nullptr" << std::endl;
	}

	glz::generic requestj;
	if (auto write_result = glz::read_json<glz::generic>(request); !write_result.has_value()) {
		LOGE << "Failed to execute request '" << request << "': " << glz::format_error(write_result, request) << std::endl;
		return std::unexpected(Error("GLAZE_ERROR: " + glz::format_error(write_result, request)));
	} else {
		requestj = write_result.value();
	}

	if (!requestj.contains("@type")) {
		LOGE << "Failed to execute request '" << request << "': not found '@type' field" << std::endl;
		return std::unexpected(Error("INCORRECT_JSON: not found '@type' field"));
	}

	if (auto cfg = config::Configuration::Get(); cfg.loadable_config.has_value() && std::ranges::find(cfg.loadable_config->blocked_requests, requestj.at("@type").as<std::string>()) != cfg.loadable_config->blocked_requests.end()) {
		LOGE << "Failed to execute request '" << request << "': Request is blocked" << std::endl;
		return std::unexpected(Error("SECURITY_ERROR: Request is blocked"));
	}

	if (!config::Configuration::Get().loadable_config.has_value()) {
		LOGW << "Configuration is not loaded: all requests are allowed" << std::endl;
	}

	const std::lock_guard lock(td_client_lock);
	auto response = td_json_client_execute(td_client, request.c_str());
	return response;
}
