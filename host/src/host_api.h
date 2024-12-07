// This is the host-interface exposed to carts

#pragma once

// use this to debug your bytes
#include "hexdump.h"

// Used to test passing structs over wasm-barrier
typedef struct {
  u32 x;
  u32 y;
} TestPoint;

// UTILS API

// print a string
HOST_FUNCTION(void, trace, (u32 strPtr), {
  char* str = copy_from_cart_string(strPtr);
  printf("%s\n", str);
  free(str);
})

// a fatal exit
HOST_FUNCTION(void, abort, (u32 messagePtr, u32 filenamePtr, u32 line, u32 column), {
  char* message = copy_from_cart_string(messagePtr);
  char* filename = copy_from_cart_string(filenamePtr);
  fprintf(stderr, "%s: %s:%u:%u\n", message, filename, line, column);
  free(message);
  free(filename);
})

// TEST API

// send a string to host
HOST_FUNCTION(void, test_string_in, (u32 strPtr), {
  char* str = copy_from_cart_string(strPtr);
  printf("host test_string_in: got string from cart: %s\n", str);
  free(str);
})

// return a string from host
HOST_FUNCTION(u32, test_string_out, (), {
  char* s = "hello from host";
  return copy_to_cart_string(s);
})

// send some bytes to host
HOST_FUNCTION(void, test_bytes_in, (u32 bytesPtr, u32 bytesLen), {
  u8* bytes = copy_from_cart(bytesPtr, bytesLen);
  printf("host test_bytes_in:\n");
  hexdump(bytes, bytesLen);
  free(bytes);
})

// return some bytes from host
HOST_FUNCTION(u32, test_bytes_out, (u32 outLenPtr), {
  u32 outLen = 4;
  u8 returnVal[4] = {1,2,3,4};
  copy_to_cart_with_pointer(outLenPtr, &outLen, sizeof(outLen));
  return copy_to_cart(returnVal, outLen);
})

// send struct to host
HOST_FUNCTION(void, test_struct_in, (u32 pointPtr), {
  TestPoint* point = copy_from_cart(pointPtr, sizeof(TestPoint));
  printf("host test_struct_in: (%u, %u)\n", point->x, point->y);
  free(point);
})

// return struct from host
HOST_FUNCTION(u32, test_struct_out, (), {
  TestPoint result = { .x=1111, .y=2222 };
  return copy_to_cart(&result, sizeof(result));
})

// GRAPHICS API


// get a color (in host color-order) from cart (u32 RGBA)
static pntr_color get_pntr_color_from_cart(u32 colorInt) {
  return pntr_new_color((colorInt >> 24) & 0xFF, (colorInt >> 16) & 0xFF, (colorInt >> 8) & 0xFF, colorInt & 0xFF);
}

// convert a pntr_color to cart (u32 RGBA)
static u32 get_cart_color_from_pntr(pntr_color color) {
  return (color.rgba.r << 24) | (color.rgba.g << 16) | (color.rgba.b << 8) | color.rgba.a;
}

// take an image-pointer and add it to images
static u32 add_image(pntr_image* im) {
  u32 id = cvector_size(images);
  cvector_push_back(images, im);
  return id;
}

// take a font-pointer and add it to fonts
static u32 add_font(pntr_font* f) {
  u32 id = cvector_size(fonts);
  cvector_push_back(fonts, f);
  return id;
}

// Create a new blank image
HOST_FUNCTION(u32, new_image, (i32 width, i32 height, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  return add_image(pntr_gen_image_color(width, height, color));
})

// Copy an image to a new image
HOST_FUNCTION(u32, image_copy, (u32 imagePtr), {
  return add_image(pntr_image_copy(images[imagePtr]));
})

// Create an image from a region of another image
HOST_FUNCTION(u32, image_subimage, (u32 imagePtr, i32 x, i32 y, i32 width, i32 height), {
  return add_image(pntr_image_subimage(images[imagePtr], x, y, width, height));
})

// Clear the screen
HOST_FUNCTION(void, clear, (u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_clear_background(screen, color);
})

// Draw a single pixel on the screen
HOST_FUNCTION(void, draw_point, (i32 x, i32 y, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_point(screen, x, y, color);
})

