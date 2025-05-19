#pragma once

#include "Error.h"
#include "Interpreter.h"
#include <map>
#include <vector>

class Resolver : public ExprVisitor, public StmtVisitor {
  Interpreter &interpreter;
  std::vector<std::map<std::string, bool>> scopes;

  enum class FunctionType : std::uint8_t {
    NONE,
    FUNCTION,
  };

  FunctionType currentFunction = FunctionType::NONE;

public:
  Resolver(Interpreter &interpreter) : interpreter{interpreter} {};

  void resolve(const std::vector<std::shared_ptr<Stmt>> &statements) {
    for (const std::shared_ptr<Stmt> &statement : statements) {
      resolve(statement);
    }
  }

  std::any visitBlockStmt(const std::shared_ptr<Block> stmt) override {
    beginScope();
    resolve(stmt->statements);
    endScope();
    return {};
  }

  std::any
  visitExpressionStmt(const std::shared_ptr<Expression> stmt) override {
    resolve(stmt->expression);
    return {};
  }

  std::any visitFunctionStmt(const std::shared_ptr<Function> stmt) override {
    declare(stmt->name);
    define(stmt->name);

    resolveFunction(stmt, FunctionType::FUNCTION);
    return {};
  }

  std::any visitIfStmt(const std::shared_ptr<If> stmt) override {
    resolve(stmt->condition);
    resolve(stmt->thenBranch);
    if (stmt->elseBranch != nullptr) {
      resolve(stmt->elseBranch);
    }
    return {};
  }

  std::any visitPrintStmt(const std::shared_ptr<Print> stmt) override {
    resolve(stmt->expression);
    return {};
  }

  std::any visitReturnStmt(const std::shared_ptr<Return> stmt) override {
    if (currentFunction == FunctionType::NONE) {
      error(stmt->keyword, "Can't return from top-level code.");
    }

    if (stmt->value != nullptr) {
      resolve(stmt->value);
    }
    return {};
  }

  std::any visitVarStmt(const std::shared_ptr<Var> stmt) override {
    declare(stmt->name);
    if (stmt->initializer != nullptr) {
      resolve(stmt->initializer);
    }
    define(stmt->name);
    return {};
  }

  std::any visitWhileStmt(const std::shared_ptr<While> stmt) override {
    resolve(stmt->condition);
    resolve(stmt->body);
    return {};
  }

  std::any visitAssignExpr(const std::shared_ptr<Assign> expr) override {
    resolve(expr->value);
    resolveLocal(expr, expr->name);
    return {};
  }

  std::any visitBinaryExpr(const std::shared_ptr<Binary> expr) override {
    resolve(expr->left);
    resolve(expr->right);
    return {};
  }

  std::any visitCallExpr(const std::shared_ptr<Call> expr) override {
    resolve(expr->callee);

    for (const std::shared_ptr<Expr> &argument : expr->arguments) {
      resolve(argument);
    }

    return {};
  }

  std::any visitGroupingExpr(const std::shared_ptr<Grouping> expr) override {
    resolve(expr->expression);
    return {};
  }

  std::any visitLiteralExpr(
      [[maybe_unused]] const std::shared_ptr<Literal> expr) override {
    return {};
  }

  std::any visitLogicalExpr(const std::shared_ptr<Logical> expr) override {
    resolve(expr->left);
    resolve(expr->right);
    return {};
  }

  std::any visitUnaryExpr(const std::shared_ptr<Unary> expr) override {
    resolve(expr->right);
    return {};
  }

  std::any visitVariableExpr(const std::shared_ptr<Variable> expr) override {
    if (!scopes.empty()) {
      std::map<std::string, bool> scope = scopes.back();
      if (scope.contains(expr->name.lexeme) && !scope[expr->name.lexeme]) {
        error(expr->name, "Can't read local variable in its own initializer.");
      }
    }

    resolveLocal(expr, expr->name);
    return {};
  }

private:
  void resolve(const std::shared_ptr<Stmt> &stmt) { stmt->accept(*this); }

  void resolve(const std::shared_ptr<Expr> &expr) { expr->accept(*this); }

  void resolveFunction(const std::shared_ptr<Function> &function,
                       FunctionType type) {
    FunctionType enclosingFunction = currentFunction;
    currentFunction = type;

    beginScope();
    for (const Token &param : function->params) {
      declare(param);
      define(param);
    }
    resolve(function->body);
    endScope();

    currentFunction = enclosingFunction;
  }

  void beginScope() { scopes.push_back(std::map<std::string, bool>{}); }

  void endScope() { scopes.pop_back(); }

  void declare(const Token &name) {
    if (scopes.empty()) {
      return;
    }

    std::map<std::string, bool> scope = scopes.back();
    if (scope.contains(name.lexeme)) {
      error(name,
            "A variable with this name already exists in the current scope.");
    }

    scope[name.lexeme] = false;
  }

  void define(const Token &name) {
    if (scopes.empty()) {
      return;
    }

    std::map<std::string, bool> scope = scopes.back();
    scope[name.lexeme] = true;
  }

  void resolveLocal(const std::shared_ptr<Expr> &expr, const Token &name) {
    for (int i = scopes.size() - 1; i >= 0; i--) {
      if (scopes[i].contains(name.lexeme)) {
        interpreter.resolve(expr, scopes.size() - 1 - i);
        return;
      }
    }
  }
};
