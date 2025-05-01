# Compiler and flags
CXX := clang++
CXXFLAGS := -std=c++23 -Wall -Wextra -Wpedantic -Wshadow
CPPFLAGS := -MMD

COMPILE := $(CXX) $(CXXFLAGS) $(CPPFLAGS)

# Project files
SRCS := src/main.cpp tools/AstPrinterDriver.cpp tools/GenerateAst.cpp
DEPS := $(SRCS:.cpp=.d)
TARGET := build/cpplox

# Default target
cpplox: src/Expr.h build/cpplox.o
	$(COMPILE) build/cpplox.o -o $(TARGET)

# Run the program
run: cpplox
	./$(TARGET)

# Build GenerateAst
generate_ast: build/GenerateAst.o
	$(COMPILE) $< -o build/generate_ast

# Run GenerateAst (generate Expr.h)
Expr.h: generate_ast
	./build/generate_ast src

# Build AstPrinterDriver
ast_printer: src/Expr.h build/AstPrinterDriver.o
	$(COMPILE) build/AstPrinterDriver.o -o build/$@

# Build object files
build/%.o: src/%.cpp
	$(COMPILE) -c $< -o $@

build/%.o: tools/%.cpp
	$(COMPILE) -c $< -o $@

-include $(DEPS)

# Clean build files
.PHONY: clean
clean:
	rm -f build/*

.PHONY: test-expressions
test-expressions:
	@make $(TARGET) >/dev/null
	@echo "testing cpplox with test-expressions.lox ..."
	@./build/cpplox tests/test-expressions.lox 2>&1 | diff -u --color tests/test-expressions.lox.expected -;


.PHONY: test-expressions2
test-expressions2:
	@make $(TARGET) >/dev/null
	@echo "testing cpplox with test-expressions2.lox ..."
	@./build/cpplox tests/test-expressions2.lox | diff -u --color tests/test-expressions2.lox.expected -;
