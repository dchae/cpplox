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

  [[nodiscard]] std::string toString() const {
    std::string literalStr;

    switch (type) {
    case (IDENTIFIER):
      literalStr = lexeme;
      break;
    case (STRING):
      literalStr = std::any_cast<std::string>(literal);
      break;
    case (NUMBER):
      literalStr = std::to_string(std::any_cast<double>(literal));
      break;
    case (TRUE):
      literalStr = "true";
      break;
    case (FALSE):
      literalStr = "false";
      break;
    default:
      literalStr = "nil";
    }

    return ::toString(type) + " " + lexeme + " " + literalStr;
  }
};
