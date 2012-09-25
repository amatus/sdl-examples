/*
This is an example paint program for beginning C programmers.
It simply creates a blank surface on which you can paint with the mouse.
Each mouse press gets a new random color.

To verify that you have the required SDL library files installed use the
pkg-config utility at your shell prompt:
$ pkg-config --version sdl

On Debian and Ubuntu you can install the SDL library files using apt-get:
$ sudo apt-get install libsdl1.2-dev

To build this program using the accompanying Makefile run the make utility at
your shell prompt:
$ make

Run the resulting program at your shell prompt:
$ ./paint
*/

#include <stdlib.h>
#include <SDL.h>

// paint: Utility function that paints colors to a surface.
void paint(SDL_Surface *surface, int x, int y,
           Uint8 red, Uint8 green, Uint8 blue);

void main() {
  int return_code;

  // Initialize the SDL library
  return_code = SDL_Init(SDL_INIT_VIDEO);
  if(return_code < 0) {
    fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
    exit(1);
  }
  atexit(SDL_Quit);

  // Set the video mode we want
  SDL_Surface *surface;
  int width = 0; // 0 means any width
  int height = 0; // 0 means any height
  int bits_per_pixel = 0; // 0 means use the current bpp
  Uint32 flags = SDL_RESIZABLE; // make a resizable window
  surface = SDL_SetVideoMode(width, height, bits_per_pixel, flags);
  if(NULL == surface) {
    fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
    exit(2);
  }

  // Main loop
  int painting = 0; // variable to keep track of mouse-down state
  Uint8 red, green, blue; // variables to keep track of our current color
  while(1) {
    SDL_Event event;

    // Wait for the user to do something
    SDL_WaitEvent(&event);

    // Act on the various event types
    if(SDL_QUIT == event.type) { // the user wants to quit
      exit(0);
    } else if(SDL_VIDEORESIZE == event.type) { // our window was resized
      // Update our surface with the new size
      width = event.resize.w;
      height = event.resize.h;
      surface = SDL_SetVideoMode(width, height, bits_per_pixel, flags);
      if(NULL == surface) {
        fprintf(stderr, "Unable to resize video mode: %s\n", SDL_GetError());
        exit(2);
      }
    } else if(SDL_MOUSEBUTTONDOWN == event.type) { // a button is being pressed
      // Start painting with a random color
      painting = 1;
      red = (rand() % 7 + 1) * 32;
      green = (rand() % 7 + 1) * 32;
      blue = (rand() % 7 + 1) * 32;
      paint(surface, event.button.x, event.button.y, red, green, blue);
    } else if(SDL_MOUSEMOTION == event.type) { // the mouse moved
      if(painting) {
        // Continue painting
        paint(surface, event.motion.x, event.motion.y, red, green, blue);
      }
    } else if(SDL_MOUSEBUTTONUP == event.type) { // a button is being released
      // Stop painting
      painting = 0;
    }
  }
}

/*
paint: Utility function that paints colors to a surface.
The location to paint is given by x and y, the color to paint is
a mixture of red, green, and blue values in the range 0 to 255.
*/
void paint(SDL_Surface *surface, int x, int y,
           Uint8 red, Uint8 green, Uint8 blue) {
  if(SDL_MUSTLOCK(surface)) {
    // The surface must be locked to access surface->pixels
    SDL_LockSurface(surface);
  }
  // This is the size of each pixel in bytes
  int bytes_per_pixel = surface->format->BytesPerPixel;
  // surface->pixels is a pointer into memory, it points at the first pixel in
  // the surface which is at (0, 0).
  // We create our own pointer which we are going to move around until we find
  // the pixel we want to modify.
  Uint8 *pixel;
  // We start at (0, 0)
  pixel = surface->pixels;
  // surface->pitch is the number of bytes between each row of pixels, we move
  // our pointer forward in memory until we find the start of the row for y
  pixel += y * surface->pitch;
  // We move our pointer forward in memory unitl we find the pixel in this row
  // for x
  pixel += x * bytes_per_pixel;
  // For the given red, green, and blue values we need to know the corresponding
  // number to write into memory to make that color
  Uint32 color = SDL_MapRGB(surface->format, red, green, blue);
  // Figure out the integer type used to write the color into memory
  if(sizeof(Uint8) == bytes_per_pixel) {
    *pixel = color;
  } else if(sizeof(Uint16) == bytes_per_pixel) {
    Uint16 *pixel2 = (Uint16 *)pixel;
    *pixel2 = color;
  } else if(sizeof(Uint32) == bytes_per_pixel) {
    Uint32 *pixel4 = (Uint32 *)pixel;
    *pixel4 = color;
  }
  if(SDL_MUSTLOCK(surface)) {
    // We're done with surface->pixels so unlock the surface
    SDL_UnlockSurface(surface);
  }
  // Make our changes to surface memory visible on the screen
  Sint32 rect_width = 1, rect_height = 1;
  SDL_UpdateRect(surface, x, y, rect_width, rect_height);
}