// Draw a line on the screen
HOST_FUNCTION(void, draw_line, (i32 startPosX, i32 startPosY, i32 endPosX, i32 endPosY, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_line(screen, startPosX, startPosY, endPosX, endPosY, color);
})

// Draw a filled rectangle on the screen
HOST_FUNCTION(void, draw_rectangle, (i32 posX, i32 posY, i32 width, i32 height, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_rectangle(screen, posX, posY, width, height, color);
})

// Draw a filled triangle on the screen
HOST_FUNCTION(void, draw_triangle, (i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_triangle(screen, x1, y1, x2, y2, x3, y3, color);
})

// Draw a filled ellipse on the screen
HOST_FUNCTION(void, draw_ellipse, (i32 centerX, i32 centerY, i32 radiusX, i32 radiusY, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_ellipse(screen, centerX, centerY, radiusX, radiusY, color);
})

// Draw a filled circle on the screen
HOST_FUNCTION(void, draw_circle, (u32 centerX, u32 centerY, u32 radius, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_circle_fill(screen, centerX, centerY, radius, color);
})

// Draw a filled polygon on the screen
HOST_FUNCTION(void, draw_polygon, (u32 pointsPtr, i32 numPoints, u32 colorPtr), {
  pntr_vector* points = copy_from_cart(pointsPtr, sizeof(pntr_vector) * numPoints);
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_polygon_fill(screen, points, numPoints, color);
  free(points);
})

// Draw several lines on the screen
HOST_FUNCTION(void, draw_polyline, (u32 pointsPtr, i32 numPoints, u32 colorPtr), {
  pntr_vector* points = copy_from_cart(pointsPtr, sizeof(pntr_vector) * numPoints);
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_polyline(screen, points, numPoints, color);
  free(points);
})

// Draw a filled arc on the screen
HOST_FUNCTION(void, draw_arc, (i32 centerX, i32 centerY, f32 radius, f32 startAngle, f32 endAngle, i32 segments, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_arc(screen, centerX, centerY, radius, startAngle, endAngle, segments, color);
})

// Draw a filled round-rectangle on the screen
HOST_FUNCTION(void, draw_rectangle_rounded, (i32 x, i32 y, i32 width, i32 height, i32 cornerRadius, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_rectangle_rounded(screen, x, y, width, height, cornerRadius, cornerRadius, cornerRadius, cornerRadius, color);
})

// Draw an image on the screen
HOST_FUNCTION(void, draw_image, (u32 srcPtr, i32 posX, i32 posY), {
  pntr_draw_image(screen, images[srcPtr], posX, posY);
})

// Draw a tinted image on the screen
HOST_FUNCTION(void, draw_image_tint, (u32 srcPtr, i32 posX, i32 posY, u32 tintPtr), {
  pntr_color tint = get_pntr_color_from_cart(tintPtr);
  pntr_draw_image_tint(screen, images[srcPtr], posX, posY, tint);
})

// Draw an image, rotated, on the screen
HOST_FUNCTION(void, draw_image_rotated, (u32 srcPtr, i32 posX, i32 posY, f32 degrees, f32 offsetX, f32 offsetY, u32 filter), {
  pntr_draw_image_rotated(screen, images[srcPtr], posX, posY, degrees, offsetX, offsetY, filter);
})

// Draw an image, flipped, on the screen
HOST_FUNCTION(void, draw_image_flipped, (u32 srcPtr, i32 posX, i32 posY, bool flipHorizontal, bool flipVertical, bool flipDiagonal), {
  pntr_draw_image_flipped(screen, images[srcPtr], posX, posY, flipHorizontal, flipVertical, flipDiagonal);
})

// Draw an image, scaled, on the screen
HOST_FUNCTION(void, draw_image_scaled, (u32 srcPtr, i32 posX, i32 posY, f32 scaleX, f32 scaleY, f32 offsetX, f32 offsetY, u32 filter), {
  pntr_draw_image_scaled(screen, images[srcPtr], posX, posY, scaleX, scaleY, offsetX, offsetY, filter);
})

// Draw some text on the screen
HOST_FUNCTION(void, draw_text, (u32 fontPtr, u32 textPtr, i32 posX, i32 posY, u32 colorPtr), {
  char* text = copy_from_cart_string(textPtr);
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_text(screen, fonts[fontPtr], text, posX, posY, color);
})

