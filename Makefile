# Compiler and flags
CXX = clang++
# Alternative: CXX = g++-14 (if you have multiple gcc versions)
CXXFLAGS = -std=c++23 -Wall -Wextra -Wpedantic -Wshadow
# CXXFLAGS = -std=c++17 -Wall -Wextra

# Project files
TARGET = build/cpplox
SRC = src/main.cpp
# If you have multiple source files:
# SRC = main.cpp file1.cpp file2.cpp

# Build the executable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Run test file
test: $(TARGET)
	./$(TARGET) test.lox

# Clean build files
clean:
	rm -f $(TARGET)

# Build and run generate_ast
generate: src/GenerateAst.cpp
	$(CXX) $(CXXFLAGS) -o build/generate_ast $<
	./build/generate_ast src

# Build and run AstPrinterDriver
testPrint: tools/AstPrinterDriver.cpp
	$(CXX) $(CXXFLAGS) -o build/AstPrinterDriver $<
	./build/AstPrinterDriver

# Default target
.PHONY: all clean run brun
all: $(TARGET)
