#include "../macro_lib_impl.hpp"

void to_mondo() {
  sleep(1000);
  application_pointer->output->key_press("e");
  sleep(4000);
  walk(14, right);
  sleep(1000);
}

IMPL_SUB_MACRO("mondo", to_mondo);