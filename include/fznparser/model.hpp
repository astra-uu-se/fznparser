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
    std::vector<Constraint> _constraints;
    Objective _objective;

  public:
    FZNModel(std::vector<Parameter> parameters, std::vector<Variable> variables,
             std::vector<Constraint> constraints, Objective objective)
        : _parameters(std::move(parameters)),
          _variables(std::move(variables)),
          _constraints(std::move(constraints)),
          _objective(std::move(objective)) {}

    /**
     * Since the models can be quite large, the copy constructor
     * is deleted, but a move constructor is defined.
     */
    FZNModel(FZNModel& other) = delete;
    FZNModel(FZNModel&& other) = default;

    [[nodiscard]] const std::vector<Parameter>& parameters() const noexcept { return _parameters; }
    [[nodiscard]] const std::vector<Variable>& variables() const noexcept { return _variables; }
    [[nodiscard]] const std::vector<Constraint>& constraints() const noexcept {
      return _constraints;
    }
    [[nodiscard]] const Objective& objective() const noexcept { return _objective; }
  };

}  // namespace fznparser