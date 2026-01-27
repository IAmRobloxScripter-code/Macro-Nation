#include "application.hpp"

#include <GLFW/glfw3.h>

#include "macro_lib_impl.hpp"
#include "utils.hpp"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <nlohmann/detail/meta/type_traits.hpp>
#include <string>
#include <unordered_map>
namespace fs = std::filesystem;
APPLICATION *application_pointer = nullptr;
APPLICATION::APPLICATION() {
  if (!glfwInit())
    exit(1);
  json json_settings =
      json::parse(std::ifstream("_macro_cache_/macro_settings.json"));
  this->settings = new SETTINGS();
  this->settings->walk_to_hive = json_settings["walk to hive"];
  this->settings->convert_every = json_settings["convert every"];
  this->settings->selected_field = json_settings["field"];
  this->settings->selected_pattern = json_settings["pattern"];
  this->settings->selected_hive = json_settings["hive"];
  this->settings->base_walkspeed = json_settings["base walkspeed"];
  this->settings->start_key = json_settings["start key"];
  this->settings->stop_key = json_settings["stop key"];
  this->settings->discord_webhook = json_settings["webhook"];
  this->settings->selected_theme = json_settings["selected theme"];
  this->settings->width = json_settings["width"];
  this->settings->length = json_settings["length"];
  this->settings->drift_comp = json_settings["drift_comp"];
  this->settings->direction = json_settings["direction"];
  this->settings->mondo = json_settings["mondo"];
  this->settings->mondo_wait = json_settings["mondo_wait"];
  this->settings->slot_2 = json_settings["slot_2"];
  this->settings->slot_3 = json_settings["slot_3"];
  this->settings->slot_4 = json_settings["slot_4"];
  this->settings->slot_5 = json_settings["slot_5"];
  this->settings->slot_6 = json_settings["slot_6"];
  this->settings->slot_7 = json_settings["slot_7"];

  std::map<std::string, json> fields_data;
  std::map<std::string, json> patterns_data;
  std::vector<std::string> fields_display;
  std::vector<std::string> patterns_display;

  for (const auto &entry : fs::directory_iterator("_macro_cache_/fields")) {
    if (entry.is_regular_file()) {
      json entry_data = json::parse(std::ifstream(entry.path()));
      fields_data[entry_data["display"]] = entry_data;
      fields_display.push_back(entry_data["display"]);
    }
  }

  for (const auto &entry : fs::directory_iterator("_macro_cache_/patterns")) {
    if (entry.is_regular_file()) {
      json entry_data = json::parse(std::ifstream(entry.path()));
      patterns_data[entry_data["display"]] = entry_data;
      patterns_display.push_back(entry_data["display"]);
    }
  }

  this->raw_data = new RAW_DATA;
  this->machine_state = new STATE;
  this->input = new X11_INPUT();
  this->output = new X11_OUTPUT();
  this->interface = new INTERFACE(fields_display, patterns_display);
  this->interface->REHOOK_KEY_EVENT =
      new SIGNAL<void(const std::string &, REHOOK_KEY_EVENT_TYPE)>;
  this->interface->settings = this->settings;
  this->raw_data->RAW_FIELDS_DATA = fields_data;
  this->raw_data->RAW_PATTERNS_DATA = patterns_data;
  strcpy(this->interface->DISCORD_WEBHOOK_URL,
         this->settings->discord_webhook.c_str());
  init_create_bitmaps();
  application_pointer = this;
  std::thread application_thread([this]() { this->main(); });
  while (!glfwWindowShouldClose(this->interface->window)) {
    this->interface->render();
  }

  glfwDestroyWindow(this->interface->window);
  glfwTerminate();
  this->input->stop();
  application_thread.join();
}
static const std::vector<float> pattern_size_options = {0.25, 0.5, 1, 1.5, 2};
void APPLICATION::action_collect() {
  json pattern_data =
      this->raw_data->RAW_PATTERNS_DATA
          [this->interface->PATTERN_OPTIONS[this->settings->selected_pattern]];
  const std::string pattern_name = pattern_data["name"];
  auto it = patterns_impl().find(pattern_name);
  if (it != patterns_impl().end()) {
    this->output->mouse_down();
    it->second(this->settings->width,
               pattern_size_options[this->settings->length]);
    this->output->mouse_up();
  }
}

