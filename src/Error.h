#pragma once

#include <iostream>
#include <string_view>

inline bool hadError = false;

inline void report(int line, std::string_view where, std::string_view message) {
  std::cerr << "[line " << line << "] Error" << where << ": " << message
            << std::endl;
  hadError = true;
}

inline void error(int line, std::string_view message) {
  report(line, "", message);
}
