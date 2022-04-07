#include "fznparser/model.hpp"

using namespace fznparser;

FZNModel::FZNModel(std::vector<Parameter> parameters, std::vector<Variable> variables,
                   std::vector<Constraint> constraints, Objective objective)
    : _parameters(std::move(parameters)),
      _variables(std::move(variables)),
      _constraints(std::move(constraints)),
      _objective(std::move(objective)) {
  for (size_t i = 0; i < _parameters.size(); ++i) {
    std::visit([&](const auto& parameter) { _parameterIndices.emplace(parameter.name, i); },
               _parameters[i]);
  }

  for (size_t i = 0; i < _variables.size(); ++i) {
    std::visit([&](const auto& variable) { _variableIndices.emplace(variable.name, i); },
               _variables[i]);
  }
}

std::optional<FZNModel::Identifiable> FZNModel::identify(
    const std::string_view& id) const noexcept {
  if (auto paramIt = _parameterIndices.find(id); paramIt != _parameterIndices.end()) {
    return _parameters[paramIt->second];
  }

  if (auto varsIt = _variableIndices.find(id); varsIt != _variableIndices.end()) {
    return _variables[varsIt->second];
  }

  return {};
}
