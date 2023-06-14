#pragma once

#include <string_view>
#include <vector>

namespace fznparser {
class Predicate {
 public:
  const std::string_view name;
  std::vector<Parameter> parameters;
};
}  // namespace fznparser