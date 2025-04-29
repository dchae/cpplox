#pragma once

#include "TokenType.h"
#include <any>
#include <string>
#include <utility> // for std::move

class Token {
public:
  const TokenType type;
  const std::string lexeme;
  const std::any literal;
  const int line;

  Token(TokenType type, std::string lexeme, std::any literal, int line)
      : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)),
        line(line) {}

  std::string toString() const {
    std::string literal_str;

    switch (type) {
    case (IDENTIFIER):
      literal_str = lexeme;
      break;
    case (STRING):
      literal_str = std::any_cast<std::string>(literal);
      break;
    case (NUMBER):
      literal_str = std::to_string(std::any_cast<double>(literal));
      break;
    case (TRUE):
      literal_str = "true";
      break;
    case (FALSE):
      literal_str = "false";
      break;
    default:
      literal_str = "nil";
    }

    return ::toString(type) + " " + lexeme + " " + literal_str;
  }
};
