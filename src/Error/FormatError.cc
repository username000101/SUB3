#include "Error/Error.h"

std::string sub::format_error(const Error& err) {
	std::string message;
	message = err.message;
	if (err.td_error.has_value()) {
		message += ": ";
		message += err.td_error.value().message;
	}
	return message;
}
