#include <algorithm>
#include <ranges>

#include <glaze/glaze.hpp>

#include "Config/Config.h"
#include "Td.h"

#define CONTINUE                                                 \
	std::this_thread::sleep_for(std::chrono::milliseconds(150)); \
	continue

std::expected<std::string, sub::Error> sub::td::TdWrapper::SendBlockingRequest(const std::string& request) {
	if (!td_client) {
		LOGE << "Failed to send request '" << request << "': this->TdClient == nullptr" << std::endl;
	}

	std::string extra_field_data;
	glz::generic requestj;
	if (auto write_result = glz::read_json<glz::generic>(request); !write_result.has_value()) {
		LOGE << "Failed to send request '" << request << "': " << glz::format_error(write_result, request) << std::endl;
		return std::unexpected(Error("GLAZE_ERROR: " + glz::format_error(write_result, request)));
	} else {
		requestj = write_result.value();
	}

	if (!requestj.contains("@type")) {
		LOGE << "Failed to send request '" << request << "': not found '@type' field" << std::endl;
		return std::unexpected(Error("INCORRECT_JSON: not found '@type' field"));
	}

	if (auto cfg = config::Configuration::Get(); cfg.loadable_config.has_value() && std::ranges::find(cfg.loadable_config->blocked_requests, requestj.at("@type").as<std::string>()) != cfg.loadable_config->blocked_requests.end()) {
		LOGE << "Failed to send request '" << request << "': Request is blocked" << std::endl;
		return std::unexpected(Error("SECURITY_ERROR: Request is blocked"));
	}

	if (!config::Configuration::Get().loadable_config.has_value()) {
		LOGW << "Configuration is not loaded: all requests are allowed" << std::endl;
	}

	if (!requestj.contains("@extra")) {
		LOGE << "Failed to send request '" << request << "': not found '@extra' field" << std::endl;
		return std::unexpected(Error("INCORRECT_JSON: not found '@extra' field"));
	}
	extra_field_data = requestj.at("@extra").as<std::string>();

	{
		const std::lock_guard lock(requests_lock);
		pending_requests.push_back(request);
	}

	while (true) {
		if (responses.empty()) {
			CONTINUE;
		}

		std::string response;
		glz::generic responsej;
		{
			const std::lock_guard lock(responses_lock);
			response = responses.front();
			if (auto write_result = glz::read_json<glz::generic>(response); !write_result.has_value()) {
				LOGE << "Failed to read response '" << response << "': " << glz::format_error(write_result, response) << std::endl;
				responses.push_front(response);
				CONTINUE;
			} else {
				responsej = write_result.value();
			}

			LOGD << "Trying to get @extra field of '" << responsej.dump().value() << "'" << std::endl;
			if (responsej.at("@extra").as<std::string>() == extra_field_data) {
				return response;
			}
		}
	}
}
