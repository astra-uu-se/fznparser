#include "fznparser/solveType.hpp"

namespace fznparser {

SolveType::SolveType(std::vector<fznparser::Annotation>&& annotations)
    : _annotations(std::move(annotations)),
      _problemType(ProblemType::SATISFY),
      _objective({}) {}

SolveType::SolveType(ProblemType problemType, const Var& var,
                     std::vector<fznparser::Annotation>&& annotations)
    : _annotations(std::move(annotations)),
      _problemType(problemType),
      _objective(var) {}

const std::optional<Var> SolveType::objective() const {
  if (_objective.has_value()) {
    return _objective.value().get();
  }
  return std::nullopt;
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
      _objective.has_value() != other._objective.has_value()) {
    return false;
  }
  if (_objective.has_value()) {
    return other._objective.has_value() &&
           _objective.value().get().operator==(other._objective.value().get());
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

std::string SolveType::toString() const {
  std::string s = "solve ";
  if (_problemType == ProblemType::SATISFY) {
    s += "satisfy";
  } else {
    s += (_problemType == ProblemType::MINIMIZE ? "minimize" : "maximize");
    if (_objective.has_value()) {
      s += std::string(_objective.value().get().identifier());
    }
  }
  if (!_annotations.empty()) {
    for (const auto& annotation : _annotations) {
      s += " :: " + annotation.toString();
    }
  }
  return s;
}

}  // namespace fznparser