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

#include "fznparser/constraint.hpp"
#include "fznparser/except.hpp"
#include "fznparser/solveType.hpp"
#include "fznparser/types.hpp"
#include "fznparser/variables.hpp"

namespace fznparser {

class Model {
 private:
  std::unordered_map<std::string_view, Variable> _variables;
  std::vector<Constraint> _constraints;
  SolveType _solveType;

  std::array<BoolVar, 2> _boolVarPars;
  std::unordered_map<int64_t, IntVar> _intVarPars;
  std::unordered_map<double, FloatVar> _floatVarPars;
  std::vector<SetVar> _setVarPars;

 public:
  Model(const Model&) = default;
  Model(Model&&) = default;

  Model(std::unordered_map<std::string_view, Variable>&& variables,
        std::vector<Constraint>&& constraints, SolveType&& solveType);

  BoolVar& boolVarPar(bool);
  IntVar& addIntVarPar(int64_t);
  FloatVar& addFloatVarPar(double);
  const SetVar& addSetVarPar(const IntSet&);

  size_t numVariables() const;
  size_t numConstraints() const;

  bool hasVariable(std::string_view variable) const;
  const Variable& variable(std::string_view identifier) const;

  const std::unordered_map<std::string_view, Variable>& variables() const;
  const std::vector<Constraint>& constraints() const;
  const SolveType& solveType() const;
  const std::optional<Variable> objective() const;
  bool isSatisfactionProblem() const;
  bool isOptimizationProblem() const;
  bool isMaximisationProblem() const;
  bool isMinimisationProblem() const;

  bool operator==(const Model&) const;
  bool operator!=(const Model&) const;
  std::string toString() const;
};
}  // namespace fznparser