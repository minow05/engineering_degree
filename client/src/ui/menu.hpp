#pragma once

#include <string>
#include <vector>
#include <functional>

namespace smart_ssh::ui {

struct menu_item {
    std::string label;
    std::function<void()> action;
};

class menu {
public:
    explicit menu(std::string title);
    void add_item(const std::string& label, std::function<void()> action);
    void run();

private:
    std::string m_title;
    std::vector<menu_item> m_items;
};

} // namespace smart_ssh::ui
