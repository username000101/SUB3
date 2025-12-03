#ifdef _WIN32
#include <algorithm>
#include <csignal>
#include <ranges>
#include <thread>

#include <plog/Log.h>

#include "Safety.h"

inline const char* sigstr(DWORD signal) {
	switch (signal) {
	case EXCEPTION_ACCESS_VIOLATION:
		return "EXCEPTION_ACCESS_VIOLATION";
	case EXCEPTION_STACK_OVERFLOW:
		return "EXCEPTION_STACK_OVERFLOW";
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		return "EXCEPTION_INT_DIVIDE_BY_ZERO";
	case EXCEPTION_INT_OVERFLOW:
		return "EXCEPTION_INT_OVERFLOW";
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		return "EXCEPTION_ILLEGAL_INSTRUCTION";
	default:
		return "UNKNOWN";
	}
}

volatile sig_atomic_t __error_occurred = 0;
volatile sig_atomic_t __signal = 0;
volatile sig_atomic_t __instruction_pointer = 0;
void error_check_thread() {
	LOGI << "Error check thread started" << std::endl;
	while (!__error_occurred) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }

	LOGF << "AN ERROR HAS BEEN OCCURRED: " << sigstr(__signal) << " at " << std::hex << __instruction_pointer << std::endl;
	sub::safety::Safety::Shutdown(EXIT_FAILURE, std::string(sigstr(__signal)).c_str());
}

LONG WINAPI sub::safety::Safety::DefaultHandler(EXCEPTION_POINTERS* eptrs) {
	constexpr DWORD panic_codes[] = {
		EXCEPTION_ACCESS_VIOLATION,
		EXCEPTION_STACK_OVERFLOW,
		EXCEPTION_STACK_INVALID,
		EXCEPTION_ILLEGAL_INSTRUCTION,
		EXCEPTION_INT_OVERFLOW,
		EXCEPTION_INT_DIVIDE_BY_ZERO,
		EXCEPTION_FLT_OVERFLOW,
		EXCEPTION_FLT_DIVIDE_BY_ZERO,
	};

	if (std::ranges::any_of(panic_codes, [eptrs](auto& pcode) -> bool { return pcode == eptrs->ExceptionRecord->ExceptionCode; })) {
		__error_occurred = 1;
		__signal = static_cast<int>(eptrs->ExceptionRecord->ExceptionCode);
		__instruction_pointer = static_cast<int>(eptrs->ContextRecord->Rip);
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

void sub::safety::Safety::EnableSafety(sub::safety::Safety::HFnPtrT OverrideDefaultHandler) {
	std::thread err_t([] { error_check_thread(); });
	err_t.detach();

	if (OverrideDefaultHandler) {
		AddVectoredExceptionHandler(0, OverrideDefaultHandler);
	} else {
		AddVectoredExceptionHandler(0, DefaultHandler);
	}
}
#endif