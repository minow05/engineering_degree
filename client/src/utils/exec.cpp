#include "exec.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <memory>

#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

namespace smart_ssh::utils {

[[nodiscard]] exec_result run_command(const std::string& command) {
    exec_result result;
    std::array<char, 256> buffer{};

    std::string full_cmd = command + " 2>&1";

    std::unique_ptr<FILE, decltype(&PCLOSE)> pipe(POPEN(full_cmd.c_str(), "r"), PCLOSE);
    if (!pipe) {
        result.exit_code = -1;
        result.stderr_str = "Failed to open process pipe.";
        return result;
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result.stdout_str += buffer.data();
    }

    result.exit_code = 0;
    return result;
}

[[nodiscard]] int run_interactive(const std::string& command) {
    return std::system(command.c_str());
}

[[nodiscard]] bool has_command(const std::string& cmd_name) {
#ifdef _WIN32
    std::string check_cmd = "where " + cmd_name + " >nul 2>&1";
#else
    std::string check_cmd = "which " + cmd_name + " >/dev/null 2>&1";
#endif
    return (std::system(check_cmd.c_str()) == 0);
}

} // namespace smart_ssh::utils
