#include "fznparser/model.hpp"

namespace fznparser {

std::unordered_map<std::string_view, Variable> varToMap(Variable&& var) {
  std::unordered_map<std::string_view, Variable> varMap;
  varMap.emplace(var.identifier(), std::move(var));
  return varMap;
}

std::unordered_map<std::string_view, Variable> varVectorToMap(
    std::vector<Variable>&& vars) {
  std::unordered_map<std::string_view, Variable> varMap;
  for (Variable& var : vars) {
    varMap.emplace(var.identifier(), std::move(var));
  }
  return varMap;
}

const Variable& varInMap(
    const std::unordered_map<std::string_view, Variable>& vars) {
  if (vars.size() != 1) {
    throw FznException("Invalid initialization: expected exactly one variable");
  }
  for (const auto& [identifier, var] : vars) {
    return var;
  }
  throw FznException("Invalid initialization: expected exactly one variable");
}

Model::Model(Variable&& objective, ProblemType problemType)
    : _variables(varToMap(std::move(objective))),
      _constraints(),
      _solveType(problemType, varInMap(_variables)),
      _boolVarPars{BoolVar(false, ""), BoolVar(true, "")} {}

Model::Model(std::unordered_map<std::string_view, Variable>&& variables,
             std::vector<Constraint>&& constraints, SolveType&& solveType)
    : _variables(std::move(variables)),
      _constraints(std::move(constraints)),
      _solveType(std::move(solveType)),
      _boolVarPars{BoolVar(false, ""), BoolVar(true, "")} {}

Model::Model()
    : Model(std::vector<Variable>{}, std::vector<Constraint>{}, SolveType()) {}

Model::Model(std::vector<Variable>&& variables,
             std::vector<Constraint>&& constraints, SolveType&& solveType)
    : Model(varVectorToMap(std::move(variables)), std::move(constraints),
            std::move(solveType)) {}

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

SetVar& Model::addSetVarPar(const IntSet& is) {
  return _setVarPars.emplace_back(SetVar(is, ""));
}

const Variable& Model::addVariable(Variable&& var) {
  if (_variables.contains(var.identifier())) {
    throw FznException("Variable with identifier \"" +
                       std::string(var.identifier()) + "\" already exists");
  }
  _variables.emplace(var.identifier(), std::move(var));
  return _variables.at(var.identifier());
}

const Constraint& Model::addConstraint(Constraint&& constraint) {
  return _constraints.emplace_back(std::move(constraint));
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