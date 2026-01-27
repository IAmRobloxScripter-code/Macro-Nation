#include "interface.hpp"

#include <GLFW/glfw3.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <unordered_map>

#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/imgui.h"
#include "utils.hpp"
using json = nlohmann::json;

std::string key_to_string(ImGuiKey key) {
  if (key == ImGuiKey_None)
    return "None";

  const char *keyName = ImGui::GetKeyName(key);
  if (keyName && keyName[0] != '\0') {
    return std::string(keyName);
  }
  return "Unknown";
}

void update_json_settings(INTERFACE *self) {
  auto jsonified =
      nlohmann::json{{"start key", self->settings->start_key},
                     {"stop key", self->settings->stop_key},
                     {"base walkspeed", self->settings->base_walkspeed},
                     {"walk to hive", self->settings->walk_to_hive},
                     {"convert every", self->settings->convert_every},
                     {"pattern", self->settings->selected_pattern},
                     {"field", self->settings->selected_field},
                     {"hive", self->settings->selected_hive},
                     {"webhook", self->settings->discord_webhook},
                     {"selected theme", self->settings->selected_theme},
                     {"width", self->settings->width},
                     {"length", self->settings->length},
                     {"drift_comp", self->settings->drift_comp},
                     {"direction", self->settings->direction},
                     {"mondo", self->settings->mondo},
                     {"mondo_wait", self->settings->mondo_wait},
                     {"slot_2", self->settings->slot_2},
                     {"slot_3", self->settings->slot_3},
                     {"slot_4", self->settings->slot_4},
                     {"slot_5", self->settings->slot_5},
                     {"slot_6", self->settings->slot_6},
                     {"slot_7", self->settings->slot_7}};
  std::ofstream file("_macro_cache_/macro_settings.json");
  if (!file.is_open()) {
    exit(1);
  }
  file << jsonified.dump(4);
  file.close();
}

void INTERFACE::main_tab_button() {
  if (ImGui::BeginTabItem("Main")) {
    ImGui::SetWindowFontScale(2);
    ImGui::Text("%s", "Movement");
    ImGui::SetWindowFontScale(1.5);
    ImGui::Spacing();

    ImGui::BeginGroup();
    this->selected_hive_counter();
    this->base_walkspeed_counter();
    ImGui::EndGroup();
    ImGui::SameLine(0, 20);
    ImGui::BeginGroup();
    this->fields_dropdown();
    this->patterns_dropdown();
    ImGui::EndGroup();
    ImGui::SameLine(0, 20);
    this->drift_comp_checkbox();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::SetWindowFontScale(2);
    ImGui::Text("%s", "Converting");
    ImGui::SetWindowFontScale(1.5);
    ImGui::Spacing();

    this->convert_every_minutes_counter();
    ImGui::SameLine(0, 20);
    this->walk_to_hive_checkbox();
    ImGui::EndTabItem();
  }
}

void INTERFACE::themes_dropdown() {
  if (ImGui::Combo("Theme", &this->settings->selected_theme,
                   this->THEME_OPTIONS.data(), this->THEME_OPTIONS.size())) {
    this->update_theme(this->THEME_OPTIONS[this->settings->selected_theme]);
    update_json_settings(this);
  }
}

void INTERFACE::mondo_group() {
  ImGui::BeginGroup();
  if (ImGui::Checkbox("Mondo", &this->settings->mondo)) {
    update_json_settings(this);
  }
  if (this->settings->mondo) {
    ImGui::SameLine(0, 20);
    if (ImGui::InputInt("Seconds", &this->settings->mondo_wait, 1, 5)) {
      if (this->settings->mondo_wait < 0) {
        this->settings->mondo_wait = 0;
      }
      update_json_settings(this);
    }
  }
  ImGui::EndGroup();
}

void INTERFACE::collect_and_kill_tab_button() {
  if (ImGui::BeginTabItem("Collect/Kill")) {
    ImGui::SetWindowFontScale(2);
    ImGui::Text("%s", "Collect");
    ImGui::SetWindowFontScale(1.5);
    ImGui::Spacing();
    this->mondo_group();
    ImGui::EndTabItem();
  }
}

