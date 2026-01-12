module;

#include <optional>
#include <string>

#include "Td/Types.h"

export module sub3_error;

export namespace sub {
	struct Error {
		std::string message;
		std::optional<td::types::error> td_error;
	};

	std::string format_error(const Error& err) {
		std::string message = err.message;
		if (err.td_error.has_value()) {
			message += ": ";
			message += err.td_error.value().message;
		}
		return message;
	}
} // namespace sub