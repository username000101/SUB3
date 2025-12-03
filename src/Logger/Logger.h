#pragma once

#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>
#include <plog/Log.h>

namespace sub::logger {
	inline void InitializeLogger() {
		static plog::ColorConsoleAppender<plog::TxtFormatter> ColorConsoleAppender;
#ifndef NDEBUG
		plog::init(plog::debug, &ColorConsoleAppender);
#else
		plog::init(plog::info, &ColorConsoleAppender);
#endif
		LOGI << "Logger initialized" << std::endl;
	}
} // namespace sub::logger