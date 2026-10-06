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

std::string prompt(const std::string& label, const std::string& default_value = "");
std::string prompt_password(const std::string& label);
bool confirm(const std::string& question, bool default_yes = true);
char read_key_immediate();
void wait_for_key();

} // namespace smart_ssh::ui
