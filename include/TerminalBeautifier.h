#ifndef HAICL_TERMINAL_BEAUTIFIER_H
#define HAICL_TERMINAL_BEAUTIFIER_H

#include <string>

namespace TerminalBeautifier {

// Text colors
std::string red(const std::string& text);
std::string green(const std::string& text);
std::string yellow(const std::string& text);
std::string blue(const std::string& text);
std::string magenta(const std::string& text);
std::string cyan(const std::string& text);
std::string white(const std::string& text);

// Text styles
std::string bold(const std::string& text);
std::string underline(const std::string& text);
std::string reset(const std::string& text);

} // namespace TerminalBeautifier

#endif // HAICL_TERMINAL_BEAUTIFIER_H


