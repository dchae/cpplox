// GenerateAst.cpp > defineAst()
#pragma once

#include "Token.h"
#include <any>
#include <memory>  // std::shared_ptr
#include <utility> // std::move

struct Grouping;
struct Binary;
struct Unary;
struct Literal;
struct Variable;

// GenerateAst.cpp > defineVisitor()
struct ExprVisitor {
  virtual std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) = 0;
  virtual std::any visitBinaryExpr(std::shared_ptr<Binary> expr) = 0;
  virtual std::any visitUnaryExpr(std::shared_ptr<Unary> expr) = 0;
  virtual std::any visitLiteralExpr(std::shared_ptr<Literal> expr) = 0;
  virtual std::any visitVariableExpr(std::shared_ptr<Variable> expr) = 0;

  virtual ~ExprVisitor() = default;
};

struct Expr {
  virtual std::any accept(ExprVisitor &visitor) = 0;
  virtual ~Expr() = default;
};

// GenerateAst.cpp > defineType()
struct Grouping : Expr, public std::enable_shared_from_this<Grouping> {
  Grouping(std::shared_ptr<Expr> expression)
      : expression{std::move(expression)} {}

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitGroupingExpr(shared_from_this());
  }

  const std::shared_ptr<Expr> expression;
};

struct Binary : Expr, public std::enable_shared_from_this<Binary> {
  Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
      : left{std::move(left)}, op{std::move(op)}, right{std::move(right)} {}

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitBinaryExpr(shared_from_this());
  }

  const std::shared_ptr<Expr> left;
  const Token op;
  const std::shared_ptr<Expr> right;
};

struct Unary : Expr, public std::enable_shared_from_this<Unary> {
  Unary(Token op, std::shared_ptr<Expr> right)
      : op{std::move(op)}, right{std::move(right)} {}

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitUnaryExpr(shared_from_this());
  }

  const Token op;
  const std::shared_ptr<Expr> right;
};

struct Literal : Expr, public std::enable_shared_from_this<Literal> {
  Literal(std::any value)
      : value{std::move(value)} {}

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitLiteralExpr(shared_from_this());
  }

  const std::any value;
};

struct Variable : Expr, public std::enable_shared_from_this<Variable> {
  Variable(Token name)
      : name{std::move(name)} {}

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitVariableExpr(shared_from_this());
  }

  const Token name;
};

