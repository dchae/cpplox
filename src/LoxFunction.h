#pragma once

#include "LoxCallable.h"
#include <any>
#include <string>
#include <vector>

class Function;

class LoxFunction : public LoxCallable {
  std::shared_ptr<Function> declaration;

public:
  LoxFunction(std::shared_ptr<Function> declaration);
  size_t arity() override;
  std::any call(Interpreter &interpreter,
                std::vector<std::any> arguments) override;
  std::string toString() override;
};
