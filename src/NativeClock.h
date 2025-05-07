#pragma once

#include "LoxCallable.h"

class NativeClock : public LoxCallable {
public:
  size_t arity() override;
  std::any call(Interpreter &interpreter,
                std::vector<std::any> arguments) override;
  std::string toString() override;
};
