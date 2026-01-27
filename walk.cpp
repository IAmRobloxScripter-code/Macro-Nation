#include "walk.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cstddef>
#include <string>
static std::vector<std::string> bears = {"Brown", "Black",   "Panda", "Polar",
                                         "Gummy", "Science", "Mother"};
static std::unordered_map<u8, BITMAP> buff_characters = {
    {0, bitmap_from_base64("iVBORw0KGgoAAAANSUhEUgAAAAgAAAAKCAAAAACsrEBcAAAAAnR"
                           "STlMAAHaTzTgAAAArSURBVHgBY2Rg+"
                           "MzAwMALxCAaQoDBZyYYmwlMYmXAAFApWPVnBkYIi5cBAJNvCLCT"
                           "FAy9AAAAAElFTkSuQmCC")},
    {1,
     bitmap_from_base64(
         "iVBORw0KGgoAAAANSUhEUgAAAAIAAAAMCAAAAABt1zOIAAAAAnRSTlMAAHaTzTgAAAACY"
         "ktHRAD/h4/"
         "MvwAAABZJREFUeAFjYPjM+JmBgeEzEwMDLgQAWo0C7U3u8hAAAAAASUVORK5CYII=")},
    {2, bitmap_from_base64(
            "iVBORw0KGgoAAAANSUhEUgAAAAQAAAALCAAAAAB9zHN3AAAAAnRSTlMAAHaTzTgAAA"
            "BCSURBVHgBATcAyP8BAPMAAADzAAAAAAAAAAAAAAAAAAAAAAAAAAAAAPMAAADzAAAA"
            "8wAAAPMAAAAB8wAAAAIAAAAAtc8GqohTl5oAAAAASUVORK5CYII=")},
    {3, bitmap_from_base64(
            "iVBORw0KGgoAAAANSUhEUgAAAAQAAAAKCAAAAAC2kKDSAAAAAnRSTlMAAHaTzTgAAA"
            "A9SURBVHgBATIAzf8BAPMAAAAAAAAAAAAAAAAAAAAAAAAAAADzAAAAAAAAAAAAAAAA"
            "AAAAAPMAAAABAPMAAFILA8/B68+8AAAAAElFTkSuQmCC")},
    {4,
     bitmap_from_base64("iVBORw0KGgoAAAANSUhEUgAAAAQAAAAGCAAAAADBUmCpAAAAAnRSTl"
                        "MAAHaTzTgAAAApSURBVHgBAR4A4f8AAAAA8wAAAAAAAAAA8wAAAPMA"
                        "AALzAAAAAfMAAABBtgTDARckPAAAAABJRU5ErkJggg==")},
    {5, bitmap_from_base64(
            "iVBORw0KGgoAAAANSUhEUgAAAAQAAAALCAAAAAB9zHN3AAAAAnRSTlMAAHaTzTgAAA"
            "BCSURBVHgBATcAyP8B8wAAAAIAAAAAAPMAAAACAAAAAAHzAAAAAAAAAAAAAAAAAAAA"
            "AAAAAAAAAAAAAAAAAAHzAAAAgmID1KbRt+YAAAAASUVORK5CYII=")},
    {6, bitmap_from_base64(
            "iVBORw0KGgoAAAANSUhEUgAAAAQAAAAJCAAAAAAwBNJ8AAAAAnRSTlMAAHaTzTgAAA"
            "A4SURBVHgBAS0A0v8AAAAA8wAAAPMAAADzAAACAAAAAAEA8wAAAPPzAAAA8wAAAAAA"
            "8wAAAQAA8wC5oAiQ09KYngAAAABJRU5ErkJggg==")},
    {7, bitmap_from_base64(
            "iVBORw0KGgoAAAANSUhEUgAAAAQAAAAMCAAAAABgyUPPAAAAAnRSTlMAAHaTzTgAAA"
            "BHSURBVHgBATwAw/"
            "8B8wAAAAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
            "8wIAAAAAAgAAAABDdgHu70cIeQAAAABJRU5ErkJggg==")},
    {8, bitmap_from_base64(
            "iVBORw0KGgoAAAANSUhEUgAAAAQAAAAKCAAAAAC2kKDSAAAAAnRSTlMAAHaTzTgAAA"
            "A9SURBVHgBATIAzf8BAADzAAAA8wAAAgAAAAABAPMAAAEAAPMAAADzAAAAAAAAAADz"
            "AAAAAADzAAABAADzALv5B59oKTe0AAAAAElFTkSuQmCC")},
    {9, bitmap_from_base64(
            "iVBORw0KGgoAAAANSUhEUgAAAAQAAAAKCAAAAAC2kKDSAAAAAnRSTlMAAHaTzTgAAA"
            "A9SURBVHgBATIAzf8BAADzAAAA8wAAAPMAAAAAAPMAAAEAAPMAAAAAAAAAAAAAAAAA"
            "AAAAAAAAAAAAAAAA87TcBbXcfy3eAAAAAElFTkSuQmCC")},
};

static std::unordered_map<std::string, BITMAP> bitmaps;

