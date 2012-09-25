SDL_CFLAGS=$(shell pkg-config --cflags sdl)
SDL_LIBS=$(shell pkg-config --libs sdl)

all: paint

paint: paint.c
	$(CC) $(SDL_CFLAGS) $< $(SDL_LIBS) -o $@