void INTERFACE::settings_tab_button() {
  if (ImGui::BeginTabItem("Settings")) {
    ImGui::SetWindowFontScale(2);
    ImGui::Text("%s", "Pattern Settings");
    ImGui::SetWindowFontScale(1.5);
    ImGui::Spacing();
    this->pattern_settings();
    ImGui::SetWindowFontScale(2);
    ImGui::Text("%s", "Misc");
    ImGui::SetWindowFontScale(1.5);
    ImGui::Spacing();
    this->themes_dropdown();
    this->discord_webhook_input();
    ImGui::SetWindowFontScale(2);
    ImGui::Text("%s", "Keybinds");
    ImGui::SetWindowFontScale(1.5);
    this->start_key_capture();
    ImGui::SameLine(0, 20);
    this->stop_key_capture();
    ImGui::EndTabItem();
  }
}

void INTERFACE::start_key_capture() {
  ImGui::PushID(1);
  if (ImGui::Button(
          !this->is_waiting_for_start_key
              ? ("Start Key (" + this->settings->start_key + ")").c_str()
              : "Press any key...")) {
    this->is_waiting_for_start_key = true;
  }

  if (this->is_waiting_for_start_key) {
    for (ImGuiKey key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END;
         key = (ImGuiKey)(key + 1))
      if (ImGui::IsKeyPressed(key, false)) {
        this->is_waiting_for_start_key = false;
        this->REHOOK_KEY_EVENT->fire(key_to_string(key),
                                     REHOOK_KEY_EVENT_TYPE::START_KEY);
        this->settings->start_key = key_to_string(key);
        update_json_settings(this);
        break;
      }
  }

  if (this->is_waiting_for_start_key &&
      ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
    this->is_waiting_for_start_key = false;
  }
  ImGui::PopID();
}

void INTERFACE::stop_key_capture() {
  ImGui::PushID(2);
  if (ImGui::Button(
          !this->is_waiting_for_stop_key
              ? ("Stop Key (" + this->settings->stop_key + ")").c_str()
              : "Press any key...")) {
    this->is_waiting_for_stop_key = true;
  }

  if (this->is_waiting_for_stop_key) {
    for (ImGuiKey key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END;
         key = (ImGuiKey)(key + 1))
      if (ImGui::IsKeyPressed(key, false)) {
        this->is_waiting_for_stop_key = false;
        this->REHOOK_KEY_EVENT->fire(key_to_string(key),
                                     REHOOK_KEY_EVENT_TYPE::STOP_KEY);
        this->settings->stop_key = key_to_string(key);
        update_json_settings(this);
        break;
      }
  }

  if (this->is_waiting_for_stop_key &&
      ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
    this->is_waiting_for_stop_key = false;
  }
  ImGui::PopID();
}

void INTERFACE::selected_hive_counter() {
  if (ImGui::InputInt("Hive", &this->settings->selected_hive, 1)) {
    if (this->settings->selected_hive < 1)
      this->settings->selected_hive = 6;
    else if (this->settings->selected_hive > 6)
      this->settings->selected_hive = 1;

    update_json_settings(this);
  }
}

void INTERFACE::base_walkspeed_counter() {
  if (ImGui::InputFloat("Base Walkspeed", &this->settings->base_walkspeed, 0.1,
                        1, "%.2f")) {
    if (this->settings->base_walkspeed <= 0)
      this->settings->base_walkspeed = 0;
    update_json_settings(this);
  }
}

void INTERFACE::walk_to_hive_checkbox() {
  if (ImGui::Checkbox("Walk To Hive", &this->settings->walk_to_hive)) {
    update_json_settings(this);
  }
}

void INTERFACE::convert_every_minutes_counter() {
  if (ImGui::InputInt("Convert Every ", &this->settings->convert_every, 1)) {
    if (this->settings->convert_every < 0)
      this->settings->convert_every = 0;
    update_json_settings(this);
  }
}

void INTERFACE::fields_dropdown() {
  if (ImGui::Combo("Field", &this->settings->selected_field,
                   this->FIELD_OPTIONS.data(), this->FIELD_OPTIONS.size())) {
    update_json_settings(this);
  }
}

void INTERFACE::patterns_dropdown() {
  if (ImGui::Combo("Pattern", &this->settings->selected_pattern,
                   this->PATTERN_OPTIONS.data(),
                   this->PATTERN_OPTIONS.size())) {
    update_json_settings(this);
  }
}

