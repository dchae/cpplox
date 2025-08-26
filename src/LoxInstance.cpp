#include "LoxInstance.h"
#include "Error.h"
#include "LoxClass.h"
#include "LoxFunction.h"
#include <utility> // std::move

LoxInstance::LoxInstance(std::shared_ptr<LoxClass> klass)
    : klass{std::move(klass)} {}

std::any LoxInstance::get(const Token &name) {
  if (fields.contains(name.lexeme)) {
    return fields[name.lexeme];
  }

  std::shared_ptr<LoxFunction> method = klass->findMethod(name.lexeme);
  if (method != nullptr) {
    return method->bind(shared_from_this());
  }

  throw new RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
}

void LoxInstance::set(const Token &name, std::any value) {
  fields[name.lexeme] = std::move(value);
}

std::string LoxInstance::toString() { return klass->name + " instance"; }
