#pragma once

#include "Environment.h"
#include "Error.h"
#include "Expr.h"
#include "LoxCallable.h"
#include "LoxFunction.h"
#include "LoxReturn.h"
#include "NativeClock.h"
#include "RuntimeError.h"
#include "Stmt.h"
#include <any>
#include <format> // std::format (c++20)
#include <iostream>
#include <map>
#include <memory> // std::shared_ptr
#include <utility>
#include <vector>

class Interpreter : public ExprVisitor, public StmtVisitor {
  friend class LoxFunction;

public:
  std::shared_ptr<Environment> globals{new Environment};

private:
  std::shared_ptr<Environment> environment = globals;
  std::map<std::shared_ptr<Expr>, int> locals;

public:
  Interpreter() { globals->define("clock", std::make_shared<NativeClock>()); }

  void interpret(const std::vector<std::shared_ptr<Stmt>> &statements) {
    try {
      for (const std::shared_ptr<Stmt> &statement : statements) {
        execute(statement);
      }
    } catch (RuntimeError error) {
      runtimeError(error);
    }
  }

  void resolve(const std::shared_ptr<Expr> &expr, int depth) {
    locals[expr] = depth;
  }

private:
  std::any evaluate(const std::shared_ptr<Expr> &expr) {
    // send expression back into the visitor implementation
    return expr->accept(*this);
  }

  void execute(const std::shared_ptr<Stmt> &stmt) { stmt->accept(*this); }

  void executeBlock(const std::vector<std::shared_ptr<Stmt>> &statements,
                    std::shared_ptr<Environment> env) {
    std::shared_ptr<Environment> previous = this->environment;

    try {
      this->environment = std::move(env);

      for (const std::shared_ptr<Stmt> &statement : statements) {
        execute(statement);
      }
    } catch (...) {
      this->environment = previous;
      throw;
    }

    this->environment = previous;
  }

public:
  // Statement visitor implementations
  std::any visitVarStmt(std::shared_ptr<Var> stmt) override {
    std::any value = nullptr;
    if (stmt->initializer != nullptr) {
      value = evaluate(stmt->initializer);
    }

    environment->define(stmt->name.lexeme, std::move(value));
    return {};
  }

  std::any visitIfStmt(std::shared_ptr<If> stmt) override {
    if (isTruthy(evaluate(stmt->condition))) {
      execute(stmt->thenBranch);
    } else if (stmt->elseBranch != nullptr) {
      execute(stmt->elseBranch);
    }

    return {};
  }

  std::any visitPrintStmt(std::shared_ptr<Print> stmt) override {
    std::any value = evaluate(stmt->expression);
    std::cout << stringify(value) << "\n";

    return {};
  }

  std::any visitReturnStmt(std::shared_ptr<Return> stmt) override {
    std::any value = nullptr;
    if (stmt->value != nullptr) {
      value = evaluate(stmt->value);
    }

    throw LoxReturn{value};
  }

  std::any visitWhileStmt(std::shared_ptr<While> stmt) override {
    while (isTruthy(evaluate(stmt->condition))) {
      execute(stmt->body);
    }

    return {};
  }

  std::any visitBlockStmt(const std::shared_ptr<Block> stmt) override {
    executeBlock(stmt->statements, std::make_shared<Environment>(environment));
    return {};
  }

  std::any visitExpressionStmt(std::shared_ptr<Expression> stmt) override {
    evaluate(stmt->expression);

    return {};
  }

  std::any visitFunctionStmt(std::shared_ptr<Function> stmt) override {
    std::shared_ptr<LoxFunction> function =
        std::make_shared<LoxFunction>(stmt, environment);
    environment->define(stmt->name.lexeme, function);

    return {};
  }

  // Expression visitor implementations
  std::any visitAssignExpr(std::shared_ptr<Assign> expr) override {
    std::any value = evaluate(expr->value);

    if (locals.contains(expr)) {
      int distance = locals[expr];
      environment->assignAt(distance, expr->name, value);
    } else {
      globals->assign(expr->name, value);
    }

    return value;
  }

  std::any visitLogicalExpr(std::shared_ptr<Logical> expr) override {
    std::any left = evaluate(expr->left);

    if (expr->op.type == OR) {
      if (isTruthy(left)) {
        return left;
      }
    } else {
      if (!isTruthy(left)) {
        return left;
      }
    }

    return evaluate(expr->right);
  }