void INTERFACE::discord_webhook_input() {
  if (ImGui::InputText("Discord Webhook", this->DISCORD_WEBHOOK_URL, 256)) {
    this->settings->discord_webhook = this->DISCORD_WEBHOOK_URL;
    update_json_settings(this);
  }
}
static std::vector<const char *> pattern_size_options = {"XS", "S", "M", "L",
                                                         "XL"};
void INTERFACE::pattern_settings() {
  if (ImGui::InputInt("Width", &this->settings->width, 1, 5)) {
    if (this->settings->width < 1)
      this->settings->width = 1;
    update_json_settings(this);
  }
  ImGui::SameLine(0, 20);
  if (ImGui::Combo("Length", &this->settings->length,
                   pattern_size_options.data(), pattern_size_options.size())) {
    update_json_settings(this);
  }
  this->directions_dropdown();
}

void INTERFACE::drift_comp_checkbox() {
  if (ImGui::Checkbox("Drift Comp", &this->settings->drift_comp)) {
    update_json_settings(this);
  }
}

void INTERFACE::directions_dropdown() {
  if (ImGui::Combo("Position", &this->settings->direction,
                   this->DIRECTION_OPTIONS.data(),
                   this->DIRECTION_OPTIONS.size())) {
    update_json_settings(this);
  }
}

void INTERFACE::slots_group() {
  if (ImGui::Combo("2", &this->settings->slot_2, this->INVENTORY_OPTIONS.data(),
                   this->INVENTORY_OPTIONS.size()) ||
      ImGui::Combo("3", &this->settings->slot_3, this->INVENTORY_OPTIONS.data(),
                   this->INVENTORY_OPTIONS.size()) ||
      ImGui::Combo("4", &this->settings->slot_4, this->INVENTORY_OPTIONS.data(),
                   this->INVENTORY_OPTIONS.size()) ||
      ImGui::Combo("5", &this->settings->slot_5, this->INVENTORY_OPTIONS.data(),
                   this->INVENTORY_OPTIONS.size()) ||
      ImGui::Combo("6", &this->settings->slot_6, this->INVENTORY_OPTIONS.data(),
                   this->INVENTORY_OPTIONS.size()) ||
      ImGui::Combo("7", &this->settings->slot_7, this->INVENTORY_OPTIONS.data(),
                   this->INVENTORY_OPTIONS.size())) {
    update_json_settings(this);
  }
}

void INTERFACE::boost_tab_button() {
  if (ImGui::BeginTabItem("Boost")) {
    ImGui::SetWindowFontScale(2);
    ImGui::Text("%s", "Hotbar Slots");
    ImGui::SetWindowFontScale(1.5);
    ImGui::Spacing();
    this->slots_group();
    ImGui::EndTabItem();
  }
}

void INTERFACE::render() {
  glfwPollEvents();

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Macro Nation");
  ImGui::SetWindowFontScale(1.5);
  ImGui::PushItemWidth(this->X_WIDTH);
  ImGui::BeginTabBar("Tabs");
  this->main_tab_button();
  this->boost_tab_button();
  this->collect_and_kill_tab_button();
  this->settings_tab_button();
  ImGui::EndTabBar();

  ImGui::End();
  ImGui::Render();

  int width, height;
  glfwGetFramebufferSize(this->window, &width, &height);
  glViewport(0, 0, width, height);

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(this->window);
}

INTERFACE::INTERFACE(const std::vector<std::string> &fields,
                     const std::vector<std::string> &patterns) {
  init_defaults();
  for (int index = 0; index < fields.size(); ++index)
    this->FIELD_OPTIONS.push_back(fields[index].c_str());

  for (int index = 0; index < patterns.size(); ++index)
    this->PATTERN_OPTIONS.push_back(patterns[index].c_str());

  for (const auto &theme : THEMES)
    this->THEME_OPTIONS.push_back(theme.first.c_str());

  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
  this->window = glfwCreateWindow(1060, 620, "Macro Nation", nullptr, nullptr);
  glfwMakeContextCurrent(this->window);
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui_ImplGlfw_InitForOpenGL(this->window, true);
  ImGui_ImplOpenGL3_Init("#version 130");
  default_theme = ImGui::GetStyle();
}