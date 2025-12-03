#include <glaze/glaze.hpp>
#include <rpc/client.h>
#include <rpc/rpc_error.h>

#include "Td.h"

#define CONTINUE                                                 \
	std::this_thread::sleep_for(std::chrono::milliseconds(150)); \
	continue

void sub::td::TdWrapper::UpdatesBroadcastTread() {
	while (true) {
		if (!td_client) {
			LOGF << "Failed to process RequestsSender thread work: this->TdClient == nullptr" << std::endl;
			std::terminate(); /* TODO: CALL SAFE EXIT */
		}

		std::string update;
		const char* update_raw;
		{
			const std::lock_guard lock(td_client_lock);
			update_raw = td_json_client_receive(td_client, SUB_TDLIB_RECEIVE_TIMEOUT);
		}
		if (!update_raw) {
			CONTINUE;
		}

		LOGD << "Raw update: " << update_raw << std::endl;
		update = update_raw;
		glz::generic updatej;
		if (auto write_result = glz::read_json<glz::generic>(update); !write_result.has_value()) {
			LOGE << "Failed to broadcast '" << update << "': " << glz::format_error(write_result, update) << std::endl;
			CONTINUE;
		} else {
			updatej = write_result.value();
		}

		if (updatej.contains("@extra")) {
			LOGD << "That's response: " << update << std::endl;
			const std::lock_guard lock(responses_lock);
			responses.push_back(update);
		} else {
			std::vector<SubscriberInfo> SubscribersCopy;
			{
				const std::lock_guard lock(subscribers_lock);
				SubscribersCopy = std::vector(subscribers);
			}
			for (auto& subscriber: SubscribersCopy) {
				if (subscriber.update_type == updatej.at("@type").as<std::string>()) {
					try {
						LOGD << "Broadcasting update '" << updatej.dump().value() << "' to " << subscriber.update_type << "@" << subscriber.callback_notify_address << ":" << subscriber.callback_notify_port << std::endl;
						rpc::client broadcaster(subscriber.callback_notify_address, subscriber.callback_notify_port);
						broadcaster.send(updatej.at("@type").as<std::string>(), update);
					} catch (rpc::system_error& rpcerr) {
						LOGE << "Failed to broadcast '" << update << "' to " << subscriber.callback_notify_address << ":" << subscriber.callback_notify_port << ": " << rpcerr.what();
					} catch (rpc::rpc_error& rpcerr) {
						LOGE << "Failed to broadcast '" << update << "' to " << subscriber.callback_notify_address << ":" << subscriber.callback_notify_port << ": " << rpcerr.get_error().as<std::string>();
					} catch (...) {
						LOGE << "Failed to broadcast UNKNOWN ERR" << std::endl;
					}
				}
			}
		}
	}
}
