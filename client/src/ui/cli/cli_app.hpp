#pragma once

#include "core/application.hpp"

namespace smart_ssh::ui::cli {

class cli_app {
public:
    explicit cli_app(core::application& app);
    int run();

private:
    void render_main_menu();
    void render_settings_menu();

    core::application& m_app;
};

} // namespace smart_ssh::ui::cli
