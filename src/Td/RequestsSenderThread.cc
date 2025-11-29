#include <chrono>

#include <glaze/glaze.hpp>

#include "Td.h"

#define CONTINUE                                                 \
	std::this_thread::sleep_for(std::chrono::milliseconds(150)); \
	continue

void sub::td::TdWrapper::RequestsSenderThread() {
	while (true) {
		if (!td_client) {
			LOGF << "Failed to process RequestsSender thread work: this->TdClient == nullptr" << std::endl;
			std::terminate(); /* TODO: CALL SAFE EXIT */
		}

		if (pending_requests.empty()) {
			CONTINUE;
		}

		std::string request;
		{
			const std::lock_guard lock(requests_lock);
			request = std::move(pending_requests.front());
			pending_requests.pop_front();
		}

		glz::generic jreq;
		if (auto write_result = glz::read_json<glz::generic>(request); !write_result.has_value()) {
			LOGE << "Failed to send request '" << request << "': " << glz::format_error(write_result, request) << std::endl;
			CONTINUE;
		} else {
			jreq = write_result.value();
		}

		if (!jreq.contains("@extra")) {
			LOGE << "Failed to send request '" << request << "': not found '@extra' field" << std::endl;
			CONTINUE;
		}

		LOGD << "Sending request '" << request << "'" << std::endl;
		{
			const std::lock_guard lock(td_client_lock);
			td_json_client_send(td_client, request.c_str());
		}
	}
}
