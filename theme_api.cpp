#include "theme_api.hpp"
#include "imgui/imgui.h"
#include <unordered_map>

std::unordered_map<std::string, std::function<void()>> THEMES = {};

void create_theme(const std::string &name, const std::function<void()> &theme) {
  THEMES[name] = theme;
}
ImGuiStyle default_theme;
void default_theme_callback() { ImGui::GetStyle() = default_theme; };

void init_defaults() { THEMES["Default"] = default_theme_callback; }
std::function<void()> get_theme(const std::string &theme) {
  return THEMES[theme];
}