#include "fznparser/model.hpp"

using namespace fznparser;

FZNModel::FZNModel(std::vector<Parameter> parameters, std::vector<Variable> variables,
                   std::vector<Constraint> constraints, Objective objective)
    : _parameters(std::move(parameters)),
      _variables(std::move(variables)),
      _constraints(std::move(constraints)),
      _objective(std::move(objective)) {
  for (size_t i = 0; i < _parameters.size(); ++i) {
    _parameterIndices.emplace(_parameters[i].name, i);
  }

  for (size_t i = 0; i < _variables.size(); ++i) {
    std::visit([&](const auto& variable) { _variableIndices.emplace(variable.name, i); },
               _variables[i]);
  }
}

static FZNModel::Identifiable variableToIdentifiable(const Variable& variable) {
  if (std::holds_alternative<SearchVariable>(variable)) {
    return std::get<SearchVariable>(variable);
  } else if (std::holds_alternative<VariableArray>(variable)) {
    return std::get<VariableArray>(variable);
  }

  // Should never happen. See the comment in FZNModel::identify().
  throw std::runtime_error("Unhandled variable case when converting to identifiable.");
}

std::optional<FZNModel::Identifiable> FZNModel::identify(
    const std::string_view& id) const noexcept {
  if (auto paramIt = _parameterIndices.find(id); paramIt != _parameterIndices.end()) {
    return _parameters[paramIt->second];
  }

  if (auto varsIt = _variableIndices.find(id); varsIt != _variableIndices.end()) {
    // This is a hack. Since Variable is a variant and Identifiable is a variant of a superset of
    // the types covered by Variable, every Variable can be an Identifiable. However, we have to
    // unwrap the Variable variant to get the underlying types out.
    // In C++20, std::visit can return a value, so then this can be replaced with:
    // return std::visit([](const auto& variable) { return variable; }, _variables[varsIt->second]);
    return variableToIdentifiable(_variables[varsIt->second]);
  }

  return {};
}
