.PHONY: clean

CC=g++
CFLAGS=-Wall -Wpedantic -ggdb -std=c++11
ifneq ($(OS), Windows_NT)
	ifeq ($(shell uname), Darwin)
		override CFLAGS += -arch x86_64
	endif
endif
PREFIX ?= .

INCLUDE_DIR = $(PREFIX)/include
LIB_DIR = $(PREFIX)/lib
BUILD_DIR = $(PREFIX)/build
PACKAGE_DIR = $(PREFIX)/package

TARGET=$(BUILD_DIR)/engine.o

all: $(TARGET)
$(TARGET): base.h base.cpp interface.h
	$(CC) $(CFLAGS) -c base.cpp -o $(BUILD_DIR)/engine.o


clean:
ifeq ($(OS),Windows_NT)
	del /q "$(BUILD_DIR)\*"
else
	@rm -f "$(BUILD_DIR)/*"
endif