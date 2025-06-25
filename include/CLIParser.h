#ifndef HAICL_CLI_PARSER_H
#define HAICL_CLI_PARSER_H

#include "CLI11.hpp"
#include <string>
#include <vector>
#include <map>

struct CommandLineArgs {
    bool interactive_mode = false;
    std::string prompt = "";
    std::string model_type = ""; // e.g., "openai", "google"
    std::string model_name = "";
    std::string load_history_file = "";
    std::string save_history_file = "";
    std::vector<std::string> model_params; // Keep as vector<string> for CLI11 parsing
};

class CLIParser {
public:
    CLIParser(int argc, char** argv);

    // Parses the command line arguments.
    // Returns true if parsing was successful, false otherwise (e.g., help requested, parsing error).
    bool parse();

    // Returns the parsed command line arguments.
    const CommandLineArgs& getArgs() const;

private:
    CLI::App app_;
    int argc_;
    char** argv_;
    CommandLineArgs args_;
};

#endif // HAICL_CLI_PARSER_H


