#include "../macro_lib_impl.hpp"

void pattern(int reps, int size) {
  if (application_pointer->machine_state->facing != FACES::Right) {
    face_right();
    application_pointer->machine_state->facing = FACES::Right;
  }
  loop(reps) {
    walk(8 * size, forwards);
    sleep(20);
    walk(2 * size, right);
    sleep(20);
    walk(8 * size, backwards);
    sleep(20);
    walk(2 * size, right);
    sleep(20);
  }

  loop(reps) {
    walk(8 * size, forwards);
    sleep(20);
    walk(2 * size, left);
    sleep(20);
    walk(8 * size, backwards);
    sleep(20);
    walk(2 * size, left);
    sleep(20);
  }
}

IMPL_PATTERN("tidalwave", pattern);