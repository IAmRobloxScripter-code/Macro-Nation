#include "../macro_lib_impl.hpp"

/*
    nm_gotoramp()
    nm_gotocannon()
    send "{e down}"
    HyperSleep(100)
    send "{e up}{" RightKey " down}{" BackKey " down}"
    HyperSleep(700)
    send "{space 2}"
    HyperSleep(1700)
    send "{" RightKey " up}{" BackKey " up}{space}{" RotRight " 2}"
    Sleep 2000
*/

void from_hive_to_field_strawberry() {
  sleep(1000);
  application_pointer->output->key_press("e");
  sleep(100);
  application_pointer->output->key_down("d");
  application_pointer->output->key_down("s");
  sleep(700);
  application_pointer->output->key_press("space");
  sleep(20);
  application_pointer->output->key_press("space");
  sleep(1700);
  application_pointer->output->key_press("space");
  sleep(6 * 1000);
  application_pointer->output->key_up("d");
  application_pointer->output->key_up("s");
}

void from_field_to_hive_strawberry() {
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

void strawberry_field(bool to_hive) {
  if (!to_hive)
    from_hive_to_field_strawberry();
  else
    from_field_to_hive_strawberry();
}

IMPL_FIELD("strawberry", strawberry_field);