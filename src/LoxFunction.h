#pragma once

#include "LoxCallable.h"
#include <any>
#include <string>
#include <vector>

class Environment;
class Function;

class LoxFunction : public LoxCallable {
  std::shared_ptr<Function> declaration;
  std::shared_ptr<Environment> closure;

public:
  LoxFunction(std::shared_ptr<Function> declaration,
              std::shared_ptr<Environment> closure);
  size_t arity() override;
  std::any call(Interpreter &interpreter,
                std::vector<std::any> arguments) override;
  std::string toString() override;
};
