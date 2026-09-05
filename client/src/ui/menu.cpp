#include "menu.hpp"
#include "terminal.hpp"
#include <iostream>
#include <limits>

namespace smart_ssh::ui {

menu::menu(std::string title) : m_title(std::move(title)) {}

void menu::add_item(const std::string& label, std::function<void()> action) {
    m_items.push_back({label, std::move(action)});
}

void menu::run() {
    bool running = true;
    while (running) {
        print_header(m_title);

        for (size_t i = 0; i < m_items.size(); ++i) {
            std::cout << "  " << (i + 1) << ") " << m_items[i].label << "\n";
        }
        std::cout << "  0) Back / Exit\n\n";

        std::string choice_str = prompt("Select an option");
        try {
            int choice = std::stoi(choice_str);
            if (choice == 0) {
                running = false;
            } else if (choice > 0 && static_cast<size_t>(choice) <= m_items.size()) {
                std::cout << "\n";
                m_items[choice - 1].action();
            } else {
                print_warning("Invalid selection. Please enter a valid number.");
            }
        } catch (...) {
            print_warning("Invalid input. Please enter a number.");
        }
    }
}

} // namespace smart_ssh::ui
