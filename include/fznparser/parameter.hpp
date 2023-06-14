#pragma once

#include <string_view>
#include <variant>
#include <vector>

#include "fznparser/intSet.hpp"

namespace fznparser {

class Parameter {
 private:
  const std::string_view identifier;
  const std::variant<bool, int64_t, double, IntSet, std::vector<bool>,
                     std::vector<int64_t>, std::vector<double>,
                     std::vector<IntSet>>
      value;

 public:
  Parameter(const std::string_view &id, const bool val)
      : identifier(id), value(val) {}
  Parameter(const std::string_view &id, const int64_t val)
      : identifier(id), value(val) {}
  Parameter(const std::string_view &id, const double val)
      : identifier(id), value(val) {}
  Parameter(const std::string_view &id, IntSet &&val)
      : identifier(id), value(std::move(val)) {}
  Parameter(const std::string_view &id, std::vector<bool> &&val)
      : identifier(id), value(std::move(val)) {}
  Parameter(const std::string_view &id, std::vector<int64_t> &&val)
      : identifier(id), value(std::move(val)) {}
  Parameter(const std::string_view &id, std::vector<double> &&val)
      : identifier(id), value(std::move(val)) {}
  Parameter(const std::string_view &id, std::vector<IntSet> &&val)
      : identifier(id), value(std::move(val)) {}
};
}  // namespace fznparser