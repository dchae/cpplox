// GenerateAst.cpp > defineAst()
#pragma once

#include "Token.h"
#include <any>
#include <memory>  // std::shared_ptr
#include <utility> // std::move
#include <vector>

struct Assign;
struct Binary;
struct Call;
struct Grouping;
struct Literal;
struct Logical;
struct Unary;
struct Variable;

// GenerateAst.cpp > defineVisitor()
struct ExprVisitor {
  virtual std::any visitAssignExpr(std::shared_ptr<Assign> expr) = 0;
  virtual std::any visitBinaryExpr(std::shared_ptr<Binary> expr) = 0;
  virtual std::any visitCallExpr(std::shared_ptr<Call> expr) = 0;
  virtual std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) = 0;
  virtual std::any visitLiteralExpr(std::shared_ptr<Literal> expr) = 0;
  virtual std::any visitLogicalExpr(std::shared_ptr<Logical> expr) = 0;
  virtual std::any visitUnaryExpr(std::shared_ptr<Unary> expr) = 0;
  virtual std::any visitVariableExpr(std::shared_ptr<Variable> expr) = 0;

  virtual ~ExprVisitor() = default;
};

struct Expr {
  virtual std::any accept(ExprVisitor &visitor) = 0;
  virtual ~Expr() = default;
};

// GenerateAst.cpp > defineType()
struct Assign : Expr, public std::enable_shared_from_this<Assign> {
  Assign(Token name, std::shared_ptr<Expr> value)
      : name{std::move(name)}, value{std::move(value)} {}

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitAssignExpr(shared_from_this());
  }

  const Token name;
  const std::shared_ptr<Expr> value;
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

struct Call : Expr, public std::enable_shared_from_this<Call> {
  Call(std::shared_ptr<Expr> callee, Token paren, std::vector<std::shared_ptr<Expr>> arguments)
      : callee{std::move(callee)}, paren{std::move(paren)}, arguments{std::move(arguments)} {}

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitCallExpr(shared_from_this());
  }

  const std::shared_ptr<Expr> callee;
  const Token paren;
  const std::vector<std::shared_ptr<Expr>> arguments;
};

struct Grouping : Expr, public std::enable_shared_from_this<Grouping> {
  Grouping(std::shared_ptr<Expr> expression)
      : expression{std::move(expression)} {}

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitGroupingExpr(shared_from_this());
  }

  const std::shared_ptr<Expr> expression;
};

struct Literal : Expr, public std::enable_shared_from_this<Literal> {
  Literal(std::any value)
      : value{std::move(value)} {}

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitLiteralExpr(shared_from_this());
  }

  const std::any value;
};

struct Logical : Expr, public std::enable_shared_from_this<Logical> {
  Logical(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
      : left{std::move(left)}, op{std::move(op)}, right{std::move(right)} {}

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitLogicalExpr(shared_from_this());
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

struct Variable : Expr, public std::enable_shared_from_this<Variable> {
  Variable(Token name)
      : name{std::move(name)} {}

  std::any accept(ExprVisitor &visitor) override {
    return visitor.visitVariableExpr(shared_from_this());
  }

  const Token name;
};

