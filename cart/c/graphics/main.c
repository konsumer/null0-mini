#include "cart.h"
#include <math.h>
#include <stdio.h>

Vector red;
Vector green;
Vector blue;
u32 circle;
u32 logo;
u32 duck;

int f = 0;
int ballSize = SCREEN_WIDTH/10;
char* fpsText;

void update(uint64_t timeMS) {
  f++;
  red.x = f % (SCREEN_WIDTH + ballSize) - ballSize;
  green.x = SCREEN_WIDTH - red.x;
  blue.x = ((f*4) % SCREEN_WIDTH + (ballSize)) - ballSize;
  blue.y = (sin(f/4) * (SCREEN_HEIGHT/2)) + (SCREEN_HEIGHT/2);

  clear(SKYBLUE);
  draw_rectangle(0, 350, 640, 80, BROWN);
  draw_image_tint(circle, red.x, red.y, RED);
  draw_image_tint(circle, green.x, green.y, GREEN);
  draw_image_tint(circle, blue.x, blue.y, BLUE);
  draw_image(duck, SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT - 300);
  draw_image(logo, 10, SCREEN_HEIGHT - 220);
  sprintf(fpsText, "%.2f", get_fps());
  draw_text(0, fpsText, 10, 10, WHITE);
}

int main() {
  trace("cart (graphics): Hello from cart main.");

  logo = load_image("logo.png");

  // positions
  red = (Vector){.x=0, .y=0};
  green = (Vector){.x=0, .y=(SCREEN_HEIGHT/3)};
  blue = (Vector){.x=0, .y=(SCREEN_HEIGHT/3) * 2};

  // for efficiency, draw things once on images
  // this is a way to create tiles/sprites without images!
  // you can even save_image(duck, "coolduck.png") to save something you made.

  circle = new_image(ballSize*2, ballSize*2, BLANK);
  draw_circle_on_image(circle, ballSize, ballSize, ballSize, WHITE);

  duck = new_image(200, 300, BLANK);
  draw_rectangle_on_image(duck, 75, 90, 50, 75, MAROON);
  draw_circle_on_image(duck, 100, 50, 50, YELLOW);
  draw_rectangle_on_image(duck, 75, 160, 50, 75, DARKBLUE);
  draw_line_on_image(duck, 100, 180, 100, 230, BLACK);
  draw_circle_on_image(duck, 80, 30, 10, BLACK);
  draw_circle_on_image(duck, 110, 30, 10, BLACK);
  draw_ellipse_on_image(duck, 100, 70, 50, 10, ORANGE);
  draw_line_on_image(duck, 55, 70, 145, 70, RED);
  draw_text_on_image(duck, 0, "quack!", 140, 0, WHITE);


  // // shirt
  // draw_rectangle(270, 320, 100, 100, GREEN);
  // draw_line(320, 340, 320, 420, BLUE);

  // // pants
  // draw_rectangle(270, 360, 100, 100, BLUE);
  // draw_line(320, 380, 320, 480, BLACK);
    
  // // head
  // draw_circle(320, 240, 100, YELLOW);

  // // eyes
  // draw_circle(280, 200, 10, BLACK);
  // draw_circle(360, 200, 10, BLACK);

  // // bill
  // draw_ellipse(320, 260, 50, 10, ORANGE);
  // draw_line(280, 260, 360, 260, RED);

  return 0;
}