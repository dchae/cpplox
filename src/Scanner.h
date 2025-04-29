#pragma once

#include "Error.h"
#include "Token.h"
#include "TokenType.h"
#include <array>
#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class Scanner {
  std::string_view source;
  std::vector<Token> tokens;
  size_t start = 0;
  size_t current = 0;
  int line = 1;

public:
  Scanner(std::string_view source) : source{source} {};

  std::vector<Token> scanTokens() {
    while (!isAtEnd()) {
      // We are at the beginning of the next lexeme
      start = current;
      scanToken();
    }

    tokens.push_back(Token(END_OF_FILE, "", nullptr, line));
    return tokens;
  }

private:
  void scanToken() {
    char c = advance();

    switch (c) {
    case '(':
      addToken(LEFT_PAREN);
      break;
    case ')':
      addToken(RIGHT_PAREN);
      break;
    case '{':
      addToken(LEFT_BRACE);
      break;
    case '}':
      addToken(RIGHT_BRACE);
      break;
    case ',':
      addToken(COMMA);
      break;
    case '.':
      addToken(DOT);
      break;
    case '-':
      addToken(MINUS);
      break;
    case '+':
      addToken(PLUS);
      break;
    case ';':
      addToken(SEMICOLON);
      break;
    case '*':
      addToken(STAR);
      break;

    case '!':
      addToken(match('=') ? BANG_EQUAL : BANG);
      break;
    case '=':
      addToken(match('=') ? EQUAL_EQUAL : EQUAL);
      break;
    case '<':
      addToken(match('=') ? LESS_EQUAL : LESS);
      break;
    case '>':
      addToken(match('=') ? GREATER_EQUAL : GREATER);
      break;

    case '/':
      // ignore comments
      if (match('/')) {
        while (peek() != '\n' && !isAtEnd())
          advance();
      } else {
        addToken(SLASH);
      }
      break;

    // ignore whitespace
    case ' ':
    case '\r':
    case '\t':
      break;
    case '\n':
      line++;
      break;

    default:
      error(line, "Unexpected character.");
      break;
    }
  }

  bool match(char expected) {
    if (isAtEnd())
      return false;
    if (source[current] != expected)
      return false;

    current++;
    return true;
  }

  char peek() {
    if (isAtEnd())
      return '\0';
    return source[current];
  }

  bool isAtEnd() { return current >= source.size(); }

  char advance() { return source[current++]; }

  void addToken(TokenType type) { addToken(type, nullptr); }

  void addToken(TokenType type, std::any literal) {
    std::string text{source.substr(start, current - start)};
    tokens.push_back(Token(type, std::move(text), std::move(literal), line));
  }
};
