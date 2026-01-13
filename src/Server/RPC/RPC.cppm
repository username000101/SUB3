module;

#include <cstdint>

export module sub3_rpc;

export import :start_rpc_server.impl;

export namespace sub::server::rpc {
	void start_rpc_server(std::uint16_t);
} // namespace sub::server::rpc