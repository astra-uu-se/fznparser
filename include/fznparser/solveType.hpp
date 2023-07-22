#pragma once

#include <array>
#include <functional>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "fznparser/annotation.hpp"
#include "fznparser/except.hpp"
#include "fznparser/variables.hpp"

namespace fznparser {

enum class ProblemType : unsigned char { SATISFY, MINIMIZE, MAXIMIZE };

class SolveType {
  std::vector<Annotation> _annotations;
  ProblemType _problemType;
  std::optional<std::reference_wrapper<const Variable>> _objective;

 public:
  SolveType(const SolveType&) = default;
  SolveType(SolveType&&) = default;

  SolveType(std::vector<Annotation>&& = {});
  SolveType(ProblemType, const Variable&, std::vector<Annotation>&& = {});

  bool operator==(const SolveType&) const;
  bool operator!=(const SolveType&) const;
  std::string toString() const;
};

}  // namespace fznparser