// Save an image to persistant storage
HOST_FUNCTION(void, save_image, (u32 imagePtr, u32 filenamePtr), {
  char* filename = copy_from_cart_string(filenamePtr);
  pntr_save_image(images[imagePtr], filename);
})

// Load an image from a file in cart
HOST_FUNCTION(u32, load_image, (u32 filenamePtr), {
  char* filename = copy_from_cart_string(filenamePtr);
  return add_image(pntr_load_image(filename));
})

// Resize an image, in-place
HOST_FUNCTION(void, image_resize, (u32 imagePtr, i32 newWidth, i32 newHeight, i32 offsetX, i32 offsetY, u32 fillPtr), {
  pntr_color fill = get_pntr_color_from_cart(fillPtr);
  pntr_image_resize_canvas(images[imagePtr], newWidth, newHeight, offsetX, offsetY, fill);
})

// Scale an image, in-place
HOST_FUNCTION(void, image_scale, (u32 imagePtr, f32 scaleX, f32 scaleY, u32 filter), {
  pntr_image_scale(images[imagePtr], scaleX, scaleY, filter);
})

// Replace a color in an image, in-place
HOST_FUNCTION(void, image_color_replace, (u32 imagePtr, u32 colorPtr, u32 replacePtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_color replace = get_pntr_color_from_cart(replacePtr);
  pntr_image_color_replace(images[imagePtr], color, replace);
})

// Tint a color in an image, in-place
HOST_FUNCTION(void, image_color_tint, (u32 imagePtr, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_image_color_tint(images[imagePtr], color);
})

// Fade a color in an image, in-place
HOST_FUNCTION(void, image_color_fade, (u32 imagePtr, f32 alpha), {
  pntr_image_color_fade(images[imagePtr], alpha);
})

// Copy a font to a new font
HOST_FUNCTION(u32, font_copy, (u32 fontPtr), {
  return add_font(pntr_font_copy(fonts[fontPtr]));
})

// Get the current FPS
HOST_FUNCTION(u32, get_fps, (), {
  // TODO
  return 0;
})

// Scale a font, return a new font
HOST_FUNCTION(u32, font_scale, (u32 fontPtr, f32 scaleX, f32 scaleY, u32 filter), {
  return add_font(pntr_font_scale(fonts[fontPtr], scaleX, scaleY, filter));
})

// Load a BMF font from a file in cart
HOST_FUNCTION(u32, load_font_bmf, (u32 filenamePtr, u32 charactersPtr), {
  char* filename = copy_from_cart_string(filenamePtr);
  char* characters = copy_from_cart_string(charactersPtr);
  return add_font(pntr_load_font_bmf(filename, characters));
})

// Load a BMF font from an image
HOST_FUNCTION(u32, load_font_bmf_from_image, (u32 imagePtr, u32 charactersPtr), {
  char* characters = copy_from_cart_string(charactersPtr);
  return add_font(pntr_load_font_bmf_from_image(images[imagePtr], characters));
})

// Measure the size of some text
HOST_FUNCTION(u32, measure_text, (u32 fontPtr, u32 textPtr), {
  char* text = copy_from_cart_string(textPtr);
  pntr_vector result = pntr_measure_text_ex(fonts[fontPtr], text, 0);
  return copy_to_cart(&result, sizeof(result));
})

// Meaure an image (use 0 for screen)
HOST_FUNCTION(u32, measure_image, (u32 imagePtr), {
  pntr_vector result = {.x=images[imagePtr]->width, .y=images[imagePtr]->height};
  return copy_to_cart(&result, sizeof(result));
})

// Load a TTY font from a file in cart
HOST_FUNCTION(u32, load_font_tty, (u32 filenamePtr, i32 glyphWidth, i32 glyphHeight, u32 charactersPtr), {
  char* filename = copy_from_cart_string(filenamePtr);
  char* characters = copy_from_cart_string(charactersPtr);
  return add_font(pntr_load_font_tty(filename, glyphWidth, glyphHeight, characters));
})

