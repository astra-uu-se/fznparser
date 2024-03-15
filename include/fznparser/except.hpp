#pragma once

#include <stdexcept>

class FznException : public std::runtime_error {
 public:
  explicit FznException(const std::string& msg) : std::runtime_error(msg) {}
};