  std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override {
    std::any left = evaluate(expr->left);
    std::any right = evaluate(expr->right);

    switch (expr->op.type) {
    case BANG_EQUAL:
      return !isEqual(left, right);
    case EQUAL_EQUAL:
      return isEqual(left, right);
    case GREATER:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double>(left) > std::any_cast<double>(right);
    case GREATER_EQUAL:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double>(left) >= std::any_cast<double>(right);
    case LESS:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double>(left) < std::any_cast<double>(right);
    case LESS_EQUAL:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double>(left) <= std::any_cast<double>(right);
    case MINUS:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double>(left) - std::any_cast<double>(right);
    case PLUS:
      if (left.type() == right.type()) {
        if (left.type() == typeid(double)) {
          return std::any_cast<double>(left) + std::any_cast<double>(right);
        }
        if (left.type() == typeid(std::string)) {
          return std::any_cast<std::string>(left) +
                 std::any_cast<std::string>(right);
        }
      }

      throw RuntimeError(expr->op,
                         "Operands must be two numbers or two strings.");
    case SLASH:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double>(left) / std::any_cast<double>(right);
    case STAR:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double>(left) * std::any_cast<double>(right);
    default:
      break;
    }

    // Unreachable
    return {};
  }

  std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override {
    std::any right = evaluate(expr->right);

    switch (expr->op.type) {
    case MINUS:
      checkNumberOperand(expr->op, right);
      return -std::any_cast<double>(right);
    case BANG:
      return !isTruthy(right);

    default:
      // Unreachable
      return {};
    }
  }

  std::any visitCallExpr(std::shared_ptr<Call> expr) override {
    std::any callee = evaluate(expr->callee);

    std::vector<std::any> arguments{};
    for (const std::shared_ptr<Expr> &argument : expr->arguments) {
      arguments.push_back(evaluate(argument));
    }

    std::shared_ptr<LoxCallable> function;
    if (callee.type() == typeid(std::shared_ptr<LoxFunction>)) {
      function = std::any_cast<std::shared_ptr<LoxFunction>>(callee);
    } else {
      throw RuntimeError{expr->paren, "Can only call functions and classes."};
    }

    if (arguments.size() != function->arity()) {
      throw RuntimeError{expr->paren,
                         "Expected " + std::to_string(function->arity()) +
                             " arguments but got " +
                             std::to_string(arguments.size()) + "."};
    }
    return function->call(*this, std::move(arguments));
  }

  std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override {
    return expr->value;
  }

  std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override {
    return evaluate(expr->expression);
  }

  std::any visitVariableExpr(std::shared_ptr<Variable> expr) override {
    return lookUpVariable(expr->name, expr);
  }

private:
  // helpers
  std::any lookUpVariable(const Token &name,
                          const std::shared_ptr<Expr> &expr) {
    if (locals.contains(expr)) {
      int distance = locals[expr];
      return environment->getAt(distance, name.lexeme);
    }

    return globals->get(name);
  }

  void checkNumberOperand(const Token &op, const std::any &operand) {
    if (operand.type() == typeid(double)) {
      return;
    }

    throw RuntimeError(op, "Operand must be a number.");
  }

  void checkNumberOperands(const Token &op, const std::any &left,
                           const std::any &right) {
    if (left.type() == typeid(double) && right.type() == typeid(double)) {
      return;
    }

    throw RuntimeError(op, "Operands must be a number.");
  }

  bool isTruthy(const std::any &object) {
    if (object.type() == typeid(nullptr)) {
      return false;
    }
    if (object.type() == typeid(bool)) {
      return std::any_cast<bool>(object);
    }

    return true;
  }

  bool isEqual(std::any &a, std::any &b) {
    if (a.type() != b.type()) {
      return false;
    }

    if (a.type() == typeid(nullptr)) {
      return true;
    }
    if (a.type() == typeid(bool)) {
      return std::any_cast<bool>(a) == std::any_cast<bool>(b);
    }
    if (a.type() == typeid(double)) {
      // returns false for (NaN == NaN), unlike jlox
      return std::any_cast<double>(a) == std::any_cast<double>(b);
    }
    if (a.type() == typeid(std::string)) {
      return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
    }

    return false;
  }

  std::string stringify(std::any &object) {
    const auto &valueType = object.type();

    // Type narrowing with any_cast + converting to string
    if (valueType == typeid(nullptr)) {
      return "nil";
    }

    if (valueType == typeid(double)) {
      // uses std::format (C++20) to match jlox floating point error behaviour
      std::string text = std::format("{}", std::any_cast<double>(object));
      return text;
    }

    if (valueType == typeid(std::string)) {
      return std::any_cast<std::string>(object);
    }

    if (valueType == typeid(bool)) {
      return std::any_cast<bool>(object) ? "true" : "false";
    }

    if (valueType == typeid(std::shared_ptr<LoxFunction>)) {
      return std::any_cast<std::shared_ptr<LoxFunction>>(object)->toString();
    }

    return "Error in Interpreter.stringify(): unsupported object type.";
  }
};