static std::unordered_map<std::string, BITMAP> bitmaps = {
    {"pBMSupremeSaturator",
     bitmap_from_base64("iVBORw0KGgoAAAANSUhEUgAAAAsAAAANCAYAAAB/"
                        "9ZQ7AAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAAJcEhZcw"
                        "AADsMAAA7DAcdvqGQAAAAZSURBVChTY2C88f8/sXhUMTIeVYzA//"
                        "8DAD1Dlimzf8yLAAAAAElFTkSuQmCC")},
    {"pBMSprinklerSlot_Day",
     bitmap_from_base64(
         "iVBORw0KGgoAAAANSUhEUgAAADoAAAAfCAIAAAA5uEn6AAAACXBIWXMAAA7EAAAOxAGVK"
         "w4bAAADgElEQVRYhe1XzUojSxQ+9Zd0fjBxLtzJjEJUsslCvMiAwmXAnU/hE/"
         "hCLn0C38CNgoLgWnCRRMyv9sXuJph0p6rOXRynbZL5MTOaGcFv1Tl1+tRXp7/6qsL29/"
         "d935+fn9/"
         "Z2YEp4fv+"
         "3t5eo9HY3NwsFArJIcYYPXied3JyUi6Xd3d3x3KegoODA8dxtNaDwcBay5Ol/"
         "0wgIiLSM/+9VKbF66Abd1cmf0wLxhjnHBG11pw/rjz+fEoppRTl/"
         "DJnAABJpdPpdLLiVGp2Xffy8vLm5mZyCBEHg8FgMHAc5+"
         "k1k0xyuVwURcYYay1QdwEgCALP84rF4tNZAoBSanV1tVardbtd13XjuBBCKQUANFM+"
         "n69WqxSZCp7n+b7vOA5xBQAppczn81LKfr8/"
         "Ld1MJrO9vV2pVIIgSMY551JKADDGIGI2m11aWvoJuvf391IIY0zcbkmdiKKo2WwuLi5OW"
         "5ExVqlUpn3rhzVJD51OJ4yipPQ5IgohAKDRaNze3jLGKDvG81L5FsZmZIz1er16vQ4A1l"
         "oKIuLDMaGU6vf75+fn/X5/Nvy+jyAITk9PgyBI9g5i3xVCSCnb7fbx8XG32/"
         "2tVKHVah0eHrqum06nx4YkIpLFkHG2223P80ql0vLycqFQmHbz/"
         "Qru7u6CIKjX6+"
         "12ezgcZjIZpdSYGqUxhohyzjnnjDFjTLPZ7HQ6Wut8Pj8ajWbAldQohDDGSCmz2SwAkH8"
         "l9SCNMRQiEGPKkFIOh8OXo5jsnNZaCMEYE0JM7m/"
         "7BdIaCwCUmsxgjJF3zgBJC4pPhHgo5hqGobRo0SDZGa2M1DzjW2VMNzZdCmqt6awBgFKp"
         "JMMwlFKS9SIiKZgLgYjsi9s9VmUA8SoQ4PlMebI71Fe0FowJw/"
         "DDwsLW1pZcWVlptVq0n6y1pAqj9VeLWvH4pZhlHJ/t/pm80MXngtYawxC1Xvj48d/"
         "Pn4vFIkPEWq12dHTk+74QIpVK0cri5SYVPIKRxQfGiivJnk3cxI/"
         "2vbWWMWatHY1G2VRq/"
         "dOnf9bXKe1BKHSkXVxcBEEQhmH8vrU2uW6Qjxd6AUKA+"
         "OrcP6F7rXX8ojEmnU7Pzc1Vq9W1tbWk97MxN7m6ujo7O/vPdXu9XhhF19fXs6HrOE4ul/"
         "vr3TvHcf5+/35jY6NcLk9aE5s0uT8Zr+O/Wow3ui+JN7oviTe6L4lXRvd/"
         "hyIurGaoHW4AAAAASUVORK5CYII=")},
    {"pBMSprinklerSlot_Night",
     bitmap_from_base64(
         "iVBORw0KGgoAAAANSUhEUgAAADoAAAAfCAIAAAA5uEn6AAAAAXNSR0IB2cksfwAAAAlwS"
         "FlzAAAOxAAADsQBlSsOGwAAAxNJREFUeJztlstLG0EcgPd/"
         "9CKC0RSf9apeDEhKYqKI1SxrXrt5PyVRI75O4smLSISexIMggnhQFEHw0vbrzGa7SWox1"
         "qQe8jHGzc5kft/89jeTKIlEYmVlJZVK/Wid+/t7v98/"
         "NjbGJBv1bNaga2RkxOv1MvgNIcrl8s7OTqVSKRQKuVxOYbpAIPCRdbe3t5mnq9s2XSb8r"
         "fvm2n14eFhYWBgdHdV1vWIDSylNpGw2y3o8Hs/"
         "76Mbj8eXl5WKx+N1GS7o9PT3T09Nzf4KkulyugYGBxcVFBr9yWrvJ/"
         "v7+1tZWqVTKCUzdaDR6d3fXqu7T09Pe3h7Jczgcn2wMDw9/"
         "FgwNDTmdzomJCXY3g1vVRYk88rjW1tZM3XQ6zaOkJC4vL1vVlVOfn5+"
         "f1FOtVr8JTk9PuT47O3t+fn79tJbG1dVVuVRaX18vFmu6/CWTSbJ7dHT0Bt02ITWOj4/"
         "Ro2rz+bypy1bIZDLo8ubm5sY+upPqzRGvr6+tGpCg+ksXKIZIJLK7u/"
         "v4+"
         "PgRdNmX1ABVSl6zNkxdKjgWi9GNMcv6v7qULK6kD6tGXSpB6pJgaUzaDw4OLi4u5FnRMd"
         "3b21uCEholNFCSVVunyz5LC1IC65o+ujilCx2BQIRDlFdC5+"
         "qRGwwxhYwyQopyiz4W1LysdpBpwrrZMBI3JPmKUPSoLo0bdKHYKWR2JM1LIpWyUIPBoEJ"
         "Fc4oZhsEtjOUgu3RnaHj0litWuOkCfjkoqqpijTR3ybZMM77Zpg+LdWSz+Vqr7/"
         "lHmnVlXuOxmBGJaKrKWaFpmsK3Oa5cyTST89TLJDigc2ZLZsw9+"
         "i40VKoMh4wRDOqqulkuawJF/pM/"
         "dMh0WBAKhcI1DBshI7Sqr8oWjUXj70dMEBUQlNzhgI8RDldPTrQainV1eHhIVczPz38Ru"
         "AWzs7NuO16322c2j8/jewF/"
         "63gEXgHR0UAGJa0exf6G1fBK9aiBgGtmZmpqqre3t8+Oo69v0Gz9g/"
         "2DL+BsnfHx8cnJSd/"
         "c3NelJerBkvmb7senq9tOurrtpKvbTrq67eQnO6gETaM0mrEAAAAASUVORK5CYII=")},
    {"pBMButtonE",
     bitmap_from_base64(
         "iVBORw0KGgoAAAANSUhEUgAAAB4AAAAuCAIAAACwKD5YAAAACXBIWXMAAA7EAAAOxA"
         "GVK"
         "w4bAAAAf0lEQVRYhWN89+4TA20AE43MHTUaE7DAWXfu3unp6yTPFDk5+"
         "aryGjTBoRkgdAlrNFBX3SAlJU2J0UMzQEaNHjV61GiSAM4ypKm1gRj9ifHJ5mYWWKW"
         "GWY"
         "CMFqqjRo8aPWo0EYDSQhUO0lIyjAyNkUWGZoDQ0GjG0X7jqNF4AQDPzBYvH6p8QQAA"
         "AAB"
         "JRU5ErkJggg==")}};

