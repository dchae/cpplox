// GenerateAst.cpp > defineAst()
#pragma once

#include "Expr.h"
#include <any>
#include <memory>  // std::shared_ptr
#include <utility> // std::move

struct Expression;
struct Print;

// GenerateAst.cpp > defineVisitor()
struct StmtVisitor {
  virtual std::any visitExpressionStmt(std::shared_ptr<Expression> stmt) = 0;
  virtual std::any visitPrintStmt(std::shared_ptr<Print> stmt) = 0;

  virtual ~StmtVisitor() = default;
};

struct Stmt {
  virtual std::any accept(StmtVisitor &visitor) = 0;
  virtual ~Stmt() = default;
};

// GenerateAst.cpp > defineType()
struct Expression : Stmt, public std::enable_shared_from_this<Expression> {
  Expression(std::shared_ptr<Expr> expression)
      : expression{std::move(expression)} {}

  std::any accept(StmtVisitor &visitor) override {
    return visitor.visitExpressionStmt(shared_from_this());
  }

  const std::shared_ptr<Expr> expression;
};

struct Print : Stmt, public std::enable_shared_from_this<Print> {
  Print(std::shared_ptr<Expr> expression)
      : expression{std::move(expression)} {}

  std::any accept(StmtVisitor &visitor) override {
    return visitor.visitPrintStmt(shared_from_this());
  }

  const std::shared_ptr<Expr> expression;
};

