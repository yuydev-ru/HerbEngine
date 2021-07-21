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

SRC=gui.cpp base.cpp
TARGET=$(BUILD_DIR)/engine.o

all: $(TARGET)
$(TARGET): gui.o base.h base.cpp interface.h
	$(CC) $(CFLAGS) -I./$(INCLUDE_DIR) -c base.cpp -o $(BUILD_DIR)/engine.o

gui.o: gui.cpp gui.h
	$(CC) $(CFLAGS) -I./$(INCLUDE_DIR) -c gui.cpp -o $(BUILD_DIR)/gui.o


clean:
ifeq ($(OS),Windows_NT)
	del /q "$(BUILD_DIR)\*"
else
	@rm -f "$(BUILD_DIR)/*"
endif