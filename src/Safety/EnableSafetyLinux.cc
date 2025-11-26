#include "Safety.h"

#ifdef __linux__
#include <csignal>
#endif

#include <thread>

#include <plog/Log.h>

LINUX_CODE_BLOCK(

inline const char* sigstr(int signal)
{
	switch (signal)
	{
	case SIGINT: return "SIGINT";
	case SIGILL: return "SIGILL";
	case SIGSEGV: return "SIGSEGV";
	case SIGBUS: return "SIGBUS";
	case SIGABRT: return "SIGABRT";
	case SIGTERM: return "SIGTERM";
	case SIGKILL: return "SIGKILL";
	case SIGTRAP: return "SIGTRAP";
	default: return "UNKNOWN";
	}
}

volatile sig_atomic_t __error_occurred = 0;
volatile sig_atomic_t __signal = 0;
volatile sig_atomic_t __instruction_pointer = 0;

void error_check_thread()
{
	LOGI << "Error check thread started" << std::endl;
	while (!__error_occurred) { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }

	LOGF << "AN ERROR HAS BEEN OCCURRED: " << sigstr(__signal) << " at " << std::hex << __instruction_pointer << std::endl;
	sub::safety::Safety::Shutdown(EXIT_FAILURE, sigstr(__signal));
}

void sub::safety::Safety::DefaultHandler(int signal, siginfo_t* siginfo, void* ctx)
{
	__error_occurred = 1;
	__signal = signal;
	__instruction_pointer = static_cast<ucontext_t*>(ctx)->uc_mcontext.gregs[REG_RIP];
}

void sub::safety::Safety::EnableSafety(HFnPtrT override_handler)
{
	std::thread err_t([] { error_check_thread(); });
	err_t.detach();

	struct sigaction sa;
	sigemptyset(&sa.sa_mask);

	if (override_handler)
	{
		sa.sa_sigaction = override_handler;
	}
	else
	{
		sa.sa_sigaction = DefaultHandler;
	}

	sigaction(SIGINT, &sa, nullptr);
	sigaction(SIGTERM, &sa, nullptr);
	sigaction(SIGSEGV, &sa, nullptr);
	sigaction(SIGINT, &sa, nullptr);
}
)