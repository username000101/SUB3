#include "CLI/Cli.h"
#include "Safety/Safety.h"

int main(int argc, char** argv) {
	sub::safety::Safety::EnableSafety(nullptr);
	sub::cli::parse_cli_args(argc, argv);
}