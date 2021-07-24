.PHONY: clean

CC=g++
CFLAGS=-Wall -Wpedantic -ggdb -std=c++11
ifneq ($(OS),Windows_NT)
	ifeq ($(shell uname), Darwin)
		override CFLAGS += -arch x86_64
	endif
endif
PREFIX ?= .

INCLUDE_DIR = $(PREFIX)/include
LIB_DIR = $(PREFIX)/lib
BUILD_DIR = $(PREFIX)/build
PACKAGE_DIR = $(PREFIX)/package

all: base.o rendering.o physics.o sound.o

base.o: base.h base.cpp interface.h
	$(CC) $(CFLAGS) -I./$(INCLUDE_DIR) -c base.cpp -o $(BUILD_DIR)/base.o

rendering.o: components/rendering.cpp components/rendering.h
	$(CC) $(CFLAGS) -I./$(INCLUDE_DIR) -c components/rendering.cpp -o $(BUILD_DIR)/rendering.o

physics.o: components/physics.cpp components/physics.h
	$(CC) $(CFLAGS) -I./$(INCLUDE_DIR) -c components/physics.cpp -o $(BUILD_DIR)/physics.o

sound.o: components/sound.cpp components/sound.h
	$(CC) $(CFLAGS) -I./$(INCLUDE_DIR) -c components/sound.cpp -o $(BUILD_DIR)/sound.o
clean:
ifeq ($(OS),Windows_NT)
	del /q "$(BUILD_DIR)\*"
else
	@rm -f "$(BUILD_DIR)/*"
endif