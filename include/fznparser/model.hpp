#pragma once

#include <memory>
#include <vector>

#include "constraint.hpp"
#include "variable.hpp"

namespace fznparser {
  class Model {
  private:
    std::vector<std::shared_ptr<Variable>> _variables;
    std::vector<std::shared_ptr<Constraint>> _constraints;

  public:
    Model(std::vector<std::shared_ptr<Variable>> vars,
          std::vector<std::shared_ptr<Constraint>> cons)
        : _variables(vars), _constraints(cons) {}

    const std::vector<std::shared_ptr<Constraint>>& constraints() { return _constraints; }
    const std::vector<std::shared_ptr<Variable>>& variables() { return _variables; }
  };
}  // namespace fznparser