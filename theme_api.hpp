#pragma once

#include <functional>
#include <string>
struct ImGuiStyle;
namespace ImGui {
ImGuiStyle &GetStyle();
}
extern ImGuiStyle default_theme;
extern std::unordered_map<std::string, std::function<void()>> THEMES;
std::function<void()> get_theme(const std::string &theme);
void create_theme(const std::string &name, const std::function<void()> &theme);
void default_theme_callback();
void init_defaults();