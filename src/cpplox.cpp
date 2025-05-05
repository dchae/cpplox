#include "Error.h"
#include "Interpreter.h"
#include "Parser.h"
#include "Scanner.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// We are including this for now to mirror the jlox code
// TODO: Refactor this include
#include "LoxFunction.cpp"

std::string readFile(const std::string_view path) {
  std::ifstream file(path.data(), std::ios::binary);
  if (!file) {
    std::cerr << "Error reading file: " << path << std::strerror(errno) << '\n';
    std::exit(74);
  }

  std::ostringstream buffer;
  buffer << file.rdbuf();
  file.close();

  return buffer.str();
}

Interpreter interpreter{};

void run(std::string_view source) {
  Scanner scanner{source};
  std::vector<Token> tokens = scanner.scanTokens();

  Parser parser{tokens};
  std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

  // Stop if there was a syntax error
  if (hadError) {
    return;
  }

  interpreter.interpret(statements);
}

void runFile(const std::string_view path) {
  std::string contents = readFile(path);
  run(contents);

  if (hadError) {
    std::exit(65);
  }
  if (hadRuntimeError) {
    std::exit(70);
  }
}

void runPrompt() {
  std::string line;
  for (;;) {
    std::cout << "> ";
    if (!std::getline(std::cin, line)) {
      break;
    }
    run(line);
    hadError = false;
  }
}

int main(int argc, char *argv[]) {
  if (argc > 2) {
    std::cerr << "Usage: cpplox <filename>" << '\n';
    return 64;
  }

  if (argc == 2) {
    runFile(argv[1]);
  } else {
    runPrompt();
    return 0;
  }

  return 0;
}
