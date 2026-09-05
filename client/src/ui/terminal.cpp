#include "terminal.hpp"
#include <cstdlib>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace smart_ssh::ui {

namespace color {
    constexpr const char* reset   = "\033[0m";
    constexpr const char* bold    = "\033[1m";
    constexpr const char* cyan    = "\033[36m";
    constexpr const char* green   = "\033[32m";
    constexpr const char* yellow  = "\033[33m";
    constexpr const char* red     = "\033[31m";
    constexpr const char* gray    = "\033[90m";
}

void init_terminal() {
#ifdef _WIN32
    HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h_out != INVALID_HANDLE_VALUE) {
        DWORD dw_mode = 0;
        if (GetConsoleMode(h_out, &dw_mode)) {
            dw_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(h_out, dw_mode);
        }
    }
#endif
}

void clear_screen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::cout << "\033[2J\033[1;1H";
#endif
}

void print_header(const std::string& title) {
    std::cout << "\n" << color::bold << color::cyan 
              << "========================================================\n"
              << "  " << title << "\n"
              << "========================================================\n"
              << color::reset << "\n";
}

void print_success(const std::string& msg) {
    std::cout << color::green << "[+] " << msg << color::reset << "\n";
}

void print_info(const std::string& msg) {
    std::cout << color::cyan << "[i] " << msg << color::reset << "\n";
}

void print_warning(const std::string& msg) {
    std::cout << color::yellow << "[!] " << msg << color::reset << "\n";
}

void print_error(const std::string& msg) {
    std::cout << color::red << "[-] ERROR: " << msg << color::reset << "\n";
}

[[nodiscard]] std::string prompt(const std::string& label, const std::string& default_value) {
    std::cout << color::bold << label << color::reset;
    if (!default_value.empty()) {
        std::cout << color::gray << " [" << default_value << "]" << color::reset;
    }
    std::cout << ": ";

    std::string input;
    std::getline(std::cin, input);
    if (input.empty() && !default_value.empty()) {
        return default_value;
    }
    return input;
}

[[nodiscard]] std::string prompt_password(const std::string& label) {
    std::cout << color::bold << label << ": " << color::reset;
    std::string pass;
    std::getline(std::cin, pass);
    return pass;
}

[[nodiscard]] bool confirm(const std::string& question, bool default_yes) {
    std::string default_str = default_yes ? "Y/n" : "y/N";
    std::string input = prompt(question + " (" + default_str + ")");
    if (input.empty()) return default_yes;
    return (input[0] == 'y' || input[0] == 'Y');
}

} // namespace smart_ssh::ui
