#pragma once

#include "Token.h"
#include <iostream>
#include <string_view>

inline bool hadError = false;

inline void report(int line, std::string_view where, std::string_view message) {
  std::cerr << "[line " << line << "] Error" << where << ": " << message
            << std::endl;
  hadError = true;
}

inline void error(const Token &token, std::string_view message) {
  if (token.type == END_OF_FILE) {
    report(token.line, " at end", message);
  } else {
    report(token.line, " at '" + token.lexeme + "'", message);
  }
}

inline void error(int line, std::string_view message) {
  report(line, "", message);
}
