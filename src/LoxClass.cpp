#include "LoxClass.h"
#include "LoxFunction.h"
#include <utility>

LoxClass::LoxClass(std::string name, std::shared_ptr<LoxClass> superclass,
                   std::map<std::string, std::shared_ptr<LoxFunction>> methods)
    : name{std::move(name)}, superclass{std::move(superclass)},
      methods{std::move(methods)} {}

std::shared_ptr<LoxFunction>
LoxClass::findMethod(const std::string &methodName) {
  if (methods.contains(methodName)) {
    return methods[methodName];
  }

  if (superclass != nullptr) {
    return superclass->findMethod(methodName);
  }

  return nullptr;
}

std::string LoxClass::toString() { return name; }

std::any LoxClass::call(Interpreter &interpreter,
                        std::vector<std::any> arguments) {
  auto instance = std::make_shared<LoxInstance>(shared_from_this());
  std::shared_ptr<LoxFunction> initializer = findMethod("init");
  if (initializer != nullptr) {
    initializer->bind(instance)->call(interpreter, std::move(arguments));
  }

  return instance;
}

size_t LoxClass::arity() {
  std::shared_ptr<LoxFunction> initializer = findMethod("init");
  if (initializer == nullptr) {
    return 0;
  }

  return initializer->arity();
}
