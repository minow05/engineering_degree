#include "terminal.hpp"

namespace smart_ssh::ui {

namespace smart_ssh::ui::color {
    constexpr const char* reset   = "\033[0m";
    constexpr const char* bold    = "\033[1m";
    constexpr const char* red     = "\033[31m";
    constexpr const char* green   = "\033[32m";
    constexpr const char* yellow  = "\033[33m";
    constexpr const char* blue    = "\033[34m";
    constexpr const char* magenta = "\033[35m";
    constexpr const char* cyan    = "\033[36m";
    constexpr const char* gray    = "\033[90m";
}

void init_terminal() {
    // TODO: User implementation
}

void clear_screen() {
    // TODO: User implementation
}

void print_header(const std::string& title) {
    // TODO: User implementation
}

void print_success(const std::string& msg) {
    // TODO: User implementation
}

void print_info(const std::string& msg) {
    // TODO: User implementation
}

void print_warning(const std::string& msg) {
    // TODO: User implementation
}

void print_error(const std::string& msg) {
    // TODO: User implementation
}

std::string prompt(const std::string& label, const std::string& default_value) {
    // TODO: User implementation
    return "";
}

std::string prompt_password(const std::string& label) {
    // TODO: User implementation
    return "";
}

bool confirm(const std::string& question, bool default_yes) {
    // TODO: User implementation
    return default_yes;
}

} // namespace smart_ssh::ui
