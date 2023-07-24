#include "fznparser/model.hpp"

namespace fznparser {

Model::Model(std::unordered_map<std::string_view, Variable>&& variables,
             std::vector<Constraint>&& constraints, SolveType&& solveType)
    : _variables(std::move(variables)),
      _constraints(std::move(constraints)),
      _solveType(std::move(solveType)),
      _boolVarPars{BoolVar(false, ""), BoolVar(true, "")} {}

BoolVar& Model::boolVarPar(bool b) { return _boolVarPars.at(b ? 1 : 0); }

IntVar& Model::addIntVarPar(int64_t i) {
  if (_intVarPars.find(i) == _intVarPars.end()) {
    _intVarPars.emplace(i, IntVar(i, ""));
  }
  return _intVarPars.at(i);
}

FloatVar& Model::addFloatVarPar(double f) {
  if (_floatVarPars.find(f) == _floatVarPars.end()) {
    _floatVarPars.emplace(f, FloatVar(f, ""));
  }
  return _floatVarPars.at(f);
}

const SetVar& Model::addSetVarPar(const IntSet& is) {
  return _setVarPars.emplace_back(SetVar(is, ""));
}

size_t Model::numVariables() const { return _variables.size(); }
size_t Model::numConstraints() const { return _constraints.size(); }
const Variable& Model::variable(std::string_view identifier) const {
  return _variables.at(identifier);
}

bool Model::hasVariable(std::string_view identifier) const {
  return _variables.find(identifier) != _variables.end();
}

const std::unordered_map<std::string_view, Variable>& Model::variables() const {
  return _variables;
}

const std::vector<Constraint>& Model::constraints() const {
  return _constraints;
}

const SolveType& Model::solveType() const { return _solveType; }

const std::optional<Variable> Model::objective() const {
  return _solveType.objective();
}
bool Model::isSatisfactionProblem() const {
  return _solveType.isSatisfactionProblem();
}
bool Model::isOptimizationProblem() const {
  return _solveType.isOptimizationProblem();
}
bool Model::isMaximisationProblem() const {
  return _solveType.isMaximisationProblem();
}
bool Model::isMinimisationProblem() const {
  return _solveType.isMinimisationProblem();
}

bool Model::operator==(const Model& other) const {
  if (_variables.size() != other._variables.size() ||
      _constraints.size() != other._constraints.size() ||
      _solveType != other._solveType) {
    return false;
  }
  for (const auto& [identifier, var] : _variables) {
    if (other._variables.find(identifier) == other._variables.end() ||
        var.operator!=(other._variables.at(identifier))) {
      return false;
    }
  }
  for (const Constraint& con : _constraints) {
    bool found = false;
    for (const Constraint& otherCon : other._constraints) {
      if (con.operator==(otherCon)) {
        found = true;
        break;
      }
    }
    if (!found) {
      return false;
    }
  }
  if (!_solveType.operator==(other._solveType)) {
    return false;
  }
  return true;
}

bool Model::operator!=(const Model& other) const { return !operator==(other); }

std::string Model::toString() const {
  std::string s = "";
  for (const auto& [identifier, var] : _variables) {
    s += var.toString() + ";\n";
  }
  for (const Constraint& con : _constraints) {
    s += con.toString() + ";\n";
  }
  s += _solveType.toString() + ";\n";
  return s;
}
}  // namespace fznparser