
#include "TerminalBeautifier.h"

namespace TerminalBeautifier {

// ANSI escape codes for colors
const std::string RESET_COLOR = "\033[0m";
const std::string RED_COLOR = "\033[31m";
const std::string GREEN_COLOR = "\033[32m";
const std::string YELLOW_COLOR = "\033[33m";
const std::string BLUE_COLOR = "\033[34m";
const std::string MAGENTA_COLOR = "\033[35m";
const std::string CYAN_COLOR = "\033[36m";
const std::string WHITE_COLOR = "\033[37m";

// ANSI escape codes for styles
const std::string BOLD_STYLE = "\033[1m";
const std::string UNDERLINE_STYLE = "\033[4m";

std::string red(const std::string& text) {
    return RED_COLOR + text + RESET_COLOR;
}

std::string green(const std::string& text) {
    return GREEN_COLOR + text + RESET_COLOR;
}

std::string yellow(const std::string& text) {
    return YELLOW_COLOR + text + RESET_COLOR;
}

std::string blue(const std::string& text) {
    return BLUE_COLOR + text + RESET_COLOR;
}

std::string magenta(const std::string& text) {
    return MAGENTA_COLOR + text + RESET_COLOR;
}

std::string cyan(const std::string& text) {
    return CYAN_COLOR + text + RESET_COLOR;
}

std::string white(const std::string& text) {
    return WHITE_COLOR + text + RESET_COLOR;
}

std::string bold(const std::string& text) {
    return BOLD_STYLE + text + RESET_COLOR;
}

std::string underline(const std::string& text) {
    return UNDERLINE_STYLE + text + RESET_COLOR;
}

std::string reset(const std::string& text) {
    return text + RESET_COLOR;
}

} // namespace TerminalBeautifier


