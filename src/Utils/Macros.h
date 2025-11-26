#pragma once

#include "Config/Config.h"
#define SUB_GET_VERSION() \
	[] () -> std::string { \
		auto cfg = sub::config::Configuration::Get(); \
		if (cfg.spoofed_version.has_value()) \
		{ \
			return cfg.spoofed_version.value(); \
		} \
		return SUB_VERSION; \
	}()

#ifdef _WIN32
#define WINDOWS_CODE_BLOCK(expr) expr
#define LINUX_CODE_BLOCK(expr)
#else
#define WINDOWS_CODE_BLOCK(expr)
#define LINUX_CODE_BLOCK(expr) expr
#endif