#include "fznparser/model.hpp"

namespace fznparser {

std::unordered_map<std::string, Var> varToMap(Var&& var) {
  std::unordered_map<std::string, Var> varMap;
  varMap.emplace(var.identifier(), std::move(var));
  return varMap;
}

std::unordered_map<std::string, Var> varVectorToMap(std::vector<Var>&& vars) {
  std::unordered_map<std::string, Var> varMap;
  for (Var& var : vars) {
    varMap.emplace(var.identifier(), std::move(var));
  }
  return varMap;
}

const Var& varInMap(const std::unordered_map<std::string, Var>& vars) {
  if (vars.size() != 1) {
    throw FznException("Invalid initialization: expected exactly one variable");
  }
  for (const auto& [identifier, var] : vars) {
    return var;
  }
  throw FznException("Invalid initialization: expected exactly one variable");
}

Model::Model(Var&& objective, ProblemType problemType)
    : _vars(varToMap(std::move(objective))),
      _constraints(),
      _solveType(problemType, varInMap(_vars)),
      _boolVarPars{BoolVar(false, ""), BoolVar(true, "")} {}

Model::Model(std::unordered_map<std::string, Var>&& vars,
             std::vector<Constraint>&& constraints, SolveType&& solveType)
    : _vars(std::move(vars)),
      _constraints(std::move(constraints)),
      _solveType(std::move(solveType)),
      _boolVarPars{BoolVar(false, ""), BoolVar(true, "")} {}

Model::Model()
    : Model(std::vector<Var>{}, std::vector<Constraint>{}, SolveType()) {}

Model::Model(std::vector<Var>&& vars, std::vector<Constraint>&& constraints,
             SolveType&& solveType)
    : Model(varVectorToMap(std::move(vars)), std::move(constraints),
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

const Var& Model::addVar(Var&& var) {
  if (_vars.contains(var.identifier())) {
    throw FznException("Variable with identifier \"" + var.identifier() +
                       "\" already exists");
  }
  Var& addedVar = _vars.emplace(var.identifier(), std::move(var)).first->second;
  addedVar.interpretAnnotations(_vars);
  return addedVar;
}

const Constraint& Model::addConstraint(Constraint&& constraint) {
  Constraint& con = _constraints.emplace_back(std::move(constraint));
  con.interpretAnnotations(_vars);
  return con;
}

size_t Model::numVars() const { return _vars.size(); }
size_t Model::numConstraints() const { return _constraints.size(); }
const Var& Model::var(std::string identifier) const {
  return _vars.at(identifier);
}

bool Model::hasVar(std::string identifier) const {
  return _vars.find(identifier) != _vars.end();
}

const std::unordered_map<std::string, Var>& Model::vars() const {
  return _vars;
}

const std::vector<Constraint>& Model::constraints() const {
  return _constraints;
}

const SolveType& Model::solveType() const { return _solveType; }

bool Model::hasObjective() const noexcept { return _solveType.hasObjective(); }
const Var& Model::objective() const { return _solveType.objective(); }

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
  if (_vars.size() != other._vars.size() ||
      _constraints.size() != other._constraints.size() ||
      _solveType != other._solveType) {
    return false;
  }
  for (const auto& [identifier, var] : _vars) {
    if (other._vars.find(identifier) == other._vars.end() ||
        var.operator!=(other._vars.at(identifier))) {
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
  for (const auto& [identifier, var] : _vars) {
    s += var.toString() + ";\n";
  }
  for (const Constraint& con : _constraints) {
    s += con.toString() + ";\n";
  }
  s += _solveType.toString() + ";\n";
  return s;
}
}  // namespace fznparser