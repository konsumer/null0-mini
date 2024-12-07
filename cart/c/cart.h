#include <stdlib.h>
#include <stdint.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// import function from host
#define HOST_FUNCTION(return_type, name, params) __attribute__((import_module("null0"), import_name(#name))) return_type name params;

// export function to host
#define CART_FUNCTION(n) __attribute__((export_name(n)))

// mem-management

CART_FUNCTION("malloc")
void* _wasm_host_malloc(size_t size) {
  return malloc(size);
}

CART_FUNCTION("free")
void _wasm_host_free(void* ptr) {
  free(ptr);
}

// callbacks
CART_FUNCTION("load")
void load();

CART_FUNCTION("update")
void update(uint64_t timeMS);

CART_FUNCTION("unload")
void unload();

typedef struct {
  int x;
  int y;
} Vector;

typedef unsigned int u32;
typedef int i32;

#define LIGHTGRAY  0xC8C8C8FF  // (200, 200, 200, 255)
#define GRAY       0x828282FF  // (130, 130, 130, 255)
#define DARKGRAY   0x505050FF  // (80, 80, 80, 255)
#define YELLOW     0xFDF900FF  // (253, 249, 0, 255)
#define GOLD       0xFFCB00FF  // (255, 203, 0, 255)
#define ORANGE     0xFFA100FF  // (255, 161, 0, 255)
#define PINK       0xFF6DC2FF  // (255, 109, 194, 255)
#define RED        0xE62937FF  // (230, 41, 55, 255)
#define MAROON     0xBE2137FF  // (190, 33, 55, 255)
#define GREEN      0x00E430FF  // (0, 228, 48, 255)
#define LIME       0x009E2FFF  // (0, 158, 47, 255)
#define DARKGREEN  0x00752CFF  // (0, 117, 44, 255)
#define SKYBLUE    0x66BFFFFF  // (102, 191, 255, 255)
#define BLUE       0x0079F1FF  // (0, 121, 241, 255)
#define DARKBLUE   0x0052ACFF  // (0, 82, 172, 255)
#define PURPLE     0xC87AFFFF  // (200, 122, 255, 255)
#define VIOLET     0x873CBEFF  // (135, 60, 190, 255)
#define DARKPURPLE 0x701F7EFF  // (112, 31, 126, 255)
#define BEIGE      0xD3B083FF  // (211, 176, 131, 255)
#define BROWN      0x7F6A4FFF  // (127, 106, 79, 255)
#define DARKBROWN  0x4C3F2FFF  // (76, 63, 47, 255)
#define WHITE      0xFFFFFFFF  // (255, 255, 255, 255)
#define BLACK      0x000000FF  // (0, 0, 0, 255)
#define BLANK      0x00000000  // (0, 0, 0, 0)
#define MAGENTA    0xFF00FFFF  // (255, 0, 255, 255)
#define RAYWHITE   0xF5F5F5FF  // (245, 245, 245, 255)

HOST_FUNCTION(void, trace, (char* string))
HOST_FUNCTION(void, draw_circle, (u32 centerX, u32 centerY, u32 radius, u32 color))
HOST_FUNCTION(void, clear, (u32 color))
HOST_FUNCTION(void, draw_image_tint, (u32 src, i32 posX, i32 posY, u32 tint))
HOST_FUNCTION(u32, new_image, (i32 width, i32 height, u32 color))
HOST_FUNCTION(void, draw_circle_on_image, (u32 destination, i32 centerX, i32 centerY, i32 radius, u32 color))
