CC = gcc
# Automatically fetch GLib include paths
CFLAGS = -Wall -Wextra -Iinclude -g $(shell pkg-config --cflags glib-2.0)
# Automatically fetch GLib library linking flags
LDLIBS = $(shell pkg-config --libs glib-2.0) -lsmf -L/usr/local/lib

SRCS = $(wildcard src/*.c) $(wildcard src/modules/*.c)
OBJS = $(SRCS:.c=.o)
TARGET = synth

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