void APPLICATION::check_hive() {
  this->output->key_press("Escape");
  sleep(50);
  this->output->key_press("r");
  sleep(50);
  this->output->key_press("Return");
  this->machine_state->facing = FACES::Forward;
  this->machine_state->placed_sprinkler = false;
  sleep(5 * 1000);
  loop(5) {
    this->output->key_press("i");
    sleep(50);
  }
  sleep(50);
  this->output->key_press("comma");
  this->machine_state->facing = FACES::Forward;
  BITMAP pBMArea = this->input->capture_display(670, 970, 120, 100);
  bool found_hive_rotated =
      image_search(pBMArea, bitmaps["pBMSprinklerSlot_Day"], nullptr, nullptr,
                   0, 0, pBMArea.width, pBMArea.height, 30) ||
      image_search(pBMArea, bitmaps["pBMSprinklerSlot_Night"], nullptr, nullptr,
                   0, 0, pBMArea.width, pBMArea.height, 70);
  sleep(50);
  this->output->key_press("period");
  sleep(50);
  if (found_hive_rotated &&
      (image_search(pBMArea, bitmaps["pBMSprinklerSlot_Day"], nullptr, nullptr,
                    0, 0, pBMArea.width, pBMArea.height, 30) ||
       image_search(pBMArea, bitmaps["pBMSprinklerSlot_Night"], nullptr,
                    nullptr, 0, 0, pBMArea.width, pBMArea.height, 70))) {
    this->machine_state->at_hive = true;
    this->use_hotbar(HOTBAR_USE_METHOD::At_Hive);
#ifdef DEBUG
    std::cout << "FOUND HIVE" << std::endl;
#endif
    loop(5) {
      this->output->key_press("o");
      sleep(50);
    }
    return;
  }
  this->machine_state->at_hive = false;
}

