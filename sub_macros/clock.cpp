#include "../macro_lib_impl.hpp"

void to_clock() {
  sleep(1000);
  application_pointer->output->key_press("e");
  sleep(1350);
  application_pointer->output->key_press("space");
  sleep(20);
  application_pointer->output->key_press("space");
  sleep(20);
  walk(60, forwards_left, false);
  sleep(2000);
  walk(10, left, false);
  sleep(250);
  walk(30, backwards, false);
  sleep(250);
  walk(4, right, true);
  sleep(250);
  walk(3, backwards, true);
  sleep(250);
  application_pointer->output->key_press("e");
  sleep(2000);
}

IMPL_SUB_MACRO("clock", to_clock);