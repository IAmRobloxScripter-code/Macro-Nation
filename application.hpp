#pragma once
#include "interface.hpp"
#include <map>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

struct RAW_DATA {
  std::map<std::string, json> RAW_FIELDS_DATA;
  std::map<std::string, json> RAW_PATTERNS_DATA;
};

enum class FACES : u8 { Forward, Backward, Left, Right };

struct STATE {
  bool converting = false;
  bool collecting = false;
  bool at_hive = false;
  bool placed_sprinkler = false;
  bool checked_mondo = false;

  int collecting_begin = 0;
  int drift_comp_collection_tick = 0;
  FACES facing = FACES::Forward;
};

enum class HOTBAR_USE_METHOD : u8 {
  Never,
  Gather_Start,
  On_Mondo,
  At_Hive,
  On_Convert
};

class APPLICATION {
public:
  SETTINGS *settings;
  INTERFACE *interface;
  X11_INPUT *input;
  X11_OUTPUT *output;
  RAW_DATA *raw_data;
  STATE *machine_state;
  bool running = false;
  APPLICATION();
  void main();
  void start();
  void stop();
  void step();
  void execute_json(json &data);
  void action_collect();
  void action_drift_comp();
  void check_hive();
  void to_field();
  void to_red_cannon();
  void align_in_field();
  void place_sprinkler();
  void convert();
  bool collect_timer_check();
  void finish_collect();
  void align_mouse();
  void use_hotbar(HOTBAR_USE_METHOD method);
  bool mondo();
};

extern APPLICATION *application_pointer;