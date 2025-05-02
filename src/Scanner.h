#pragma once

#include "Error.h"
#include "Token.h"
#include "TokenType.h"
#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class Scanner {
  static const std::map<std::string, TokenType> keywords;

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

    tokens.emplace_back(END_OF_FILE, "", nullptr, line);
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
        while (peek() != '\n' && !isAtEnd()) {
          advance();
        }
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

    case '"':
      string();
      break;

    default:
      if (isDigit(c)) {
        number();
      } else if (isAlpha(c)) {
        identifier();
      } else {
        error(line, "Unexpected character.");
      }
      break;
    }
  }

  void identifier() {
    while (isAlphaNumeric(peek())) {
      advance();
    }

    std::string text(source.substr(start, current - start));

    TokenType type;
    auto match = keywords.find(text);
    if (match == keywords.end()) {
      type = IDENTIFIER;
    } else {
      type = match->second;
    }

    addToken(type);
  }

  void number() {
    while (isDigit(peek())) {
      advance();
    }

    if (peek() == '.' && isDigit(peekNext())) {
      // Consume the '.'
      advance();

      while (isDigit(peek())) {
        advance();
      }
    }

    std::string strValue(source.substr(start, current - start));
    double value = std::stod(strValue);
    addToken(NUMBER, value);
  }

  void string() {
    while (peek() != '"' && !isAtEnd()) {
      if (peek() == '\n') {
        line++;
      }
      advance();
    }

    if (isAtEnd()) {
      error(line, "Unterminated string.");
      return;
    }

    // Consume closing '"'
    advance();

    // Trim surrounding quotes
    std::string value{source.substr(start + 1, current - start - 2)};
    addToken(STRING, value);
  }

  bool match(char expected) {
    if (isAtEnd()) {
      return false;
    }
    if (source.at(current) != expected) {
      return false;
    }

    current++;
    return true;
  }

  char peek() {
    if (isAtEnd()) {
      return '\0';
    }
    return source.at(current);
  }

  char peekNext() {
    if (current + 1 >= source.size()) {
      return '\0';
    }
    return source.at(current + 1);
  }

  bool isAlpha(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_');
  }

  bool isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }

  bool isDigit(char c) { return '0' <= c && c <= '9'; }

  bool isAtEnd() { return current >= source.size(); }

  char advance() { return source.at(current++); }

  void addToken(TokenType type) { addToken(type, nullptr); }

  void addToken(TokenType type, std::any literal) {
    std::string text{source.substr(start, current - start)};
    tokens.emplace_back(type, std::move(text), std::move(literal), line);
  }
};

const std::map<std::string, TokenType> Scanner::keywords = {
    {"and", AND},   {"class", CLASS}, {"else", ELSE},     {"false", FALSE},
    {"for", FOR},   {"fun", FUN},     {"if", IF},         {"nil", NIL},
    {"or", OR},     {"print", PRINT}, {"return", RETURN}, {"super", SUPER},
    {"this", THIS}, {"true", TRUE},   {"var", VAR},       {"while", WHILE},
};
