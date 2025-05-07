#include "NativeClock.h"
#include <chrono>

size_t NativeClock::arity() { return 0; };

std::any NativeClock::call([[maybe_unused]] Interpreter &interpreter,
                           [[maybe_unused]] std::vector<std::any> arguments) {
  auto ticks = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration<double>{ticks}.count() / 1000.0;
}

std::string NativeClock::toString() { return "<native fn>"; }
