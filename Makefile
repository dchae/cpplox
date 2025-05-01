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
	rm -f build/*

# Build and run generate_ast
# TODO: pipeline this into normal build process
generate_ast: tools/GenerateAst.cpp
	$(CXX) $(CXXFLAGS) -o build/generate_ast $<
	./build/generate_ast src
	rm -f build/generate_ast

# Build and run AstPrinterDriver
ast_printer: tools/AstPrinterDriver.cpp
	$(CXX) $(CXXFLAGS) -o build/AstPrinterDriver $<
	# ./build/AstPrinterDriver

# Default target
.PHONY: all clean run brun
all: $(TARGET)

# .PHONY: test-expressions
# test-expressions:
# 	@make $(TARGET) >/dev/null
# 	@echo "testing jlox with test-expressions.lox ..."
# 	@./jlox tests/test-expressions.lox 2>&1 | diff -u --color tests/test-expressions.lox.expected -;
#
#
# .PHONY: test-expressions2
# test-expressions2:
# 	@make jlox >/dev/null
# 	@echo "testing jlox with test-expressions2.lox ..."
# 	@./jlox tests/test-expressions2.lox | diff -u --color tests/test-expressions2.lox.expected -;
