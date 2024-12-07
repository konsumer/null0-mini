// host implementations for emscripten/wamr

#define FS_IMPLEMENTATION
#include "fs.h"

#define PNTR_LOAD_FILE fs_load_file
#define PNTR_SAVE_FILE fs_save_file

#ifdef EMSCRIPTEN
#define PNTR_PIXELFORMAT_RGBA
#else
#define PNTR_PIXELFORMAT_ARGB
#endif

#define PNTR_ENABLE_DEFAULT_FONT
#define PNTR_IMPLEMENTATION
#include "pntr.h"

#define CVECTOR_LOGARITHMIC_GROWTH
#include "cvector.h"

pntr_image* screen;
cvector_vector_type(pntr_image*) images = NULL;
cvector_vector_type(pntr_font*) fonts = NULL;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

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

  screen = pntr_new_image(SCREEN_WIDTH, SCREEN_HEIGHT);
  cvector_push_back(images, screen);
  cvector_push_back(fonts, pntr_load_font_default());

  #ifdef EMSCRIPTEN
    emscripten_set_main_loop(wasm_host_update, 60, false);
  #else
    #include <MiniFB.h>
    struct mfb_window* null0_window = mfb_open("null0", SCREEN_WIDTH, SCREEN_HEIGHT);
    mfb_set_target_fps(60);

    while(null0_window) {
      if (mfb_update_ex(null0_window, screen->data, SCREEN_WIDTH, SCREEN_HEIGHT) != STATE_OK) {
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