bool APPLICATION::collect_timer_check() {
  return (since_boot() - this->machine_state->collecting_begin) <
         (this->settings->convert_every * 60);
}

void APPLICATION::action_drift_comp() {
  BITMAP pBMArea = this->input->capture_display(
      148, 98, this->input->width - 148, this->input->height - 148);
  u64 center_x = pBMArea.width / 2;
  u64 center_y = pBMArea.height / 2;

  u64 area_x_left_bound = center_x - 64;
  u64 area_y_top_bound = center_y - 64;

  u64 area_x_right_bound = center_x + 64;
  u64 area_y_bottom_bound = center_y + 64;

  if (image_search(pBMArea, bitmaps["pBMSupremeSaturator"], nullptr, nullptr,
                   area_x_left_bound, area_y_top_bound, area_x_right_bound,
                   area_y_bottom_bound, 60))
    return;
  int search_area_negate_x = 600;
  int search_area_negate_y = 300;
  int tolerance = 30;
  do {
    pBMArea = this->input->capture_display(148, 98, this->input->width - 148,
                                           this->input->height - 148);
    u64 x = 0;
    u64 y = 0;
    if (search_area_negate_x < 0 && search_area_negate_y < 0) {
      if (this->running) {
        this->machine_state->placed_sprinkler = false;
        this->place_sprinkler();
      }
#ifdef DEBUG
      std::cout << "FAIL_SAFE" << std::endl;
#endif
      return;
    }
    if (image_search(pBMArea, bitmaps["pBMSupremeSaturator"], &x, &y,
                     search_area_negate_x, search_area_negate_y,
                     pBMArea.width - search_area_negate_x,
                     pBMArea.height - search_area_negate_y, tolerance)) {
      u64 distance = std::sqrt(std::pow(int(x) - int(center_x), 2) +
                               std::pow(int(y) - int(center_y), 2));
      u64 flower_distance = std::max<u64>(1, distance / 50);
#ifdef DEBUG
      std::cout << "Raw Distance: " << distance
                << " | Approximated Tiles: " << flower_distance << std::endl;
#endif
      if (x <= area_x_left_bound && area_x_right_bound >= x &&
          y <= area_y_top_bound && area_y_bottom_bound >= y) {
        walk(flower_distance, forwards_left);
      }
      if (x >= area_x_left_bound && area_x_right_bound <= x &&
          y <= area_y_top_bound && area_y_bottom_bound >= y) {
        walk(flower_distance, forwards_right);
      }

      if (x <= area_x_left_bound && area_x_right_bound >= x &&
          y >= area_y_top_bound && area_y_bottom_bound <= y) {
        walk(flower_distance, backwards_left);
      }
      if (x >= area_x_left_bound && area_x_right_bound <= x &&
          y >= area_y_top_bound && area_y_bottom_bound <= y) {
        walk(flower_distance, backwards_right);
      }

      if (x <= area_x_left_bound && area_x_right_bound >= x) {
        walk(flower_distance, left);
      }
      if (x >= area_x_left_bound && area_x_right_bound <= x) {
        walk(flower_distance, right);
      }
      if (y <= area_y_top_bound && area_y_bottom_bound >= y) {
        walk(flower_distance, forwards);
      }
      if (y >= area_y_top_bound && area_y_bottom_bound <= y) {
        walk(flower_distance, backwards);
      }
    } else {
      if (search_area_negate_x > 0) {
        search_area_negate_x -= 100;
      }
      if (search_area_negate_y > 0) {
        search_area_negate_y -= 50;
      }
      tolerance += 15;
    }
    sleep(20);
  } while (this->running && collect_timer_check() &&
           search_area_negate_x >= 0 && search_area_negate_y >= 0 &&
           !image_search(pBMArea, bitmaps["pBMSupremeSaturator"], nullptr,
                         nullptr, area_x_left_bound, area_y_top_bound,
                         area_x_right_bound, area_y_bottom_bound, tolerance));
  if (this->running) {
    this->machine_state->placed_sprinkler = false;
    this->place_sprinkler();
  }
}

