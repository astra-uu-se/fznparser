#pragma once

#include <optional>
#include <string>
#include <vector>

#include "fznparser/annotation.hpp"
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

  explicit SolveType(std::vector<Annotation>&& = {});
  SolveType(ProblemType, const Var&, std::vector<Annotation>&& = {});

  [[nodiscard]] bool hasObjective() const noexcept;
  [[nodiscard]] Var objective() const;

  [[nodiscard]] ProblemType problemType() const;
  [[nodiscard]] bool isSatisfactionProblem() const;
  [[nodiscard]] bool isOptimizationProblem() const;
  [[nodiscard]] bool isMaximisationProblem() const;
  [[nodiscard]] bool isMinimisationProblem() const;
  [[nodiscard]] const std::vector<Annotation>& annotations() const;

  bool operator==(const SolveType&) const;
  bool operator!=(const SolveType&) const;
  [[nodiscard]] std::string toString() const;
};

}  // namespace fznparser