#pragma once

#include "RuntimeError.h"
#include "Token.h"
#include <any>
#include <map>
#include <memory>
#include <string>
#include <utility>

class Environment : public std::enable_shared_from_this<Environment> {
  std::shared_ptr<Environment> enclosing;
  std::map<std::string, std::any> values;

public:
  Environment() : enclosing{nullptr} {}

  Environment(std::shared_ptr<Environment> enclosing)
      : enclosing{std::move(enclosing)} {}

  std::any get(const Token &name) {
    if (values.contains(name.lexeme)) {
      return values[name.lexeme];
    }

    if (enclosing != nullptr) {
      return enclosing->get(name);
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
  }

  void assign(const Token &name, std::any value) {
    if (values.contains(name.lexeme)) {
      values[name.lexeme] = std::move(value);
      return;
    }

    if (enclosing != nullptr) {
      enclosing->assign(name, std::move(value));
      return;
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
  }

  void define(const std::string &name, std::any value) {
    values[name] = std::move(value);
  }

  std::shared_ptr<Environment> ancestor(int distance) {
    std::shared_ptr<Environment> environment = shared_from_this();
    for (int i = 0; i < distance; i++) {
      environment = environment->enclosing;
    }

    return environment;
  }

  std::any getAt(int distance, const std::string &name) {
    return ancestor(distance)->values[name];
  }

  void assignAt(int distance, const Token &name, std::any value) {
    ancestor(distance)->values[name.lexeme] = std::move(value);
  }
};