void APPLICATION::to_field() {
#ifdef DEBUG
  std::cout << "GOING TO FIELD" << std::endl;
#endif
  json field_data =
      this->raw_data->RAW_FIELDS_DATA
          [this->interface->FIELD_OPTIONS[this->settings->selected_field]];
  const std::string field_name = field_data["name"];
  auto it = fields_impl().find(field_name);
  if (it != fields_impl().end()) {
    it->second(false);
  }
}

void APPLICATION::to_red_cannon() {
  auto it = sub_macros().find("red_cannon");
  if (it != sub_macros().end()) {
    it->second();
  }
}

void APPLICATION::align_in_field() {
  const std::string position_name =
      this->interface->DIRECTION_OPTIONS[this->settings->direction];
  auto it = sub_macros().find(position_name);
  if (it != sub_macros().end()) {
    it->second();
  }
}

void APPLICATION::place_sprinkler() {
  this->output->key_press("1");
  this->machine_state->placed_sprinkler = true;
}

void APPLICATION::convert() {
  BITMAP pBMArea = this->input->capture_display(790, 70, 60, 60);
#ifdef DEBUG
  pBMArea.save_png("captured_E.png", 4);
#endif
  if (this->running && this->machine_state->at_hive) {
    if (!image_search(pBMArea, bitmaps["pBMButtonE"], nullptr, nullptr, 0, 0,
                      pBMArea.width, pBMArea.width, 12)) {
      this->machine_state->converting = false;
      return;
    }
  }
  this->machine_state->converting = true;
  this->output->key_press("e");
  this->use_hotbar(HOTBAR_USE_METHOD::On_Convert);
  while (this->running && this->machine_state->at_hive &&
         this->machine_state->converting) {
    sleep(1000) if (image_search(pBMArea, bitmaps["pBMButtonE"], nullptr,
                                 nullptr, 0, 0, pBMArea.width, pBMArea.width,
                                 12)) {
      this->machine_state->converting = true;
      pBMArea = this->input->capture_display(790, 70, 60, 60);
#ifdef DEBUG
      pBMArea.save_png("captured_E.png", 4);
#endif
      continue;
    }
    else {
      this->machine_state->converting = false;
      break;
    }
  }
}

void APPLICATION::finish_collect() {
  if (this->running && this->machine_state->collecting == false) {
    this->machine_state->at_hive = false;
    this->machine_state->converting = false;
    this->machine_state->collecting = false;
    this->machine_state->placed_sprinkler = false;
    this->machine_state->checked_mondo = false;
    this->machine_state->drift_comp_collection_tick = 0;
    if (this->settings->walk_to_hive == false) {
      this->output->key_press("Escape");
      sleep(50);
      this->output->key_press("r");
      sleep(50);
      this->output->key_press("Return");
      sleep(5 * 1000);
    } else {
      return;
    }
  }
}

void APPLICATION::align_mouse() { this->output->mouse_move(348, 128); }

bool APPLICATION::mondo() {
  if (this->running && this->settings->mondo) {
    if (get_utc_minute() > 15) {
      this->machine_state->checked_mondo = false;
      return false;
    }
    if (this->machine_state->checked_mondo) {
      return false;
    }
    this->machine_state->checked_mondo = true;
    this->machine_state->collecting = false;

    auto it = sub_macros().find("mondo");
    if (it != sub_macros().end()) {
      it->second();
      return true;
    }
    return false;
  }
  return false;
}

