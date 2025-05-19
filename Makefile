# Variables
ARGS ?= # Default empty args
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
	./$(TARGET) $(ARGS)

# Tests
define make_test
.PHONY: $(1)
$(1):
	@make all >/dev/null
	@echo "testing cpplox with $(1).lox ..."
	@./build/cpplox tests/$(1).lox | diff -u --color tests/$(1).lox.expected -;
endef


define make_test_error
.PHONY: $(1)
$(1):
	@make all >/dev/null
	@echo "testing cpplox with $(1).lox ..."
	@./build/cpplox tests/$(1).lox 2>&1 | diff -u --color tests/$(1).lox.expected -;
endef


TESTS = \
test-control-flow \
test-control-flow2 \
test-functions \
test-functions2 \
test-functions3 \
test-functions4 \
test-resolving \
test-statements \
test-statements2 \
test-statements3 \
test-statements4 \
test-statements5 \
test-statements6 \


TEST_ERRORS = \
test-resolving2 \
test-resolving3 \
test-resolving4 \

$(foreach test, $(TESTS), $(eval $(call make_test,$(test))))
$(foreach test, $(TEST_ERRORS), $(eval $(call make_test_error,$(test))))


.PHONY: test-all
test-all:
	@for test in $(TESTS) $(TEST_ERRORS); do \
		make -s $$test; \
	done


# Clean build files
.PHONY: clean
clean:
	rm -f build/*

# Aliases
.PHONY: generate_ast ast_printer all
ast: src/Expr.h src/Stmt.h
ast_printer: build/ast_printer
all: $(TARGET)