void init_create_bitmaps() {
  bitmaps["pBMHaste"] = create_bitmap(10, 1);
  fill_bitmap(bitmaps["pBMHaste"], 0xfff0f0f0);
  bitmaps["pBMMelody"] = create_bitmap(3, 2);
  fill_bitmap(bitmaps["pBMMelody"], 0xff242424);
  bitmaps["pBMSnowflake"] = create_bitmap(1, 1);
  fill_bitmap(bitmaps["pBMSnowflake"], 0xffc0d0d8);
  bitmaps["pBMBearBrown"] = bitmap_from_base64(
      "iVBORw0KGgoAAAANSUhEUgAAAAwAAAABBAMAAAAYxVIKAAAAD1BMVEUwLi1STEihfVWzpZbQ"
      "vKTt7OCuAAAAEklEQVR4AQEHAPj/ACJDEAE0IgLvAM1oKEJeAAAAAElFTkSuQmCC");
  bitmaps["pBMBearBlack"] =
      bitmap_from_base64("iVBORw0KGgoAAAANSUhEUgAAAA4AAAABBAMAAAAcMII3AAAAFVBMV"
                         "EUwLi1TTD9lbHNmbXN5enW5oXHQuYJDhTsuAAAAE0lEQVR4AQEIAP"
                         "f/ACNGUQAVZDIFbwFmjB55HwAAAABJRU5ErkJggg==");
  bitmaps["pBMBearPanda"] = bitmap_from_base64(
      "iVBORw0KGgoAAAANSUhEUgAAABAAAAABBAMAAAAlVzNsAAAAGFBMVEUwLi1VU1G9u7m/"
      "vLXAvbbPzcXg3dfq6OXkYMPeAAAAFElEQVR4AQEJAPb/"
      "AENWchABJ2U0CO4B3TmcTKkAAAAASUVORK5CYII=");
  bitmaps["pBMBearPolar"] =
      bitmap_from_base64("iVBORw0KGgoAAAANSUhEUgAAAA4AAAABBAMAAAAcMII3AAAAElBMV"
                         "EUwLi1JSUqOlZy0vMbY2dnc3NtuftTJAAAAE0lEQVR4AQEIAPf/"
                         "AFVDIQASNFUFhQFVdZ1AegAAAABJRU5ErkJggg==");
  bitmaps["pBMBearGummy"] = bitmap_from_base64(
      "iVBORw0KGgoAAAANSUhEUgAAAA4AAAABBAMAAAAcMII3AAAAFVBMVEWYprGDrKWisd+hst+"
      "ctNtFyJ4xz5uqDngAAAAAE0lEQVR4AQEIAPf/"
      "ACNAFWZRBDIFqwFmOuySwwAAAABJRU5ErkJggg==");
  bitmaps["pBMBearScience"] = bitmap_from_base64(
      "iVBORw0KGgoAAAANSUhEUgAAAA4AAAABBAMAAAAcMII3AAAAFVBMVEUwLi1TTD+"
      "zjUy0jky8l1W5oXHevny+g95vAAAAE0lEQVR4AQEIAPf/"
      "ACNGUQAVZDIFbwFmjB55HwAAAABJRU5ErkJggg==");
  bitmaps["pBMBearMother"] = bitmap_from_base64(
      "iVBORw0KGgoAAAANSUhEUgAAABAAAAABBAMAAAAlVzNsAAAAJFBMVEVBNRlDNxtTRid8b0av"
      "oG69r22+sG7Qw4PRw4Te0Jbk153m2Z5VNHxxAAAAFElEQVR4AQEJAPb/"
      "AFVouTECSnZVDPsCv+2QpmwAAAAASUVORK5CYII=");
#ifdef DEBUG
  for (auto v : bitmaps) {
    v.second.save_png(v.first + ".png", 4);
  }

  for (auto v : buff_characters) {
    v.second.save_png(std::to_string(v.first) + "_digit.png", 4);
  }
#endif
}
static u64 y_offset = 10;

void WALK::walk(u64 tiles, bool *running, bool apply_speed) {
  static const u64 freq = query_performance_frequency();

  double distance = tiles * 4.0;
  double delta = 0.0;

  u64 s = query_performance_counter();
  if (apply_speed) {
    this->calculate_walkspeed();
  } else {
    this->walkspeed = this->base_walkspeed;
  }
  u64 f = query_performance_counter();

  delta += this->walkspeed * double(f - s) / double(freq);

  while (delta < distance and *running == true) {
    double old_speed = this->walkspeed;
    s = f;
    if (apply_speed) {
      this->calculate_walkspeed();
    } else {
      this->walkspeed = this->base_walkspeed;
    }
    f = query_performance_counter();

    double avg_speed = (old_speed + this->walkspeed) / 2.0;
    delta += avg_speed * double(f - s) / double(freq);
  }
}

// void WALK::calculate_walkspeed() {
//   BITMAP pBMArea = this->input->capture_display(
//       0, 48 + y_offset, this->input->width, 30 + y_offset);
//   u16 x = 0;
//   u16 x1 = 0;
//   u16 y1 = 0;
//   u8 haste = 0;
//   for (u8 index = 0; index < 3; ++index) {
//     u64 found_x = 0, y = 0;
//     if (!image_search(pBMArea, bitmaps["pBMHaste"], &found_x, &y, x, 14,
//                       pBMArea.width, pBMArea.height, 6,
//                       search_direction::top_left))
//       break;

