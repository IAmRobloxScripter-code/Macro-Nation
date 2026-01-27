#include "../macro_lib_impl.hpp"

void from_hive_to_field() {
  sleep(1000);
  application_pointer->output->key_press("e");
  sleep(850);
  application_pointer->output->key_press("space");
  sleep(20);
  application_pointer->output->key_press("space");
  sleep(20);
  walk(60, backwards_right, false);
  sleep(1000);
}

void from_field_to_hive() {}

void field(bool to_hive) {
  if (!to_hive)
    from_hive_to_field();
  else
    from_field_to_hive();
}

IMPL_FIELD("pine_tree_forest", field);