#include "cart.h"
#include <math.h>

void update(uint64_t timeMS) {
  clear(BLACK);
  draw_circle(timeMS % 640 , sin(timeMS/100) * 200, 80, RED);
  draw_circle(sin(timeMS/1000) * 640, 220, 80, GREEN);
  draw_circle(cos(timeMS/1000) * 640, 350, 80, BLUE);
}

int main() {
  trace("cart (graphics): Hello from cart main.");
  return 0;
}