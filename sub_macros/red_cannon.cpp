#include "../macro_lib_impl.hpp"

void to_cannon() {
  walk(6, forwards);
  sleep(20);
  walk(3, right);
  sleep(20);
  if (application_pointer->settings->selected_hive > 1) {
    loop(application_pointer->settings->selected_hive - 1) {
      walk(1, right);
      sleep(20);
      walk(6, right);
      sleep(20);
    }

    walk(3, right);
    sleep(20);
  } else {
    walk(3, right);
    sleep(20);
  }
  walk(3, right);
  sleep(500);
  application_pointer->output->key_down("space");
  walk(3, right);
  application_pointer->output->key_up("space");
  sleep(1000);
  walk(3, right);
  sleep(20);
  application_pointer->output->key_down("space");
  walk(2, right);
  application_pointer->output->key_up("space");
  sleep(1000);
  walk(4, right);
}

IMPL_SUB_MACRO("red_cannon", to_cannon);