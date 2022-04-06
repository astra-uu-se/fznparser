#pragma once

#include <utility>

#include "fznparser/ast.hpp"

namespace fznparser {

  /**
   * The container for the FlatZinc model.
   */
  class FZNModel {
  private:
    std::vector<Parameter> _parameters;
    std::vector<Variable> _variables;
    Objective _objective;

  public:
    FZNModel(std::vector<Parameter> parameters, std::vector<Variable> variables,
             Objective objective)
        : _parameters(std::move(parameters)),
          _variables(std::move(variables)),
          _objective(std::move(objective)) {}

    /**
     * Since the models can be quite large, the copy constructor
     * is deleted, but a move constructor is defined.
     */
    FZNModel(FZNModel& other) = delete;
    FZNModel(FZNModel&& other) = default;

    [[nodiscard]] const std::vector<Parameter>& parameters() const noexcept { return _parameters; }
    [[nodiscard]] const std::vector<Variable>& variables() const noexcept { return _variables; }
    [[nodiscard]] const Objective& objective() const noexcept { return _objective; }
  };

}  // namespace fznparser