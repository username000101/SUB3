#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace sub::main {
	[[noreturn]] void sub_main(const std::filesystem::path&, std::optional<std::string>);
}