void APPLICATION::use_hotbar(HOTBAR_USE_METHOD method) {
  if (static_cast<HOTBAR_USE_METHOD>(this->settings->slot_2) == method) {
    this->output->key_press("2");
  }
  if (static_cast<HOTBAR_USE_METHOD>(this->settings->slot_3) == method) {
    this->output->key_press("3");
  }
  if (static_cast<HOTBAR_USE_METHOD>(this->settings->slot_4) == method) {
    this->output->key_press("4");
  }
  if (static_cast<HOTBAR_USE_METHOD>(this->settings->slot_5) == method) {
    this->output->key_press("5");
  }
  if (static_cast<HOTBAR_USE_METHOD>(this->settings->slot_6) == method) {
    this->output->key_press("6");
  }
  if (static_cast<HOTBAR_USE_METHOD>(this->settings->slot_7) == method) {
    this->output->key_press("7");
  }
}

void APPLICATION::step() {
  this->align_mouse();
  while (this->running && !this->machine_state->at_hive) {
    this->check_hive();
    wait_for(1000);
  }
  wait_for(1000);
  this->convert();
  this->align_mouse();
  wait_for(1000);
  this->machine_state->converting = false;
  this->machine_state->at_hive = false;
  this->to_red_cannon();
  this->align_mouse();
  if (this->mondo()) {
    this->use_hotbar(HOTBAR_USE_METHOD::On_Mondo);
    sleep(1000 * application_pointer->settings->mondo_wait);
    return;
  }
  this->to_field();
  this->align_mouse();
  wait_for(1000);
  this->align_in_field();
  this->align_mouse();
  wait_for(1000);
  if (!this->machine_state->collecting) {
#ifdef DEBUG
    std::cout << "COLLECTING" << std::endl;
#endif
    this->machine_state->collecting_begin = since_boot();
    this->machine_state->collecting = true;
    if (!this->machine_state->placed_sprinkler) {
      this->place_sprinkler();
    }
    this->use_hotbar(HOTBAR_USE_METHOD::Gather_Start);
    while (this->running && this->machine_state->collecting &&
           this->collect_timer_check()) {
      this->action_collect();
      this->align_mouse();
      if (this->mondo()) {
        this->use_hotbar(HOTBAR_USE_METHOD::On_Mondo);
        sleep(1000 * application_pointer->settings->mondo_wait);
        return;
      }
      if (this->settings->drift_comp) {
        this->machine_state->drift_comp_collection_tick++;
        if (this->machine_state->drift_comp_collection_tick >= 10) {
          this->machine_state->drift_comp_collection_tick = 0;
          this->action_drift_comp();
        }
      }

      wait_for(20);
    }
    this->machine_state->collecting = false;
#ifdef DEBUG
    std::cout << "COLLECTION ENDED" << std::endl;
#endif
  }
  this->align_mouse();
  this->finish_collect();
  sleep(1000);
}

void APPLICATION::start() {
  if (this->interface->is_waiting_for_start_key ||
      this->interface->is_waiting_for_stop_key) {
    return;
  }
  if (this->running)
    return;
  this->running = true;
  this->machine_state->at_hive = false;
  this->machine_state->converting = false;
  this->machine_state->collecting = false;
  this->machine_state->placed_sprinkler = false;
  this->machine_state->checked_mondo = false;
  std::thread([this]() {
    while (this->running) {
      this->step();
      wait_for(20);
    }
  }).detach();
}

void APPLICATION::stop() {
  if (this->interface->is_waiting_for_start_key ||
      this->interface->is_waiting_for_stop_key) {
    return;
  }
  for (int keycode = 8; keycode < 256; ++keycode) {
    XTestFakeKeyEvent(this->output->display, keycode, False, 0);
  }

  for (int button = 1; button <= 5; ++button) {
    XTestFakeButtonEvent(this->output->display, button, False, 0);
  }

  XFlush(this->output->display);
  this->running = false;
}

void APPLICATION::main() {
  this->interface->REHOOK_KEY_EVENT->connect(
      [this](const std::string &key, REHOOK_KEY_EVENT_TYPE kind) {
        switch (kind) {
        case REHOOK_KEY_EVENT_TYPE::START_KEY:
          this->input->unregister_hotkey(this->settings->start_key);
          this->input->register_hotkey(key,
                                       [this]() -> void { this->start(); });
          break;
        case REHOOK_KEY_EVENT_TYPE::STOP_KEY:
          this->input->unregister_hotkey(this->settings->stop_key);
          this->input->register_hotkey(key, [this]() -> void { this->stop(); });
          break;
        default:
          break;
        }
      });
  this->input->register_hotkey(this->settings->start_key,
                               [this]() -> void { this->start(); });
  this->input->register_hotkey(this->settings->stop_key,
                               [this]() -> void { this->stop(); });
  this->input->start();
}