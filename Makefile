# Variables
CXX := clang++
CXXFLAGS := -std=c++23 -Wall -Wextra -Wpedantic -Wshadow
CPPFLAGS := -MMD
COMPILE := $(CXX) $(CXXFLAGS) $(CPPFLAGS)

# Source files
LOX_SRCS := $(wildcard src/*.cpp)
TOOL_SRCS := $(wildcard tools/*.cpp)
SRCS := $(LOX_SRCS) $(TOOL_SRCS)

# Dependency files
LOX_DEPS := $(LOX_SRCS:src/%.cpp=build/%.d)
TOOL_DEPS := $(TOOL_SRCS:tools/%.cpp=build/%.d)
DEPS := $(LOX_DEPS) $(TOOL_DEPS)

# Object files
LOX_OBJS := $(LOX_SRCS:src/%.cpp=build/%.o)
TOOL_OBJS := $(TOOL_SRCS:tools/%.cpp=build/%.o)

# Executable paths
TARGET := build/cpplox

# include auto-generated dependencies
-include $(DEPS)

# Default target (build/cpplox)
# Expr.h and all Lox object files (cpplox.o,...) are prerequisites
$(TARGET): src/Expr.h $(LOX_OBJS) 
	$(COMPILE) $(LOX_OBJS) -o $(TARGET)

# Build GenerateAst
build/generate_ast: build/GenerateAst.o
	$(COMPILE) $< -o $@

# Run GenerateAst (generate Expr.h)
src/Expr.h: build/generate_ast
	./build/generate_ast src

# Run GenerateAst (generate Stmt.h)
src/Stmt.h: build/generate_ast
	./build/generate_ast src

# Build AstPrinterDriver
build/ast_printer: src/Expr.h build/AstPrinterDriver.o
	$(COMPILE) build/AstPrinterDriver.o -o $@

# Build src/ object files
build/%.o: src/%.cpp
	$(COMPILE) -c $< -o $@

# Build tools/ object files
build/%.o: tools/%.cpp
	$(COMPILE) -c $< -o $@

# Run the program
run: $(TARGET)
	./$(TARGET)

# Tests
.PHONY: test
test: $(TARGET)
	@echo "testing cpplox with test1.lox ..."
	@$(TARGET) tests/test1.lox 2>&1 | diff -u --color tests/test1.lox.expected -;

# Clean build files
.PHONY: clean
clean:
	rm -f build/*

# Aliases
.PHONY: generate_ast ast_printer all
ast: src/Expr.h src/Stmt.h
ast_printer: build/ast_printer
all: $(TARGET)
