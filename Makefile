# Makefile template for compiling C++ projects.
# 
# USAGE:
# 	make [target]
# 
# TARGETS:
# 	release		Release build
# 	debug		Debug build
# 	clean		Clean up the object files
#
# Author: Andrei Purcarus


# Configuration Settings
TARGET := agent.exe
CXXFLAGS := -std=c++1y -Wall -Wextra -pedantic -Isrc
LIBFLAGS := 
SRCS := main.cpp
DIRECTORIES := 


CXX_RELEASE := g++
CXXFLAGS_RELEASE := $(CXXFLAGS) -O3
CXX_DEBUG := g++
CXXFLAGS_DEBUG := $(CXXFLAGS) -g

BUILD_DIR := ./build
RELEASE_DIR := $(BUILD_DIR)/release
RELEASE_DIRS := $(DIRECTORIES:%=$(RELEASE_DIR)/%)
DEBUG_DIR := $(BUILD_DIR)/debug
DEBUG_DIRS := $(DIRECTORIES:%=$(DEBUG_DIR)/%)
SRC_DIR := ./src
OBJS_RELEASE := $(SRCS:%.cpp=$(RELEASE_DIR)/%.o)
DEPS_RELEASE := $(OBJS_RELEASE:.o=.d)
OBJS_DEBUG := $(SRCS:%.cpp=$(DEBUG_DIR)/%.o)
DEPS_DEBUG := $(OBJS_DEBUG:.o=.d)


all: release
	cp $(RELEASE_DIR)/$(TARGET) $(TARGET)

.PHONY: release debug clean
release: $(RELEASE_DIR) $(RELEASE_DIRS) $(RELEASE_DIR)/$(TARGET)
debug: $(DEBUG_DIR) $(DEBUG_DIRS) $(DEBUG_DIR)/$(TARGET)
clean:
	rm -rf $(BUILD_DIR) $(TARGET)


$(RELEASE_DIR):
	mkdir -p $@
$(RELEASE_DIRS):
	mkdir -p $@
$(RELEASE_DIR)/$(TARGET): $(OBJS_RELEASE)
	$(CXX_RELEASE) $(CXXFLAGS_RELEASE) $^ -o $@ $(LIBFLAGS)
-include $(DEPS_RELEASE)
$(RELEASE_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX_RELEASE) $(CXXFLAGS_RELEASE) -MMD -c $< -o $@


$(DEBUG_DIR):
	mkdir -p $@
$(DEBUG_DIRS):
	mkdir -p $@
$(DEBUG_DIR)/$(TARGET): $(OBJS_DEBUG)
	$(CXX_DEBUG) $(CXXFLAGS_DEBUG) $^ -o $@ $(LIBFLAGS)
-include $(DEPS_DEBUG)
$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX_DEBUG) $(CXXFLAGS_DEBUG) -MMD -c $< -o $@

