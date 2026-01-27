#include "imgui/imgui.h"
#include <cstdlib>
#include <unordered_map>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "utils.hpp"

BITMAP::BITMAP(u64 width, u64 height, std::vector<u8> pixels) {
  this->width = width;
  this->height = height;
  this->pixels = pixels;
}

RGB::RGB(u8 R, u8 G, u8 B, u8 A) {
  this->R = R;
  this->G = G;
  this->B = B;
  this->A = A;
}

BITMAP create_bitmap(u64 width, u64 height) {
  BITMAP bitmap(width, height, {});
  return bitmap;
};

void fill_bitmap(BITMAP &bitmap, u64 color) {
  u8 alpha = (color >> 24) & 0xFF;
  u8 red = (color >> 16) & 0xFF;
  u8 green = (color >> 8) & 0xFF;
  u8 blue = color & 0xFF;

  bitmap.pixels.clear();
  for (u64 index = 0; index < (bitmap.width * bitmap.height); ++index) {
    bitmap.pixels.push_back(red);
    bitmap.pixels.push_back(green);
    bitmap.pixels.push_back(blue);
    bitmap.pixels.push_back(alpha);
  }
}

BITMAP bitmap_from_base64(const std::string &base64_string) {
  std::vector<u8> bitmap;
  std::string chars =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  std::string encoded;
  for (char c : base64_string) {
    if (c != '=')
      encoded += c;
  }

  u64 encoded_size = encoded.size();
  for (u64 index = 0; index < encoded_size; index += 4) {
    u8 c0 = (index + 0 < encoded_size) ? chars.find(encoded[index + 0]) : 0;
    u8 c1 = (index + 1 < encoded_size) ? chars.find(encoded[index + 1]) : 0;
    u8 c2 = (index + 2 < encoded_size) ? chars.find(encoded[index + 2]) : 0;
    u8 c3 = (index + 3 < encoded_size) ? chars.find(encoded[index + 3]) : 0;

    u32 indicies = (c0 << 18) | (c1 << 12) | (c2 << 6) | c3;

    bitmap.push_back((indicies >> 16) & 0xFF);
    if (index + 2 < encoded_size)
      bitmap.push_back((indicies >> 8) & 0xFF);
    if (index + 3 < encoded_size)
      bitmap.push_back(indicies & 0xFF);
  }

  int width, height, channels;
  unsigned char *pixels = stbi_load_from_memory(bitmap.data(), bitmap.size(),
                                                &width, &height, &channels, 4);

  if (!pixels) {
    return BITMAP(0, 0, {});
  }

  std::vector<u8> bitmap_data(pixels, pixels + (width * height * 4));
  stbi_image_free(pixels);

  return BITMAP(width, height, bitmap_data);
}

RGB BITMAP::pixel_value_at(u64 x, u64 y) {
  u64 index = (y * this->width + x) * 4;
  u8 R = this->pixels[index];
  u8 G = this->pixels[index + 1];
  u8 B = this->pixels[index + 2];
  u8 A = this->pixels[index + 3];

  return RGB(R, G, B, A);
}

u64 query_performance_counter() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

u64 query_performance_frequency() { return 1000000000ULL; }

bool in_range(u64 number, u64 min_range, u64 max_range) {
  return number >= min_range && number <= max_range;
}

bool image_search(BITMAP &area, BITMAP &template_bitmap, u64 *found_x,
                  u64 *found_y, u64 x_start, u64 y_start, u64 x_end, u64 y_end,
                  u8 tolerance, search_direction direction, bool debug_mode) {
  if (x_end == UINT64_MAX)
    x_end = area.width;
  if (y_end == UINT64_MAX)
    y_end = area.height;

  if (template_bitmap.width == 0 || template_bitmap.height == 0)
    return false;
  u64 max_x = x_end - template_bitmap.width;
  u64 max_y = y_end - template_bitmap.height;
  if (direction == search_direction::top_left) {
    for (u64 y = y_start; y < max_y; ++y) {
      for (u64 x = x_start; x < max_x; ++x) {
        bool match = true;

        for (u64 template_y = 0; template_y < template_bitmap.height;
             ++template_y) {
          for (u64 template_x = 0; template_x < template_bitmap.width;
               ++template_x) {
            RGB area_pixel =
                area.pixel_value_at(x + template_x, y + template_y);
            RGB template_pixel =
                template_bitmap.pixel_value_at(template_x, template_y);

            if (template_pixel.A < 1)
              continue;

            if ((std::abs(int(area_pixel.R) - int(template_pixel.R))) >
                    tolerance ||
                (std::abs(int(area_pixel.G) - int(template_pixel.G))) >
                    tolerance ||
                (std::abs(int(area_pixel.B) - int(template_pixel.B))) >
                    tolerance) {
              match = false;
              break;
            } else {
              if (debug_mode) {
                u64 index =
                    ((y + template_y) * area.width + (x + template_x)) * 4;
                area.pixels[index] = 255;
                area.pixels[index + 1] = 0;
                area.pixels[index + 2] = 0;
              }
            }
          }
        }

        if (debug_mode) {
          u64 index = (y * area.width + x) * 4;
          area.pixels[index] = 255;
          area.pixels[index + 1] = 0;
          area.pixels[index + 2] = 0;
        }

        if (match) {
          if (found_x)
            *found_x = x;
          if (found_y)
            *found_y = y;
          return true;
        }
      }
    }
  } else if (direction == search_direction::bottom_left) {
    for (i64 y = max_y; y >= y_start; --y) {
      for (u64 x = x_start; x < max_x; ++x) {
        bool match = true;

        for (u64 template_y = 0; template_y < template_bitmap.height;
             ++template_y) {
          for (u64 template_x = 0; template_x < template_bitmap.width;
               ++template_x) {
            RGB area_pixel =
                area.pixel_value_at(x + template_x, y + template_y);
            RGB template_pixel =
                template_bitmap.pixel_value_at(template_x, template_y);
            if (template_pixel.A < 1)
              continue;
            if ((std::abs(int(area_pixel.R) - int(template_pixel.R))) >
                    tolerance ||
                (std::abs(int(area_pixel.G) - int(template_pixel.G))) >
                    tolerance ||
                (std::abs(int(area_pixel.B) - int(template_pixel.B))) >
                    tolerance) {
              match = false;
              break;
            }
          }
        }

        if (match) {
          if (found_x)
            *found_x = x;
          if (found_y)
            *found_y = y;
          return true;
        }
      }
    }
  }

  return false;
}

bool BITMAP::save_png(const std::string &filename, int channels) {
  int stride_in_bytes = width * channels;
  int result = stbi_write_png(filename.c_str(), width, height, channels,
                              pixels.data(), stride_in_bytes);

  return result != 0;
}

u64 since_boot() {
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + ts.tv_nsec / 1e9;
}

u64 get_utc_minute() {
  using namespace std::chrono;

  auto now = system_clock::now();
  std::time_t t = system_clock::to_time_t(now);

  std::tm utc{};
  gmtime_r(&t, &utc);

  return utc.tm_min;
}

std::unordered_map<std::string, std::vector<u8>> FIELD_SIZES = {
    {"pine_tree_forest", {23, 31}}};