//     x = found_x;
//     if (!image_search(pBMArea, bitmaps["pBMMelody"], nullptr, nullptr, x + 2,
//     0,
//                       x + (pBMArea.height - bitmaps["pBMHaste"].width + 2),
//                       y, 24, search_direction::top_left)) {
//       haste++;
//       if (haste == 1) {
//         x1 = x;
//         y1 = y;
//       }
//     }

//     x += pBMArea.height - bitmaps["pBMHaste"].width;
//   }

//   bool coconut_haste = haste == 2;

//   if (haste != 0) {
//     bool found = false;

//     for (u8 index = 1; index <= 9; ++index) {
//       if (image_search(pBMArea, buff_characters[10 - index], nullptr,
//       nullptr,
//                        x1 + 2 + 14, std::max<u64>(0, pBMArea.height - 18),
//                        x1 + 2 * (pBMArea.height - 14), pBMArea.height - 1, 0,
//                        search_direction::top_left, false)) {
//         haste = (index == 9) ? 10 : 10 - index;
//         found = true;
//         break;
//       }
//     }

//     if (!found) {
//       haste = 1;
//     }
//   }

//   bool bear = false;
//   for (const std::string &morph : bears) {
//     bear = image_search(pBMArea, bitmaps["pBMBear" + morph], nullptr,
//     nullptr,
//                         0, 25 + y_offset, pBMArea.width, 29 + y_offset, 40);
//     if (bear) {
//       // std::cout << morph << std::endl;
//       break;
//     }
//   }

//   this->walkspeed = (this->base_walkspeed + (coconut_haste * 10) + (bear *
//   4)) *
//                     (1 + haste * 0.1);
// #ifdef DEBUG
//   std::cout << "coconut: " << coconut_haste << std::endl;
//   std::cout << "bear: " << bear << std::endl;
//   std::cout << "haste stacks: " << (int)haste << std::endl;
//   std::cout << "walkspeed: " << this->walkspeed << std::endl;
//   pBMArea.save_png("capture.png", 4);
// #endif
// }

void WALK::calculate_walkspeed() {
  BITMAP pBMArea = this->input->capture_display(
      0, 48 + y_offset, this->input->width, 30 + y_offset);
  u8 haste = 0;
  u64 x_haste = 0;
  bool bear = false;
  for (u64 tile = 0; tile < pBMArea.width / 38; ++tile) {
    u64 tile_start_x = tile * 38;
    u64 tile_end_x = (tile * 38) + 38;

    if (haste < 2 &&
        image_search(pBMArea, bitmaps["pBMHaste"], nullptr, nullptr,
                     tile_start_x, 0, tile_end_x, 38, 6)) {
      if (!image_search(pBMArea, bitmaps["pBMMelody"], nullptr, nullptr,
                        tile_start_x, 0, tile_end_x, 38, 12) &&
          !image_search(pBMArea, bitmaps["pBMSnowflake"], nullptr, nullptr,
                        tile_start_x, 0, tile_end_x, 38, 3)) {
        haste++;
        if (haste == 1) {
          x_haste = tile;
        }
      }
    }

    if (!bear) {
      for (const std::string &morph : bears) {
        if (image_search(pBMArea, bitmaps["pBMBear" + morph], nullptr, nullptr,
                         tile_start_x, 35, tile_end_x, 39, 40)) {
          bear = true;
          break;
        }
      }
    }
  }

  if (haste != 0) {
    u64 haste_tile_start_x = x_haste * 38;
    u64 haste_tile_end_x = (x_haste * 38) + 38;

    bool found = false;

    for (u8 index = 1; index <= 9; ++index) {
      if (image_search(pBMArea, buff_characters[10 - index], nullptr, nullptr,
                       haste_tile_start_x +
                           ((haste_tile_end_x - haste_tile_start_x) / 4),
                       std::max<u64>(0, pBMArea.height - 18), haste_tile_end_x,
                       pBMArea.height - 1, 0)) {
        haste = (index == 9) ? 10 : 10 - index;
        found = true;
        break;
      }
    }

    if (!found) {
      haste = 1;
    }
  }
  bool coconut_haste = haste == 2;
  this->walkspeed = (this->base_walkspeed + (coconut_haste * 10) + (bear * 4)) *
                    (1 + haste * 0.1);
#ifdef DEBUG
  if (coconut_haste || bear || haste > 0 ||
      this->walkspeed > this->base_walkspeed)
    std::cout << "--------------------------------------------------"
              << std::endl;
  if (coconut_haste)
    std::cout << "coconut: " << coconut_haste << std::endl;
  if (bear)
    std::cout << "bear: " << bear << std::endl;
  if (haste > 0)
    std::cout << "haste stacks: " << (int)haste << std::endl;
  if (this->walkspeed > this->base_walkspeed)
    std::cout << "walkspeed: " << this->walkspeed << std::endl;
  // pBMArea.save_png("capture.png", 4);
#endif
}
