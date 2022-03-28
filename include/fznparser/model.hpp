#pragma once

#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "constraint.hpp"
#include "variable.hpp"

namespace fznparser {
  enum Objective { SATISFY, MINIMIZE, MAXIMIZE };

  class Model {
  private:
    std::vector<Parameter> _parameters;
    std::vector<std::shared_ptr<Variable>> _variables;
    std::vector<std::shared_ptr<Constraint>> _constraints;
    Objective _objective;
    std::optional<std::shared_ptr<Variable>> _objectiveValue;

  public:
    /**
     * Create a model for a satisfaction problem.
     *
     * @param params The parameters of the model.
     * @param vars The variables of the model.
     * @param cons The constraints of the model.
     */
    Model(std::vector<Parameter> params, std::vector<std::shared_ptr<Variable>> vars,
          std::vector<std::shared_ptr<Constraint>> cons)
        : Model(std::move(params), std::move(vars), std::move(cons), Objective::SATISFY, {}) {}

    /**
     * Create a model for an optimization problem.
     *
     * @param params The parameters of the model.
     * @param vars The variables of the model.
     * @param cons The constraints of the model.
     * @param objective The objective (minimization or maximization).
     * @param objectiveValue The variable to optimize. This must be an element of \p vars as well.
     */
    Model(std::vector<Parameter> params, std::vector<std::shared_ptr<Variable>> vars,
          std::vector<std::shared_ptr<Constraint>> cons, Objective objective,
          std::optional<std::shared_ptr<Variable>> objectiveValue)
        : _parameters(std::move(params)),
          _variables(std::move(vars)),
          _constraints(std::move(cons)),
          _objective(objective),
          _objectiveValue(std::move(objectiveValue)) {}

    [[nodiscard]] const std::vector<std::shared_ptr<Constraint>>& constraints() const {
      return _constraints;
    }

    [[nodiscard]] const std::vector<std::shared_ptr<Variable>>& variables() const {
      return _variables;
    }

    [[nodiscard]] const std::vector<Parameter>& parameters() const { return _parameters; }

    [[nodiscard]] Objective objective() const { return _objective; }
    [[nodiscard]] std::optional<std::shared_ptr<Variable>> objective_value() const {
      return _objectiveValue;
    }
  };
}  // namespace fznparser