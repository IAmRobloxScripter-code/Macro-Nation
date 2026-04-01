#include "../macro_lib_impl.hpp"

static BITMAP riley_identifier = bitmap_from_png("images/riley.png");
static BITMAP scroll_bar_identifier = bitmap_from_png("images/scroll_bar.png");
static BITMAP completed_identifier = bitmap_from_png("images/complete.png");

static std::unordered_map<std::string, BITMAP> riley_quest_templates = {
    {"Abilities", bitmap_from_png("images/abilities.png")},
    {"Booster", bitmap_from_png("images/booster.png")},
    {"Clean-up", bitmap_from_png("images/clean-up.png")},
    {"Extraction", bitmap_from_png("images/extraction.png")},
    {"Goo", bitmap_from_png("images/goo.png")},
    {"Medley", bitmap_from_png("images/medley.png")},
    {"Mushrooms", bitmap_from_png("images/mushrooms.png")},
    {"Picnic", bitmap_from_png("images/picnic.png")},
    {"Pollen", bitmap_from_png("images/pollen.png")},
    {"Rampage", bitmap_from_png("images/rampage.png")},
    {"Roses", bitmap_from_png("images/roses.png")},
    {"Skirmish", bitmap_from_png("images/skirmish.png")},
    {"Strawberries", bitmap_from_png("images/strawberries.png")},
    {"Tango", bitmap_from_png("images/tango.png")},
    {"Tour", bitmap_from_png("images/tour.png")},
};
// 0 x 170, 309 x 720
int get_riley_quest_offset() {
  BITMAP pBMArea =
      application_pointer->input->capture_display(0, 170, 308, 720);
  // pBMArea.save_png("quest_capture.png", 4);
  u64 y = 0;
  bool found = image_search(pBMArea, riley_identifier, nullptr, &y, 0, 0,
                            pBMArea.width, pBMArea.height, 6);
  if (!found) {
    return -1;
  }
  return y;
}

bool is_at_bottom() {
  BITMAP pBMArea =
      application_pointer->input->capture_display(309, 952, 12, 12);
  return image_search(pBMArea, scroll_bar_identifier, nullptr, nullptr, 0, 0,
                      pBMArea.width, pBMArea.height, 12);
}

void move_mouse_to_quest_scroll() {
  application_pointer->output->mouse_move(312, 208);
}

void scroll_until_quest() {
  move_mouse_to_quest_scroll();
  application_pointer->output->mouse_scroll_up(75);
  int offset = get_riley_quest_offset();
  while (offset != -1) {
    move_mouse_to_quest_scroll();
    application_pointer->output->mouse_scroll_down();
    offset = get_riley_quest_offset();
    if (is_at_bottom()) {
      break;
    }
    sleep(20);
  }
}

struct quest_position_data {
  u64 y_offset = 0;
  std::string quest = "none";
  bool valid = false;
};

quest_position_data find_what_quest() {
  quest_position_data data;
  scroll_until_quest();
  int y_offset = get_riley_quest_offset();
  if (y_offset == -1) {
    return data;
  }

  BITMAP pBMArea =
      application_pointer->input->capture_display(0, 170, 308, 720);
  for (auto item : riley_quest_templates) {
    if (image_search(pBMArea, item.second, nullptr, nullptr, 0, y_offset,
                     pBMArea.width, y_offset + (item.second.height + 1), 12)) {
      data.y_offset = y_offset;
      data.quest = item.first;
      data.valid = true;
      return data;
    }
  }

  return data;
}
// 55 x 220
// 242 x 40
// 50 pix from top to first bar
// 15 pixels inbetween each bar
bool is_completed(u64 y_offset) {
  BITMAP pBMArea =
      application_pointer->input->capture_display(55, 170 + y_offset, 242, 40);
  return image_search(pBMArea, completed_identifier, nullptr, nullptr, 0, 0,
                      pBMArea.width, pBMArea.height, 32);
}

#define FIRST_BAR_OFFSET 50
#define BAR_OFFSET 15

void pollen_quest(quest_position_data data) {
  if (is_completed(data.y_offset + FIRST_BAR_OFFSET)) {
    return;
  }

  fields_impl()["strawberry"](false);
}

void do_quest() {
  quest_position_data data = find_what_quest();
  if (!data.valid) {
    return;
  }
  if (data.quest == "Pollen") {
    pollen_quest(data);
  }
}