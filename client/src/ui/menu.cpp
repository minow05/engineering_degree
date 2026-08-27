#include "menu.hpp"

namespace smart_ssh::ui {

menu::menu(std::string title) : m_title(std::move(title)) {}

void menu::add_item(const std::string& label, std::function<void()> action) {
    m_items.push_back({label, std::move(action)});
}

void menu::run() {
    // TODO: User implementation
}

} // namespace smart_ssh::ui
