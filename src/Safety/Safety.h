#pragma once

#include "Utils/Macros.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace sub::safety
{
WINDOWS_CODE_BLOCK(
class Safety
{
	static LONG WINAPI DefaultHandler(EXCEPTION_POINTERS*);
public:
	using HFnPtrT = LONG(*)(EXCEPTION_POINTERS*);

	static void Shutdown(int code, const char* = nullptr);
	static void EnableSafety(HFnPtrT);
};
)

LINUX_CODE_BLOCK(
class Safety
{
	static void DefaultHandler(int, siginfo_t*, void*);
public:
	using HFnPtrT = void(*)(int, siginfo_t*, void*);\

	static void Shutdown(int code, const char* = nullptr);
	static void EnableSafety(HFnPtrT);
};
)

}