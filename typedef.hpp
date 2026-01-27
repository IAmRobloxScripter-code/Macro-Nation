#pragma once
#include <string>
struct SETTINGS {
  bool walk_to_hive = false;
  bool drift_comp = false;
  bool mondo = false;
  int mondo_wait = 60;
  int convert_every = 10;
  int selected_hive = 1;
  int selected_field = 0;
  int selected_pattern = 0;
  int direction = 0;
  int selected_theme = 0;
  int width = 1;
  int length = 1;
  int slot_2 = 0;
  int slot_3 = 0;
  int slot_4 = 0;
  int slot_5 = 0;
  int slot_6 = 0;
  int slot_7 = 0;

  float base_walkspeed = 16;

  std::string discord_webhook = "";
  std::string start_key = "F7";
  std::string stop_key = "F8";
};

inline std::string json_macro_settings_default = R"json(
{
  "base walkspeed": 16.0,
  "convert every": 10,
  "direction": 1,
  "drift_comp": false,
  "field": 1,
  "hive": 1,
  "length": 2,
  "mondo": false,
  "mondo_wait": 60,
  "pattern": 0,
  "selected theme": 0,
  "slot_2": 0,
  "slot_3": 0,
  "slot_4": 0,
  "slot_5": 0,
  "slot_6": 0,
  "slot_7": 0,
  "start key": "F7",
  "stop key": "F8",
  "walk to hive": false,
  "webhook": "",
  "width": 2
}
)json";
