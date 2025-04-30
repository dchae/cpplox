#pragma once

#include "Token.h"
#include <any>
#include <memory>
#include <utility> // std::move
#include <vector>

struct Binary;
struct Grouping;
struct Literal;
struct Unary;

struct Expr {
};

struct Binary : Expr, public std::enable_shared_from_this<Binary> {
  Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
      : left{std::move(left)}, op{std::move(op)}, right{std::move(right)} {}

  const std::shared_ptr<Expr> left;
  const Token op;
  const std::shared_ptr<Expr> right;
};

struct Grouping : Expr, public std::enable_shared_from_this<Grouping> {
  Grouping(std::shared_ptr<Expr> expression)
      : expression{std::move(expression)} {}

  const std::shared_ptr<Expr> expression;
};

struct Literal : Expr, public std::enable_shared_from_this<Literal> {
  Literal(std::any value)
      : value{std::move(value)} {}

  const std::any value;
};

struct Unary : Expr, public std::enable_shared_from_this<Unary> {
  Unary(Token op, std::shared_ptr<Expr> right)
      : op{std::move(op)}, right{std::move(right)} {}

  const Token op;
  const std::shared_ptr<Expr> right;
};

