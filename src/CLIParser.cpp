
#include "CLIParser.h"
#include <iostream>

CLIParser::CLIParser(int argc, char** argv)
    : app_("HAICL - Hitmux AI in Command Line", "haicl"), argc_(argc), argv_(argv) {
    
    app_.set_version_flag("--version", "0.1.0");

    // Interactive mode option
    app_.add_flag("-i,--interactive", args_.interactive_mode, "Enter interactive chat mode.");

    // Prompt for quick question mode
    app_.add_option("-p,--prompt", args_.prompt, "Quick question to the AI. If provided, interactive mode is skipped.");

    // Model type (e.g., openai, google)
    app_.add_option("-t,--type", args_.model_type, "Specify AI model type (e.g., openai, google). Overrides config.");

    // Model name (e.g., gpt-4, gemini-pro)
    app_.add_option("-m,--model", args_.model_name, "Specify AI model name (e.g., gpt-4, gemini-pro). Overrides config.");

    // Load history file
    app_.add_option("--load-history", args_.load_history_file, "Load conversation from a history file to continue.");

    // Save history file (manual save)
    app_.add_option("--save-history", args_.save_history_file, "Manually save current conversation to a specified file.");

    // Model parameters (e.g., --param temperature=0.7 --param max_tokens=100)
    app_.add_option("--param", args_.model_params, "Pass model-specific parameters (e.g., --param temperature=0.7).");
}

bool CLIParser::parse() {
    try {
        app_.parse(argc_, argv_);
        return true;
    } catch (const CLI::ParseError &e) {
        app_.exit(e);
        return false;
    }
}

const CommandLineArgs& CLIParser::getArgs() const {
    return args_;
}


