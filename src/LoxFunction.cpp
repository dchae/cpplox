#include "LoxFunction.h"
#include "Environment.h"
#include "Interpreter.h"
#include "Stmt.h"

LoxFunction::LoxFunction(std::shared_ptr<Function> declaration,
                         std::shared_ptr<Environment> closure,
                         bool isInitializer)
    : declaration(std::move(declaration)), closure(std::move(closure)),
      isInitializer{isInitializer} {}

std::shared_ptr<LoxFunction>
LoxFunction::bind(std::shared_ptr<LoxInstance> instance) {
  auto environment = std::make_shared<Environment>(closure);
  environment->define("this", instance);

  return std::make_shared<LoxFunction>(declaration, environment, isInitializer);
}

size_t LoxFunction::arity() { return declaration->params.size(); }

std::any LoxFunction::call(Interpreter &interpreter,
                           std::vector<std::any> arguments) {
  std::shared_ptr<Environment> environment =
      std::make_shared<Environment>(closure);

  for (size_t i = 0; i < declaration->params.size(); i++) {
    environment->define(declaration->params[i].lexeme, arguments[i]);
  }

  try {
    interpreter.executeBlock(declaration->body, environment);
  } catch (LoxReturn returnValue) {
    if (isInitializer) {
      return closure->getAt(0, "this");
    }

    return returnValue.value;
  }

  if (isInitializer) {
    return closure->getAt(0, "this");
  }

  return nullptr;
}

std::string LoxFunction::toString() {
  return "<fn " + declaration->name.lexeme + ">";
}
