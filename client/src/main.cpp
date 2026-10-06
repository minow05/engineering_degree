#include "core/application.hpp"
#include "ui/cli/cli_app.hpp"

using namespace smart_ssh;

int main() {
    core::application app;
    (void)app.initialize();

    ui::cli::cli_app cli(app);
    return cli.run();
}
