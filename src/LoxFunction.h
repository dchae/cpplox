#pragma once

#include "LoxCallable.h"
#include "LoxInstance.h"
#include <any>
#include <string>
#include <vector>

class Environment;
struct Function;

class LoxFunction : public LoxCallable {
  std::shared_ptr<Function> declaration;
  std::shared_ptr<Environment> closure;
  bool isInitializer;

public:
  LoxFunction(std::shared_ptr<Function> declaration,
              std::shared_ptr<Environment> closure, bool isInitializer);
  std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxInstance> instance);
  std::string toString() override;
  size_t arity() override;
  std::any call(Interpreter &interpreter,
                std::vector<std::any> arguments) override;
};
