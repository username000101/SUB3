#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef __linux__
#include <sys/signal.h>
#endif

namespace sub::safety {
#ifdef _WIN32
	class Safety {
		static LONG WINAPI DefaultHandler(EXCEPTION_POINTERS*);

	  public:
		using HFnPtrT = LONG (*)(EXCEPTION_POINTERS*);

		static void Shutdown(int code, const char* = nullptr);
		static void EnableSafety(HFnPtrT);
	};
#endif

#ifdef __linux__
	class Safety {
		static void DefaultHandler(int, siginfo_t*, void*);

	  public:
		using HFnPtrT = void (*)(int, siginfo_t*, void*);

		static void Shutdown(int code, const char* = nullptr);
		static void EnableSafety(HFnPtrT);
	};
#endif

} // namespace sub::safety