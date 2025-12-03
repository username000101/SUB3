#include "CLI/Cli.h"
#include "Logger/Logger.h"
#include "Safety/Safety.h"

int main(int argc, char** argv) {
	sub::logger::InitializeLogger();
	sub::safety::Safety::EnableSafety(nullptr);
	sub::cli::parse_cli_args(argc, argv);
}