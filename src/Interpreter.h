#pragma once

#include "Expr.h"
#include "RuntimeError.h"
#include <any>
#include <memory> // std::shared_ptr

class Interpreter : public ExprVisitor {
public:
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
    std::any right = evaluate(expr->left);

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

private:
  void checkNumberOperand(const Token &op, const std::any &operand) {
    if (operand.type() == typeid(double))
      return;

    throw RuntimeError(op, "Operand must be a number.");
  }

  void checkNumberOperands(const Token &op, const std::any &left,
                           const std::any &right) {
    if (left.type() == typeid(double) && right.type() == typeid(double))
      return;

    throw RuntimeError(op, "Operands must be a number.");
  }

  bool isTruthy(std::any &object) {
    if (object.type() == typeid(nullptr))
      return false;
    if (object.type() == typeid(bool))
      return std::any_cast<bool>(object);

    return true;
  }

  bool isEqual(std::any &a, std::any &b) {
    if (a.type() != b.type())
      return false;

    if (a.type() == typeid(nullptr))
      return true;
    if (a.type() == typeid(bool))
      return std::any_cast<bool>(a) == std::any_cast<bool>(b);
    if (a.type() == typeid(double))
      // returns false for (NaN == NaN), unlike jlox
      return std::any_cast<double>(a) == std::any_cast<double>(b);
    if (a.type() == typeid(std::string))
      return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);

    return false;
  }

  std::any evaluate(std::shared_ptr<Expr> expr) {
    // send expression back into the visitor implementation
    return expr->accept(*this);
  }
};
