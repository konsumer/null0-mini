// host implementations for emscripten/wamr

#ifdef EMSCRIPTEN
#define PNTR_PIXELFORMAT_RGBA
#else
#define PNTR_PIXELFORMAT_ARGB
#endif

#define PNTR_IMPLEMENTATION
#include "pntr.h"

pntr_image* screen;

#include "host.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <CART_FILE>\n", argv[0]);
    return 1;
  }

  if (!fs_init(argv[1])) {
    fprintf(stderr, "Could not initialize filesystem with %s\n", argv[1]);
    return 1;
  }

  DetectFileType d = fs_detect_type_real(argv[1]);

  if (d == FILE_TYPE_UNKNOWN) {
    fprintf(stderr, "Invalid cart %s\n", argv[1]);
    return 1;
  }

  char* cartFilename = d == FILE_TYPE_WASM ? basename(argv[1]) : "main.wasm";
  if (!wasm_host_load(cartFilename)) {
    fprintf(stderr, "Could not start cart-host with %s\n", argv[1]);
    return 1;
  }

  screen = pntr_new_image(640, 480);

  pntr_draw_circle_fill(screen, 100, 100, 80, PNTR_RED);
  pntr_draw_circle_fill(screen, 200, 100, 80, PNTR_GREEN);
  pntr_draw_circle_fill(screen, 300, 100, 80, PNTR_BLUE);

  #ifdef EMSCRIPTEN
    emscripten_set_main_loop(wasm_host_update, 60, false);
  #else
    #include <MiniFB.h>
    struct mfb_window* null0_window = mfb_open("null0", 640, 480);
    while(null0_window) {
      if (mfb_update_ex(null0_window, screen->data, 640, 480 ) != STATE_OK) {
        null0_window = NULL;
      } else {
        wasm_host_update();
        mfb_wait_sync(null0_window);
      }
    }
  #endif

  wasm_host_unload();
  return 0;
}
