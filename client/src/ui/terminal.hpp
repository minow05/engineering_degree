#pragma once

#include <string>

namespace smart_ssh::ui {

void init_terminal();
void clear_screen();
void print_header(const std::string& title);
void print_success(const std::string& msg);
void print_info(const std::string& msg);
void print_warning(const std::string& msg);
void print_error(const std::string& msg);

[[nodiscard]] std::string prompt(const std::string& label, const std::string& default_value = "");
[[nodiscard]] std::string prompt_password(const std::string& label);
[[nodiscard]] bool confirm(const std::string& question, bool default_yes = true);

} // namespace smart_ssh::ui
