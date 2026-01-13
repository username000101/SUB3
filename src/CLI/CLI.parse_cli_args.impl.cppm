module;

#include <filesystem>
#include <optional>

#include <CLI/CLI.hpp>
#include <plog/Log.h>

#include "Config/Config.h"

import sub3_main;
import sub3_logging;

export module sub3_cli:parse_cli_args.impl;

export namespace sub::cli {
	void parse_cli_args(int argc, char** argv) {
		CLI::App app{"Telegram userbot"};
		argv = app.ensure_utf8(argv);

		std::filesystem::path config;
		std::optional<std::string> spoofed_version;
		bool debug;

		auto cfg_opt = app.add_option("-c,--config", config, "Specify the configuration file");
		cfg_opt->required();
		cfg_opt->type_name("FILE");
		cfg_opt->check(CLI::ExistingFile);

		auto spoof_opt = app.add_option("-s,--spoof-version", spoofed_version, "Spoof userbot version");
		spoof_opt->type_name("STRING");

		auto debug_flag = app.add_flag("-d,--debug", debug, "Enable debug info(config file sections and other)");
		debug_flag->needs(cfg_opt);

		try {
			if (argc < 2) throw CLI::CallForHelp();
		} catch (const CLI::CallForHelp& neargs_error) {
			app.exit(neargs_error);
			std::exit(EXIT_FAILURE);
		}

		try {
			logger::InitializeLogger();
			app.parse(argc, argv);
		} catch (const std::exception& parse_cli_args_err) {
			LOGE << "Failed to parse cli args: " << parse_cli_args_err.what() << std::endl;
			std::exit(EXIT_FAILURE);
		}

		if (debug) {
			if (!config::Configuration::Load(config)) {
				std::print(std::cerr, "Failed to load config file \"{}\"\n", config.string());
				std::exit(EXIT_FAILURE);
			}

			if (auto cfg = config::Configuration::Get(); !cfg.loadable_config.has_value()) {
				std::print(std::cerr, "cfg::loadable_config is nullopt\n");
				std::exit(EXIT_FAILURE);
			} else {
				if (cfg.loadable_config->blocked_requests.empty()) std::print(std::cout, "cfg::loadable_config::blocked_requests: empty\n");
				else {
					std::print(std::cout, "cfg::loadable_config::blocked_requests: {{\n");
					for (auto& brequest: cfg.loadable_config->blocked_requests) {
						std::print(std::cout, "\t'{}'\n", brequest);
					}
					std::print(std::cout, "}}\n");
				}

				if (cfg.loadable_config->modules.empty()) std::print(std::cout, "cfg::loadable_config::modules: empty\n");
				else {
					std::print(std::cout, "cfg::loadable_config::modules: {{\n");
					for (auto& mlinfo: cfg.loadable_config->modules) {
						std::print(std::cout, "\t{{'{}', '{}'}}\n", mlinfo.prefix ? mlinfo.prefix.value() : "", mlinfo.file.string());
					}
					std::print(std::cout, "}}\n");
				}

				if (cfg.loadable_config->variables.empty()) std::print(std::cout, "cfg::loadable_config::variables: empty\n");
				else {
					std::print(std::cout, "cfg::loadable_config::variables: {{\n");
					for (auto& var: cfg.loadable_config->variables) {
						std::print(std::cout, "\t{{'{}', '{}'}}\n", var.first, var.second);
					}
					std::print(std::cout, "}}\n");
				}
			}
			std::exit(EXIT_SUCCESS);
		}

		main::sub_main(config, spoofed_version);
	}
} // namespace sub::cli