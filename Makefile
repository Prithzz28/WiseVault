# WiseVault Makefile
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS  := -lm
SRC      := src/main.cpp
TARGET   := WiseVault
DATA_DIR := data

.PHONY: all clean run

all: $(DATA_DIR) $(TARGET)

$(DATA_DIR):
	mkdir -p $(DATA_DIR)

$(TARGET): $(SRC) include/*.hpp
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