// Load a TTY font from an image
HOST_FUNCTION(u32, load_font_tty_from_image, (u32 imagePtr, i32 glyphWidth, i32 glyphHeight, u32 charactersPtr), {
  char* characters = copy_from_cart_string(charactersPtr);
  return add_font(pntr_load_font_tty_from_image(images[imagePtr], glyphWidth, glyphHeight, characters));
})

// Load a TTF font from a file in cart
HOST_FUNCTION(u32, load_font_ttf, (u32 filenamePtr, i32 fontSize), {
  char* filename = copy_from_cart_string(filenamePtr);
  return add_font(pntr_load_font_ttf(filename, fontSize));
})

// Invert the colors in an image, in-place
HOST_FUNCTION(void, image_color_invert, (u32 imagePtr), {
  pntr_image_color_invert(images[imagePtr]);
})

// Calculate a rectangle representing the available alpha border in an image
HOST_FUNCTION(u32, image_alpha_border, (u32 imagePtr, f32 threshold), {
  pntr_rectangle result = pntr_image_alpha_border(images[imagePtr], threshold);
  return copy_to_cart(&result, sizeof(result));
})

// Crop an image, in-place
HOST_FUNCTION(void, image_crop, (u32 imagePtr, i32 x, i32 y, i32 width, i32 height), {
  pntr_image_crop(images[imagePtr], x, y, width, height);
})

// Crop an image based on the alpha border, in-place
HOST_FUNCTION(void, image_alpha_crop, (u32 imagePtr, f32 threshold), {
  pntr_image_alpha_crop(images[imagePtr], threshold);
})

// Adjust the brightness of an image, in-place
HOST_FUNCTION(void, image_color_brightness, (u32 imagePtr, f32 factor), {
  pntr_image_color_brightness(images[imagePtr], factor);
})

// Flip an image, in-place
HOST_FUNCTION(void, image_flip, (u32 imagePtr, bool horizontal, bool vertical), {
  pntr_image_flip(images[imagePtr], horizontal, vertical);
})

// Change the contrast of an image, in-place
HOST_FUNCTION(void, image_color_contrast, (u32 imagePtr, f32 contrast), {
  pntr_image_color_contrast(images[imagePtr], contrast);
})

// Use an image as an alpha-mask on another image
HOST_FUNCTION(void, image_alpha_mask, (u32 imagePtr, u32 alphaMaskPtr, i32 posX, i32 posY), {
  pntr_image_alpha_mask(images[imagePtr], images[alphaMaskPtr], posX, posY);
})

// Create a new image, rotating another image
HOST_FUNCTION(u32, image_rotate, (u32 imagePtr, f32 degrees, u32 filter), {
  return add_image(pntr_image_rotate(images[imagePtr], degrees, filter));
})

// Create a new image of a gradient
HOST_FUNCTION(u32, image_gradient, (i32 width, i32 height, u32 topLeftPtr, u32 topRightPtr, u32 bottomLeftPtr, u32 bottomRightPtr), {
  pntr_color topLeft = get_pntr_color_from_cart(topLeftPtr);
  pntr_color topRight = get_pntr_color_from_cart(topRightPtr);
  pntr_color bottomLeft = get_pntr_color_from_cart(bottomLeftPtr);
  pntr_color bottomRight = get_pntr_color_from_cart(bottomRightPtr);
  return add_image(pntr_gen_image_gradient(width, height, topLeft, topRight, bottomLeft, bottomRight));
})

// Unload an image
HOST_FUNCTION(void, unload_image, (u32 imagePtr), {
  pntr_unload_image(images[imagePtr]);
  cvector_erase(images, imagePtr);
})

// Unload a font
HOST_FUNCTION(void, unload_font, (u32 fontPtr), {
  pntr_unload_font(fonts[fontPtr]);
  cvector_erase(fonts, fontPtr);
})

// Clear an image
HOST_FUNCTION(void, clear_on_image, (u32 destinationPtr, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_clear_background(images[destinationPtr], color);
})

// Draw a single pixel on an image
HOST_FUNCTION(void, draw_point_on_image, (u32 destinationPtr, i32 x, i32 y, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_point(images[destinationPtr], x, y, color);
})

// Draw a line on an image
HOST_FUNCTION(void, draw_line_on_image, (u32 destinationPtr, i32 startPosX, i32 startPosY, i32 endPosX, i32 endPosY, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_line(images[destinationPtr], startPosX, startPosY, endPosX, endPosY, color);
})

