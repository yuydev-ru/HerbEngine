.PHONY: clean

CXX=g++
CXXFLAGS=-Wall -Wpedantic -ggdb -std=c++11
ifneq ($(OS),Windows_NT)
	ifeq ($(shell uname), Darwin)
		override CXXFLAGS += -arch x86_64
	endif
endif
PREFIX ?= .

INCLUDE_DIR = $(PREFIX)/include
BUILD_DIR = $(PREFIX)/build

SOURCES := $(wildcard components/*.cpp)
OBJECTS := $(patsubst components/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
DEPENDS := $(patsubst components/%.cpp,$(BUILD_DIR)/%.d,$(SOURCES))
OBJECTS += $(BUILD_DIR)/base.o
DEPENDS += $(BUILD_DIR)/base.d

all: $(OBJECTS)

-include $(DEPENDS)

$(BUILD_DIR)/base.o: base.cpp Makefile
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: components/%.cpp Makefile
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -MMD -MP -c $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	del /q "$(BUILD_DIR)\*"
else
	@rm -f "$(BUILD_DIR)/*"
endif