#pragma once
// #define DEBUG

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <thread>
#include <unordered_map>
#include <vector>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

inline void wait_for(int ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

class RGB {
public:
  u8 R = 0;
  u8 G = 0;
  u8 B = 0;
  u8 A = 0;
  RGB(u8 R, u8 G, u8 B, u8 A);
};

class BITMAP {
public:
  u64 width = 0;
  u64 height = 0;
  std::vector<u8> pixels;
  BITMAP() : width(0), height(0), pixels() {};
  BITMAP(u64 width, u64 height, std::vector<u8> pixels);
  RGB pixel_value_at(u64 x, u64 y);
  bool save_png(const std::string &filename, int channels);
};

class X11_INPUT {
private:
  Display *display;
  std::thread listenerThread;
  std::atomic<bool> running{false};

  std::unordered_map<KeySym, std::function<void()>> hotkeys;

public:
  u64 width = 0;
  u64 height = 0;
  X11_INPUT() {
    display = XOpenDisplay(nullptr);
    if (!display) {
      std::cerr << "Cannot open X display" << std::endl;
    }
    Screen *screen = DefaultScreenOfDisplay(display);
    this->width = screen->width;
    this->height = screen->height;
  }

  ~X11_INPUT() {
    stop();
    if (display) {
      XCloseDisplay(display);
    }
  }

  void register_hotkey(const std::string &key_name,
                       std::function<void()> callback) {
    if (!display)
      return;

    KeySym keySym = XStringToKeysym(key_name.c_str());
    if (keySym == NoSymbol) {
      std::cerr << "Invalid key name: " << key_name << std::endl;
      return;
    }

    KeyCode keyCode = XKeysymToKeycode(display, keySym);
    Window root = DefaultRootWindow(display);

    XGrabKey(display, keyCode, AnyModifier, root, False, GrabModeAsync,
             GrabModeAsync);

    hotkeys[keySym] = callback;
  }

  void unregister_hotkey(const std::string &key_name) {
    KeySym keySym = XStringToKeysym(key_name.c_str());
    if (keySym == NoSymbol) {
      std::cerr << "Invalid key name: " << key_name << std::endl;
      return;
    }

    hotkeys.erase(keySym);
  }

  void start() {
    if (running || !display)
      return;

    running = true;
    listenerThread = std::thread(&X11_INPUT::listen_loop, this);
  }

  void stop() {
    running = false;
    if (listenerThread.joinable()) {
      listenerThread.join();
    }
  }

  BITMAP capture_display(uint64_t x_start, uint64_t y_start, uint64_t x_size,
                         uint64_t y_size) {
    Window root = DefaultRootWindow(display);

    XShmSegmentInfo shminfo;
    XImage *img = XShmCreateImage(display, DefaultVisual(display, 0), 24,
                                  ZPixmap, nullptr, &shminfo, x_size, y_size);
    if (!img) {
      std::cerr << "XShmCreateImage failed\n";
      return BITMAP(0, 0, {});
    }

    shminfo.shmid = shmget(IPC_PRIVATE, img->bytes_per_line * img->height,
                           IPC_CREAT | 0777);
    shminfo.shmaddr = (char *)shmat(shminfo.shmid, nullptr, 0);
    shminfo.readOnly = False;
    img->data = shminfo.shmaddr;

    if (!XShmAttach(display, &shminfo)) {
      std::cerr << "XShmAttach failed\n";
      return BITMAP(0, 0, {});
    }

    if (!XShmGetImage(display, root, img, x_start, y_start, AllPlanes)) {
      std::cerr << "XShmGetImage failed\n";
      return BITMAP(0, 0, {});
    }

    std::vector<uint8_t> rgb_buffer(x_size * y_size * 4);
    for (int y = 0; y < img->height; ++y) {
      for (int x = 0; x < img->width; ++x) {
        unsigned long pixel = XGetPixel(img, x, y);
        rgb_buffer[(y * img->width + x) * 4 + 0] =
            (pixel & img->red_mask) >> 16;
        rgb_buffer[(y * img->width + x) * 4 + 1] =
            (pixel & img->green_mask) >> 8;
        rgb_buffer[(y * img->width + x) * 4 + 2] = (pixel & img->blue_mask);
        rgb_buffer[(y * img->width + x) * 4 + 3] = 255;
      }
    }

    XShmDetach(display, &shminfo);
    XDestroyImage(img);
    shmdt(shminfo.shmaddr);
    shmctl(shminfo.shmid, IPC_RMID, nullptr);

    return BITMAP(x_size, y_size, std::move(rgb_buffer));
  }

private:
  void listen_loop() {
    XEvent event;

    while (running) {
      if (XPending(display) > 0) {
        XNextEvent(display, &event);

        if (event.type == KeyPress) {
          KeySym keySym = XLookupKeysym(&event.xkey, 0);

          auto it = hotkeys.find(keySym);
          if (it != hotkeys.end()) {
            it->second();
          }
        }
      }

      wait_for(10);
    }
  }
};

class X11_OUTPUT {
public:
  Display *display;

public:
  X11_OUTPUT() {
    display = XOpenDisplay(nullptr);
    if (!display) {
      std::cerr << "Cannot open X display" << std::endl;
    }
  }

  ~X11_OUTPUT() {
    if (display) {
      XCloseDisplay(display);
    }
  }

  void key_down(const std::string &key_name) {
    if (!display)
      return;

    KeySym keySym = XStringToKeysym(key_name.c_str());
    if (keySym == NoSymbol) {
      std::cerr << "Invalid key name: " << key_name << std::endl;
      return;
    }

    KeyCode keyCode = XKeysymToKeycode(display, keySym);
    XTestFakeKeyEvent(display, keyCode, True, CurrentTime);
    XFlush(display);
  }

  void key_up(const std::string &key_name) {
    if (!display)
      return;

    KeySym keySym = XStringToKeysym(key_name.c_str());
    if (keySym == NoSymbol) {
      std::cerr << "Invalid key name: " << key_name << std::endl;
      return;
    }

    KeyCode keyCode = XKeysymToKeycode(display, keySym);
    XTestFakeKeyEvent(display, keyCode, False, CurrentTime);
    XFlush(display);
  }

  void key_press(const std::string &key_name, int delayMs = 50) {
    key_down(key_name);
    std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    key_up(key_name);
  }

  void mouse_down(int button = 1) {
    if (!display)
      return;

    XTestFakeButtonEvent(display, button, True, CurrentTime);
    XFlush(display);
  }

  void mouse_up(int button = 1) {
    if (!display)
      return;

    XTestFakeButtonEvent(display, button, False, CurrentTime);
    XFlush(display);
  }

  void mouse_press(int button = 1, int delayMs = 50) {
    mouse_down(button);
    std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    mouse_up(button);
  }

  void mouse_move(int x, int y) {
    if (!display)
      return;

    XTestFakeMotionEvent(display, -1, x, y, CurrentTime);
    XFlush(display);
  }
};

template <typename> class SIGNAL;

template <typename R, typename... Args> class SIGNAL<R(Args...)> {
private:
  std::vector<std::function<R(Args...)>> hooks;

public:
  void connect(std::function<R(Args...)> callback) {
    hooks.push_back(std::move(callback));
  }
  void fire(Args... args) {
    for (auto &cb : hooks)
      cb(args...);
  }
};

enum class search_direction : u8 {
  top_left = 0,
  top_right = 1,
  bottom_left = 2,
  bottom_right = 3
};

BITMAP create_bitmap(u64 width, u64 height);
void fill_bitmap(BITMAP &bitmap, u64 color);
BITMAP bitmap_from_base64(const std::string &base64_string);
bool image_search(BITMAP &area, BITMAP &template_bitmap, u64 *found_x,
                  u64 *found_y, u64 x_start, u64 y_start, u64 x_end, u64 y_end,
                  u8 tolerance,
                  search_direction direction = search_direction::top_left,
                  bool debug_mode = false);
u64 query_performance_counter();
u64 query_performance_frequency();
u64 since_boot();
u64 get_utc_minute();

extern std::unordered_map<std::string, std::vector<u8>> FIELD_SIZES;