// Draw a filled rectangle on an image
HOST_FUNCTION(void, draw_rectangle_on_image, (u32 destinationPtr, i32 posX, i32 posY, i32 width, i32 height, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_rectangle(images[destinationPtr], posX, posY, width, height, color);
})

// Draw a filled triangle on an image
HOST_FUNCTION(void, draw_triangle_on_image, (u32 destinationPtr, i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_triangle(images[destinationPtr], x1, y1, x2, y2, x3, y3, color);
})

// Draw a filled ellipse on an image
HOST_FUNCTION(void, draw_ellipse_on_image, (u32 destinationPtr, i32 centerX, i32 centerY, i32 radiusX, i32 radiusY, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_ellipse(images[destinationPtr], centerX, centerY, radiusX, radiusY, color);
})

// Draw a circle on an image
HOST_FUNCTION(void, draw_circle_on_image, (u32 destinationPtr, i32 centerX, i32 centerY, i32 radius, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_circle_fill(images[destinationPtr], centerX, centerY, radius, color);
})

// Draw a filled polygon on an image
HOST_FUNCTION(void, draw_polygon_on_image, (u32 destinationPtr, u32 pointsPtr, i32 numPoints, u32 colorPtr), {
  pntr_vector* points = copy_from_cart(pointsPtr, sizeof(pntr_vector) * numPoints);
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_polygon(images[destinationPtr], points, numPoints, color);
  free(points);
})

// Draw several lines on an image
HOST_FUNCTION(void, draw_polyline_on_image, (u32 destinationPtr, u32 pointsPtr, i32 numPoints, u32 colorPtr), {
  pntr_vector* points = copy_from_cart(pointsPtr, sizeof(pntr_vector) * numPoints);
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_polyline(images[destinationPtr], points, numPoints, color);
  free(points);
})

// Draw a filled round-rectangle on an image
HOST_FUNCTION(void, draw_rectangle_rounded_on_image, (u32 destinationPtr, i32 x, i32 y, i32 width, i32 height, i32 cornerRadius, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_rectangle_rounded(images[destinationPtr], x, y, width, height, cornerRadius, cornerRadius, cornerRadius, cornerRadius, color);
})

// Draw an image on an image
HOST_FUNCTION(void, draw_image_on_image, (u32 destinationPtr, u32 srcPtr, i32 posX, i32 posY), {
  pntr_draw_image(images[destinationPtr], images[srcPtr], posX, posY);
})

// Draw a tinted image on an image
HOST_FUNCTION(void, draw_image_tint_on_image, (u32 destinationPtr, u32 srcPtr, i32 posX, i32 posY, u32 tintPtr), {
  pntr_color tint = get_pntr_color_from_cart(tintPtr);
  pntr_draw_image_tint(images[destinationPtr], images[srcPtr], posX, posY, tint);
})

// Draw an image, rotated, on an image
HOST_FUNCTION(void, draw_image_rotated_on_image, (u32 destinationPtr, u32 srcPtr, i32 posX, i32 posY, f32 degrees, f32 offsetX, f32 offsetY, u32 filter), {
  pntr_draw_image_rotated(images[destinationPtr], images[srcPtr], posX, posY, degrees, offsetX, offsetY, filter);
})

// Draw an image, flipped, on an image
HOST_FUNCTION(void, draw_image_flipped_on_image, (u32 destinationPtr, u32 srcPtr, i32 posX, i32 posY, bool flipHorizontal, bool flipVertical, bool flipDiagonal), {
  pntr_draw_image_flipped(images[destinationPtr], images[srcPtr], posX, posY, flipHorizontal, flipVertical, flipDiagonal);
})

// Draw an image, scaled, on an image
HOST_FUNCTION(void, draw_image_scaled_on_image, (u32 destinationPtr, u32 srcPtr, i32 posX, i32 posY, f32 scaleX, f32 scaleY, f32 offsetX, f32 offsetY, u32 filter), {
  pntr_draw_image_scaled(images[destinationPtr], images[srcPtr], posX, posY, scaleX, scaleY, offsetX, offsetY, filter);
})

