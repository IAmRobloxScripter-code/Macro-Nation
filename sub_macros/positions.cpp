#include "../macro_lib_impl.hpp"

std::vector<u8> get_field_size() {
  return FIELD_SIZES[application_pointer->interface->FIELD_OPTIONS
                         [application_pointer->settings->selected_field]];
}
/*
   23
_________
[LL   LR]       ^
[       ]       ^
[       ]       ^
[       ]  31   ^ FORWARDS
[       ]       ^
[       ]       ^
[UL   UR]       ^
---------
*/
void upper_left() {
  auto size = get_field_size();
  walk(10, forwards);
  sleep(20);
  walk(size[0] - 10, left);
  sleep(20);
}

void upper_right() {
  auto size = get_field_size();
  walk(10, forwards);
  sleep(20);
  walk(10, left);
  sleep(20);
}

void middle() {
  auto size = get_field_size();
  walk(size[1] / 2, forwards);
  sleep(20);
  walk(size[0] / 2, left);
  sleep(20);
}

void lower_left() {
  auto size = get_field_size();
  walk(size[1] - 10, forwards);
  sleep(20);
  walk(size[0] - 10, left);
  sleep(20);
}

void lower_right() {
  auto size = get_field_size();
  walk(size[1] - 10, forwards);
  sleep(20);
  walk(10, left);
  sleep(20);
}

IMPL_SUB_MACRO("Upper_Left", upper_left);
IMPL_SUB_MACRO("Upper_Right", upper_right);
IMPL_SUB_MACRO("Middle", middle);
IMPL_SUB_MACRO("Lower_Left", lower_left);
IMPL_SUB_MACRO("Lower_Right", lower_right);