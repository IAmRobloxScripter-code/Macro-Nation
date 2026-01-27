#pragma once

#include <string>
#include <vector>

#include "theme_api.hpp"
#include "typedef.hpp"
#include "utils.hpp"

struct GLFWwindow;

enum class REHOOK_KEY_EVENT_TYPE { START_KEY, STOP_KEY };

class INTERFACE {
public:
  static inline std::vector<const char *> FIELD_OPTIONS;
  static inline std::vector<const char *> PATTERN_OPTIONS;
  static inline std::vector<const char *> THEME_OPTIONS;
  static inline std::vector<const char *> DIRECTION_OPTIONS = {
      "Upper_Left", "Upper_Right", "Middle", "Lower_Left", "Lower_Right"};
  static inline std::vector<const char *> INVENTORY_OPTIONS = {
      "Never", "Gather Start", "On Mondo", "At Hive", "On Convert"};

  static inline char DISCORD_WEBHOOK_URL[256];
  static inline bool is_waiting_for_start_key = false;
  static inline bool is_waiting_for_stop_key = false;
  SIGNAL<void(const std::string &, REHOOK_KEY_EVENT_TYPE)> *REHOOK_KEY_EVENT;
  SETTINGS *settings;
  GLFWwindow *window;
  static const int X_WIDTH = 250;

  INTERFACE(const std::vector<std::string> &fields,
            const std::vector<std::string> &patterns);
  void update_theme(const std::string &theme) { get_theme(theme)(); };
  void render();
  void fields_dropdown();
  void patterns_dropdown();
  void selected_hive_counter();
  void base_walkspeed_counter();
  void walk_to_hive_checkbox();
  void convert_every_minutes_counter();
  void discord_webhook_input();
  void start_key_capture();
  void stop_key_capture();
  void themes_dropdown();
  void main_tab_button();
  void settings_tab_button();
  void pattern_settings();
  void drift_comp_checkbox();
  void directions_dropdown();
  void collect_and_kill_tab_button();
  void boost_tab_button();
  void mondo_group();
  void slots_group();
};