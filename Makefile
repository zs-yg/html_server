CC = gcc
CFLAGS = -Wall -Wextra -g `pkg-config --cflags gtk4 gio-2.0` -mwindows
LDFLAGS = `pkg-config --libs gtk4 gio-2.0` -mwindows

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET = html_server

.PHONY: all clean

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

install: all
	cp $(BUILD_DIR)/$(TARGET) /usr/local/bin

uninstall:
	rm -f /usr/local/bin/$(TARGET)
