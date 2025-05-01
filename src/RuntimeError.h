#pragma once

#include "Token.h"
#include <stdexcept>

class RuntimeError : public std::runtime_error {
public:
  const Token token;

  RuntimeError(Token token, std::string_view message)
      : std::runtime_error{message.data()}, token{token} {};
};
