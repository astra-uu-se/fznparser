#include "fznparser/solveType.hpp"

#include <functional>
#include <numeric>

#include "fznparser/annotation.hpp"
#include "fznparser/except.hpp"

namespace fznparser {

SolveType::SolveType(std::vector<Annotation>&& annotations)
    : _annotations(std::move(annotations)),
      _problemType(ProblemType::SATISFY),
      _objective(std::nullopt) {}

SolveType::SolveType(const ProblemType problemType, const Var& var,
                     std::vector<Annotation>&& annotations)
    : _annotations(std::move(annotations)),
      _problemType(problemType),
      _objective(var) {}

bool SolveType::hasObjective() const noexcept { return _objective.has_value(); }

Var SolveType::objective() const {
  if (!hasObjective()) {
    throw FznException("No objective defined");
  }
  return _objective.value();
}

ProblemType SolveType::problemType() const { return _problemType; }
bool SolveType::isSatisfactionProblem() const {
  return _problemType == ProblemType::SATISFY;
}
bool SolveType::isOptimizationProblem() const {
  return !isSatisfactionProblem();
}
bool SolveType::isMaximisationProblem() const {
  return _problemType == ProblemType::MAXIMIZE;
}
bool SolveType::isMinimisationProblem() const {
  return _problemType == ProblemType::MINIMIZE;
}
const std::vector<Annotation>& SolveType::annotations() const {
  return _annotations;
}

bool SolveType::operator==(const SolveType& other) const {
  if (_annotations.size() != other._annotations.size() ||
      _problemType != other._problemType ||
      hasObjective() != other.hasObjective()) {
    return false;
  }
  if (hasObjective()) {
    return other.hasObjective() && objective().operator==(other.objective());
  }
  for (size_t i = 0; i < _annotations.size(); ++i) {
    if (_annotations[i].operator!=(other._annotations[i])) {
      return false;
    }
  }
  return true;
}

bool SolveType::operator!=(const SolveType& other) const {
  return !operator==(other);
}

}  // namespace fznparser