// Draw some text on an image
HOST_FUNCTION(void, draw_text_on_image, (u32 destinationPtr, u32 fontPtr, u32 textPtr, i32 posX, i32 posY, u32 colorPtr), {
  char* text = copy_from_cart_string(textPtr);
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_text(images[destinationPtr], fonts[fontPtr], text, posX, posY, color);
})

// Draw a 1px outlined rectangle on the screen
HOST_FUNCTION(void, draw_rectangle_outline, (i32 posX, i32 posY, i32 width, i32 height, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_rectangle(screen, posX, posY, width, height, color);
})

// Draw a 1px outlined triangle on the screen
HOST_FUNCTION(void, draw_triangle_outline, (i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_triangle(screen, x1, y1, x2, y2, x3, y3, color);
})

// Draw a 1px outlined ellipse on the screen
HOST_FUNCTION(void, draw_ellipse_outline, (i32 centerX, i32 centerY, i32 radiusX, i32 radiusY, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_ellipse(screen, centerX, centerY, radiusX, radiusY, color);
})

// Draw a 1px outlined circle on the screen
HOST_FUNCTION(void, draw_circle_outline, (i32 centerX, i32 centerY, i32 radius, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_circle(screen, centerX, centerY, radius, color);
})

// Draw a 1px outlined polygon on the screen
HOST_FUNCTION(void, draw_polygon_outline, (u32 pointsPtr, i32 numPoints, u32 colorPtr), {
  pntr_vector* points = copy_from_cart(pointsPtr, sizeof(pntr_vector) * numPoints);
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_polygon(screen, points, numPoints, color);
  free(points);
})

// Draw a 1px outlined arc on the screen
HOST_FUNCTION(void, draw_arc_outline, (i32 centerX, i32 centerY, f32 radius, f32 startAngle, f32 endAngle, i32 segments, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_arc(screen, centerX, centerY, radius, startAngle, endAngle, segments, color);
})

// Draw a 1px outlined round-rectangle on the screen
HOST_FUNCTION(void, draw_rectangle_rounded_outline, (i32 x, i32 y, i32 width, i32 height, i32 cornerRadius, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_rectangle_rounded(screen, x, y, width, height, cornerRadius, cornerRadius, cornerRadius, cornerRadius, color);
})

// Draw a 1px outlined rectangle on an image
HOST_FUNCTION(void, draw_rectangle_outline_on_image, (u32 destinationPtr, i32 posX, i32 posY, i32 width, i32 height, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_rectangle(images[destinationPtr], posX, posY, width, height, color);
})

// Draw a 1px outlined triangle on an image
HOST_FUNCTION(void, draw_triangle_outline_on_image, (u32 destinationPtr, i32 x1, i32 y1, i32 x2, i32 y2, i32 x3, i32 y3, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_triangle(images[destinationPtr], x1, y1, x2, y2, x3, y3, color);
})

// Draw a 1px outlined ellipse on an image
HOST_FUNCTION(void, draw_ellipse_outline_on_image, (u32 destinationPtr, i32 centerX, i32 centerY, i32 radiusX, i32 radiusY, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_ellipse(images[destinationPtr], centerX, centerY, radiusX, radiusY, color);
})

// Draw a 1px outlined circle on an image
HOST_FUNCTION(void, draw_circle_outline_on_image, (u32 destinationPtr, i32 centerX, i32 centerY, i32 radius, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_circle(images[destinationPtr], centerX, centerY, radius, color);
})

// Draw a 1px outlined polygon on an image
HOST_FUNCTION(void, draw_polygon_outline_on_image, (u32 destinationPtr, u32 pointsPtr, i32 numPoints, u32 colorPtr), {
  pntr_vector* points = copy_from_cart(pointsPtr, sizeof(pntr_vector) * numPoints);
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_polygon(images[destinationPtr], points, numPoints, color);
  free(points);
})

// Draw a 1px outlined round-rectangle on an image
HOST_FUNCTION(void, draw_rectangle_rounded_outline_on_image, (u32 destinationPtr, i32 x, i32 y, i32 width, i32 height, i32 cornerRadius, u32 colorPtr), {
  pntr_color color = get_pntr_color_from_cart(colorPtr);
  pntr_draw_rectangle_rounded(images[destinationPtr], x, y, width, height, cornerRadius, cornerRadius, cornerRadius, cornerRadius, color);
})



