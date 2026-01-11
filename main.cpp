#include "Safety/Safety.h"

import sub3_cli;

int main(int argc, char** argv) {
	sub::safety::Safety::EnableSafety(nullptr);
	sub::cli::parse_cli_args(argc, argv);
}