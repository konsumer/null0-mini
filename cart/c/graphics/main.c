#include "cart.h"
#include <math.h>

Vector red;
Vector green;
Vector blue;
u32 circle;

int f = 0;
int ballSize = SCREEN_WIDTH/10;

void update(uint64_t timeMS) {
  f++;
  red.x = (f % SCREEN_WIDTH + (ballSize/2)) - ballSize;
  green.x = SCREEN_WIDTH - red.x;
  blue.x = ((f*4) % SCREEN_WIDTH + (ballSize/2)) - ballSize;
  blue.y = (sin(f/4) * (SCREEN_HEIGHT/2)) + (SCREEN_HEIGHT/2);

  clear(BLACK);
  draw_image_tint(circle, red.x, red.y, RED);
  draw_image_tint(circle, green.x, green.y, GREEN);
  draw_image_tint(circle, blue.x, blue.y, BLUE);
}

int main() {
  trace("cart (graphics): Hello from cart main.");

  // positions
  red = (Vector){.x=0, .y=0};
  green = (Vector){.x=0, .y=(SCREEN_HEIGHT/3)};
  blue = (Vector){.x=0, .y=(SCREEN_HEIGHT/3) * 2};

  // for efficiency, draw the circles once
  circle = new_image(ballSize*2, ballSize*2, BLANK);
  draw_circle_on_image(circle, ballSize, ballSize, ballSize, WHITE);

  return 0;
}