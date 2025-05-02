#pragma once

#include "Error.h"
#include "Expr.h"
#include "RuntimeError.h"
#include <any>
#include <format> // std::format (c++20)
#include <iostream>
#include <memory> // std::shared_ptr

class Interpreter : public ExprVisitor {
public:
  void interpret(const std::shared_ptr<Expr> &expression) {
    try {
      std::any value = evaluate(expression);
      std::cout << stringify(value) << "\n";
    } catch (RuntimeError error) {
      runtimeError(error);
    }
  }

  std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override {
    return expr->value;
  }

  std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override {
    return evaluate(expr->expression);
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
      if (left.type() == typeid(std::string) ||
          right.type() == typeid(std::string)) {
        return stringify(left) + stringify(right);
      }
      if (left.type() == typeid(double) && right.type() == typeid(double)) {
        return std::any_cast<double>(left) + std::any_cast<double>(right);
      }

      throw RuntimeError(
          expr->op, "Operands must be both numbers or at least one string.");
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

private:
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

  bool isTruthy(std::any &object) {
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

    return "Error in Interpreter.stringify(): unsupported object type.";
  }

  std::any evaluate(const std::shared_ptr<Expr> &expr) {
    // send expression back into the visitor implementation
    return expr->accept(*this);
  }
};
