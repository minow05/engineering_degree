#include "menu.hpp"
#include "terminal.hpp"
#include <iostream>

namespace smart_ssh::ui {

menu::menu(std::string title) : m_title(std::move(title)) {}

void menu::add_item(const std::string& label, std::function<void()> action) {
    m_items.push_back({label, std::move(action)});
}

void menu::run() {
    bool running = true;
    while (running) {
        clear_screen();
        print_header(m_title);

        for (size_t i = 0; i < m_items.size(); ++i) {
            std::cout << "  " << (i + 1) << ") " << m_items[i].label << "\n";
        }
        std::cout << "  0) Back / Exit\n\n";

        std::cout << "Select an option [0-" << m_items.size() << "]: ";
        std::cout.flush();

        char key = read_key_immediate();
        std::cout << key << "\n"; // Instant visual echo

        if (key == '0' || key == 27 /* ESC */) {
            running = false;
        } else if (key >= '1' && key <= '9') {
            size_t choice = static_cast<size_t>(key - '0');
            if (choice <= m_items.size()) {
                clear_screen();
                m_items[choice - 1].action();
            } else {
                print_warning("Invalid selection range.");
            }
        } else {
            print_warning("Invalid key pressed.");
        }
    }
}

} // namespace smart_ssh::ui
