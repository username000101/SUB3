#include <filesystem>
#include <optional>

#include <CLI/CLI.hpp>
#include <plog/Log.h>

#include "Cli.h"
#include "Main/Loop.h"

void sub::cli::parse_cli_args(int argc, char** argv) {
	CLI::App app{"Telegram userbot"};
	argv = app.ensure_utf8(argv);

	std::filesystem::path config;
	std::optional<std::string> spoofed_version;

	auto cfg_opt = app.add_option("-c,--config", config, "Specify the configuration file");
	cfg_opt->required();
	cfg_opt->type_name("FILE");
	cfg_opt->check(CLI::ExistingFile);

	auto spoof_opt = app.add_option("-s,--spoof-version", spoofed_version, "Spoof userbot version");
	spoof_opt->type_name("STRING");

	try {
		if (argc < 2) throw CLI::CallForHelp();
	} catch (const CLI::CallForHelp& neargs_error) {
		app.exit(neargs_error);
		std::exit(EXIT_FAILURE);
	}

	try {
		app.parse(argc, argv);
	} catch (const std::exception& parse_cli_args_err) {
		LOGE << "Failed to parse cli args: " << parse_cli_args_err.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}
	main::sub_main(config, spoofed_version);
}
