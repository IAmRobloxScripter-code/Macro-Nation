#pragma once
#include "application.hpp"
#include "utils.hpp"
#include "walk.hpp"
#include <string>
#include <unordered_map>
enum class move_direction : u8 {
  left,
  right,
  forwards,
  backwards,
  forwards_left,
  forwards_right,
  backwards_left,
  backwards_right
};

#define left move_direction::left
#define right move_direction::right
#define forwards move_direction::forwards
#define backwards move_direction::backwards
#define forwards_left move_direction::forwards_left
#define backwards_left move_direction::backwards_left
#define forwards_right move_direction::forwards_right
#define backwards_right move_direction::backwards_right

#define loop(x) for (int index = 0; index < x; ++index)

inline void walk(u64 tiles, move_direction direction, bool apply_speed = true) {
  switch (direction) {
  case left:
    application_pointer->output->key_down("a");
    break;
  case right:
    application_pointer->output->key_down("d");
    break;
  case forwards:
    application_pointer->output->key_down("w");
    break;
  case backwards:
    application_pointer->output->key_down("s");
    break;
  case forwards_left:
    application_pointer->output->key_down("w");
    application_pointer->output->key_down("a");
    break;
  case forwards_right:
    application_pointer->output->key_down("w");
    application_pointer->output->key_down("d");
    break;
  case backwards_left:
    application_pointer->output->key_down("s");
    application_pointer->output->key_down("a");
    break;
  case backwards_right:
    application_pointer->output->key_down("s");
    application_pointer->output->key_down("d");
    break;
  }

  WALK walk_class = WALK();
  walk_class.base_walkspeed = application_pointer->settings->base_walkspeed;
  walk_class.input = application_pointer->input;
  walk_class.walk(tiles, &application_pointer->running, apply_speed);

  switch (direction) {
  case left:
    application_pointer->output->key_up("a");
    break;
  case right:
    application_pointer->output->key_up("d");
    break;
  case forwards:
    application_pointer->output->key_up("w");
    break;
  case backwards:
    application_pointer->output->key_up("s");
    break;
  case forwards_left:
    application_pointer->output->key_up("w");
    application_pointer->output->key_up("a");
    break;
  case forwards_right:
    application_pointer->output->key_up("w");
    application_pointer->output->key_up("d");
    break;
  case backwards_left:
    application_pointer->output->key_up("s");
    application_pointer->output->key_up("a");
    break;
  case backwards_right:
    application_pointer->output->key_up("s");
    application_pointer->output->key_up("d");
    break;
  }
}

#define sleep(ms)                                                              \
  if (!application_pointer->running)                                           \
    return;                                                                    \
  wait_for(ms);                                                                \
  if (!application_pointer->running)                                           \
    return;

inline void face_left() {
  application_pointer->output->key_press("comma");
  sleep(20);
  application_pointer->output->key_press("comma");
}

inline void face_right() {
  application_pointer->output->key_press("period");
  sleep(20);
  application_pointer->output->key_press("period");
}

inline std::unordered_map<std::string, void (*)(int, int)> &patterns_impl() {
  static std::unordered_map<std::string, void (*)(int, int)> registry;
  return registry;
}

inline std::unordered_map<std::string, void (*)(bool)> &fields_impl() {
  static std::unordered_map<std::string, void (*)(bool)> registry;
  return registry;
}

inline std::unordered_map<std::string, void (*)()> &sub_macros() {
  static std::unordered_map<std::string, void (*)()> registry;
  return registry;
}

#define IMPL_PATTERN(name, impl)                                               \
  static bool _##impl##_registered = []() {                                    \
    patterns_impl()[name] = impl;                                              \
    return true;                                                               \
  }()
#define IMPL_FIELD(name, impl)                                                 \
  static bool _##impl##_registered = []() {                                    \
    fields_impl()[name] = impl;                                                \
    return true;                                                               \
  }()
#define IMPL_SUB_MACRO(name, impl)                                             \
  static bool _##impl##_registered = []() {                                    \
    sub_macros()[name] = impl;                                                 \
    return true;                                                               \
  }()