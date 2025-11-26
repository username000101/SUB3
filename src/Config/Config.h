#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <glaze/glaze.hpp>
#include <subprocess.hpp>

namespace sub::config
{

struct ModuleLoadingInfo
{
	std::filesystem::path file;
	std::optional<std::string> prefix;
};

struct SUBConfig
{
	std::unordered_map<std::string, std::string> variables;
	std::vector<ModuleLoadingInfo> modules;
	std::vector<std::string> blocked_requests;
};

struct SUBConfiguration
{
	std::optional<std::string> spoofed_version;
	std::shared_ptr<std::vector<std::unique_ptr<subprocess::Popen>>> processes;
	std::optional<SUBConfig> loadable_config;
};

class Configuration
{
	static inline SUBConfiguration config;
public:
	enum WriteOptions
	{
		KEEP_EXISING = 0,
		OVERWRITE_EXISING = 1,
	};

	[[nodiscard]] static SUBConfiguration Get() { return config; }
	static bool Load(const std::filesystem::path&);
	static void AddVariable(const std::string&, const std::string&, const WriteOptions&);
	static void RemoveVariable(const std::string&);
	static std::optional<std::pair<std::string, std::string>> GetVariable(const std::string&);
	static void KillProcesses();
	static void RunProcesses();
};

} // namespace sub::config

template <>
struct glz::meta<sub::config::SUBConfig>
{
	using T = sub::config::SUBConfig;

	static constexpr auto api_id_read = [](T& self, double api_id)
	{
		self.variables["api_id"] = std::to_string(api_id);
	};

	static constexpr auto api_id_write = [](T& self) -> std::string
	{
		if (self.variables.contains("api_id"))
		{
			auto api_id = self.variables.at("api_id");
			self.variables.erase("api_id");
			return api_id;
		}
		return "";
	};

	static constexpr auto api_hash_read = [](T& self, const std::string& api_hash)
	{
		self.variables["api_hash"] = api_hash;
	};

	static constexpr auto api_hash_write = [] (T& self) -> std::string
	{
		if (self.variables.contains("api_hash"))
		{
			auto api_hash = self.variables.at("api_hash");
			self.variables.erase("api_hash");
			return api_hash;
		}
		return "";
	};

	static constexpr auto value = object(
		"api_id", custom<api_id_read, api_id_write>,
		"api_hash", custom<api_hash_read, api_hash_write>,
		"variables", &T::variables,
		"blocked_requests", &T::blocked_requests,
		"modules", &T::modules);
};