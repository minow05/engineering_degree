#pragma once

#include <string>

namespace smart_ssh::utils {

struct exec_result {
    int exit_code{-1};
    std::string stdout_str;
    std::string stderr_str;
};

// Execute command in background and capture stdout/stderr output
[[nodiscard]] exec_result run_command(const std::string& command);

// Execute interactive command with direct TTY terminal passthrough
[[nodiscard]] int run_interactive(const std::string& command);

// Check if a system CLI command is available in PATH
[[nodiscard]] bool has_command(const std::string& cmd_name);

} // namespace smart_ssh::utils
