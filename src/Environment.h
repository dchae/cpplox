#pragma once

#include "RuntimeError.h"
#include "Token.h"
#include <any>
#include <map>
#include <string>
#include <utility>

class Environment {
  std::map<std::string, std::any> values;

public:
  std::any get(const Token &name) {
    if (values.contains(name.lexeme)) {
      return values[name.lexeme];
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
  }

  void assign(const Token &name, std::any value) {
    if (values.contains(name.lexeme)) {
      values[name.lexeme] = std::move(value);
      return;
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
  }
  void define(const std::string &name, std::any value) {
    values[name] = std::move(value);
  }
};
