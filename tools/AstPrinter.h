#pragma once

#include "../src/Expr.h"
#include <any>
#include <cassert>
#include <sstream> // std::ostringstream
#include <string>
#include <utility>

class AstPrinter : public ExprVisitor {
public:
  std::string print(std::shared_ptr<Expr> expr) {
    // casting the return value of accept() to the correct type
    return std::any_cast<std::string>(expr->accept(*this));
  }

  std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override {
    return parenthesize(expr->op.lexeme, expr->left, expr->right);
  };

  std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override {
    return parenthesize("group", expr->expression);
  }

  std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override {
    const auto &valueType = expr->value.type();

    // Type narrowing with any_cast + converting to string
    if (valueType == typeid(nullptr)) {
      return "nil";
    }
    if (valueType == typeid(std::string)) {
      return std::any_cast<std::string>(expr->value);
    }
    if (valueType == typeid(double)) {
      return std::to_string(std::any_cast<double>(expr->value));
    }
    if (valueType == typeid(bool)) {
      return std::any_cast<bool>(expr->value) ? "true" : "false";
    }

    return "Error in AstPrinter::visitLiteralExpr: literal type not "
           "recognized.";
  }

  std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override {
    return parenthesize(expr->op.lexeme, expr->right);
  }

private:
  template <class... E>
  std::string parenthesize(std::string_view name, E... expr) {
    assert((... && std::is_same_v<E, std::shared_ptr<Expr>>));

    std::ostringstream builder;
    builder << '(' << name;
    (..., (builder << " " << print(std::move(expr))));
    builder << ")";

    return builder.str();
  }
};
