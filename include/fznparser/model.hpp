#pragma once

#include <array>
#include <functional>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string>
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
  std::unordered_map<std::string, Var> _vars;
  std::vector<Constraint> _constraints;
  SolveType _solveType;

  std::array<BoolVar, 2> _boolVarPars;
  std::unordered_map<int64_t, IntVar> _intVarPars;
  std::unordered_map<double, FloatVar> _floatVarPars;
  std::vector<SetVar> _setVarPars;

 public:
  Model(const Model&) = default;
  Model(Model&&) = default;

  Model();
  Model(std::vector<Var>&& vars, std::vector<Constraint>&& constraints,
        SolveType&& solveType);
  Model(std::unordered_map<std::string, Var>&& vars,
        std::vector<Constraint>&& constraints, SolveType&& solveType);
  Model(Var&& objective, ProblemType);

  BoolVar& boolVarPar(bool);
  IntVar& addIntVarPar(int64_t);
  FloatVar& addFloatVarPar(double);
  SetVar& addSetVarPar(const IntSet&);

  const Var& addVar(Var&& var);
  const Constraint& addConstraint(Constraint&& constraint);

  size_t numVars() const;
  size_t numConstraints() const;

  bool hasVar(std::string var) const;
  const Var& var(std::string identifier) const;

  const std::unordered_map<std::string, Var>& vars() const;
  const std::vector<Constraint>& constraints() const;
  const SolveType& solveType() const;

  bool hasObjective() const noexcept;
  const Var& objective() const;
  bool isSatisfactionProblem() const;
  bool isOptimizationProblem() const;
  bool isMaximisationProblem() const;
  bool isMinimisationProblem() const;

  bool operator==(const Model&) const;
  bool operator!=(const Model&) const;
  std::string toString() const;
};
}  // namespace fznparser