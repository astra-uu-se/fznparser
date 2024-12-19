#pragma once

#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "constraint.hpp"
#include "fznparser/solveType.hpp"
#include "fznparser/types.hpp"

namespace fznparser {

class Var;
class BoolVar;
class IntVar;
class FloatVar;
class SetVar;

class Model {
  std::unordered_map<std::string, Var> _vars;
  std::vector<Constraint> _constraints;
  SolveType _solveType;

  std::array<std::shared_ptr<BoolVar>, 2> _boolVarPars;
  std::unordered_map<int64_t, std::shared_ptr<IntVar>> _intVarPars;
  std::unordered_map<double, std::shared_ptr<FloatVar>> _floatVarPars;
  std::vector<std::shared_ptr<SetVar>> _setVarPars;

 public:
  Model(const Model&) = default;
  Model(Model&&) = default;

  Model();
  Model(std::vector<Var>&& vars, std::vector<Constraint>&& constraints,
        SolveType&& solveType);
  Model(std::unordered_map<std::string, Var>&& vars,
        std::vector<Constraint>&& constraints, SolveType&& solveType);
  Model(Var&& objective, ProblemType);

  std::shared_ptr<BoolVar> boolVarPar(bool);
  std::shared_ptr<IntVar> addIntVarPar(int64_t);
  std::shared_ptr<FloatVar> addFloatVarPar(double);
  std::shared_ptr<SetVar> addSetVarPar(const IntSet&);

  Var addVar(Var&& var);
  const Constraint& addConstraint(Constraint&& constraint);

  size_t numVars() const;
  size_t numConstraints() const;

  bool hasIdentifier(const std::string& var) const;

  bool hasVar(const std::string&) const;
  Var var(const std::string& identifier) const;

  const std::unordered_map<std::string, Var>& vars() const;
  const std::vector<Constraint>& constraints() const;
  const SolveType& solveType() const;

  bool hasObjective() const noexcept;
  Var objective() const;
  bool isSatisfactionProblem() const;
  bool isOptimizationProblem() const;
  bool isMaximisationProblem() const;
  bool isMinimisationProblem() const;

  bool operator==(const Model&) const;
  bool operator!=(const Model&) const;
};
}  // namespace fznparser