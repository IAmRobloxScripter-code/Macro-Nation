#include "../macro_lib_impl.hpp"

void from_hive_to_field_pinetree() {
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

void from_field_to_hive_pinetree() {
  point_in_global_direction(FACES::Forward);
  sleep(20);
  walk(60, backwards_right, false);
  sleep(20);
  walk(100, left, false);
  sleep(20);
  walk(50, forwards, false);
  sleep(20);
  application_pointer->output->key_press("space");
  sleep(60);
  application_pointer->output->key_press("space");
  sleep(20);
  walk(120, forwards, false);
  sleep(20);
}

void pinetree_field(bool to_hive) {
  if (!to_hive)

    from_hive_to_field_pinetree();
  else
    from_field_to_hive_pinetree();
}

IMPL_FIELD("pine_tree_forest", pinetree_field);