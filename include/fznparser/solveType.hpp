#pragma once

#include <string>
#include <vector>
#include <memory>

#include "fznparser/annotation.hpp"
#include "fznparser/except.hpp"
#include "fznparser/variables.hpp"

namespace fznparser {

enum class ProblemType : unsigned char { SATISFY, MINIMIZE, MAXIMIZE };

class SolveType {
  std::vector<Annotation> _annotations;
  ProblemType _problemType;
  std::optional<Var> _objective;

 public:
  SolveType(const SolveType&) = default;
  SolveType(SolveType&&) = default;

  SolveType(std::vector<Annotation>&& = {});
  SolveType(ProblemType, const Var, std::vector<Annotation>&& = {});

  bool hasObjective() const noexcept;
  const Var objective() const;

  ProblemType problemType() const;
  bool isSatisfactionProblem() const;
  bool isOptimizationProblem() const;
  bool isMaximisationProblem() const;
  bool isMinimisationProblem() const;
  const std::vector<Annotation>& annotations() const;

  bool operator==(const SolveType&) const;
  bool operator!=(const SolveType&) const;
  std::string toString() const;
};

}  // namespace fznparser