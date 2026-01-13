module;
export module sub3_cli;

export import :parse_cli_args.impl;

export namespace sub::cli { void parse_cli_args(int, char**); }