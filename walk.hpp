#include "utils.hpp"
class WALK {
public:
  double base_walkspeed = 0;
  double walkspeed = 0;
  X11_INPUT *input;
  X11_OUTPUT *output;
  void walk(u64 tiles, bool *running, bool apply_speed = true);
  void calculate_walkspeed();
};

void init_create_bitmaps();