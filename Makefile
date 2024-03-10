CXX = g++
CXXFLAGS = -std=c++17
SRC_DIR = ./src
SRC = $(SRC_DIR)/Main.cpp
HEADERS = $(SRC_DIR)/Tokenizer.hpp $(SRC_DIR)/Parser.hpp $(SRC_DIR)/Visitor.hpp
TARGET = verse-compiler
INPUT = code.vs

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $<
clean:
	rm -f $(TARGET)

run:
	 ./verse-compiler $(INPUT)

