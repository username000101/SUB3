#pragma once

#include <optional>
#include <string>

#include "Td/Types.h"

namespace sub {
	struct Error {
		std::string message;
		std::optional<td::types::error> td_error;
	};

	std::string format_error(const Error&);
} // namespace sub