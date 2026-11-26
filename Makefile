CC = gcc
# Automatically fetch include paths for GLib, SDL3, and SDL3_mixer
CFLAGS = -Wall -Wextra -Iinclude -g $(shell pkg-config --cflags glib-2.0 sdl3 sdl3-mixer)
# Automatically fetch library linking flags
LDLIBS = $(shell pkg-config --libs glib-2.0 sdl3 sdl3-mixer) -lsmf -latomic -L/usr/local/lib

SRCS = $(wildcard src/*.c) $(wildcard src/microui/microui.c) $(wildcard src/modules/*.c) $(wildcard src/modules/sdl/*.c)

# Route all object files into the build/ folder
OBJS = $(patsubst %.c, build/%.o, $(SRCS))
TARGET = synth

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

# Pattern rule to compile into build/ and auto-create subdirectories
build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build

.PHONY: all clean
