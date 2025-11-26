#pragma once

#include <expected>
#include <deque>
#include <format>
#include <optional>
#include <thread>
#include <vector>

#include <fair_mutex.hpp>
#include <plog/Log.h>
#include <td/telegram/td_json_client.h>

#include "Error/Error.h"

namespace sub::td
{
class TdWrapper
{
  public:
	struct SubscriberInfo
	{
		std::string update_type;
		std::string callback_notify_address;
		std::uint16_t callback_notify_port;

		bool operator==(const SubscriberInfo& other) const
		{
			return this->update_type == other.update_type && this->callback_notify_address == other.callback_notify_address && this->callback_notify_port == other.callback_notify_port;
		}
	};

  private:
	static inline yamc::fair::mutex td_client_lock, requests_lock, responses_lock, subscribers_lock;
	static inline void* td_client;
	static inline std::deque<std::string> pending_requests, responses;
	static inline std::vector<SubscriberInfo> subscribers;

	[[noreturn]] static void UpdatesBroadcastTread();
	[[noreturn]] static void RequestsSenderThread();

  public:
	static void CreateInstance()
	{
		td_client = td_json_client_create();
	}

	static void DestroyInstance()
	{
		if (td_client)
		{
			const std::lock_guard lock(td_client_lock);
			td_json_client_destroy(td_client);
		}
	}

	static void AddSubscriber(const SubscriberInfo& subscriber)
	{
		LOGD << "Adding subscriber for type '" << subscriber.update_type << "'"
		<< " by socket '" << subscriber.callback_notify_address << ":" << subscriber.callback_notify_port << "'" << std::endl;
		const std::lock_guard lock(subscribers_lock);
		subscribers.push_back(subscriber);
	}

	static void RemoveSubscriber(const SubscriberInfo& subscriber)
	{
		for (auto it = subscribers.begin(); it != subscribers.end(); ++it)
		{
			if (*it == subscriber)
			{
				LOGD << "Removing subscriber for type '" << subscriber.update_type << "'"
				<< " by socket '" << subscriber.callback_notify_address << ":" << subscriber.callback_notify_port << "'" << std::endl;
				const std::lock_guard lock(subscribers_lock);
				subscribers.erase(it);
				break;
			}
		}
	}

	static void SendRequest(const std::string&);
	static std::expected<std::string, Error> SendBlockingRequest(const std::string&);
	static std::expected<std::string, Error> ExecuteRequest(const std::string&);

	static void StartUpdatesBroadcasterThread()
	{
		std::thread updates_broadcast_t([] { UpdatesBroadcastTread(); });
		updates_broadcast_t.detach();
	}

	static void StartRequestsSenderThread()
	{
		std::thread sender_thread([] { RequestsSenderThread(); });
		sender_thread.detach();
	}
};
} // namespace sub::td