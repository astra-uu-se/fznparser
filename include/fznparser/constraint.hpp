#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "variable.hpp"

namespace fznparser {
  class Constraint {
  private:
    std::string _name;

  public:
    static std::shared_ptr<Constraint> create(
        const std::string& name,
        const std::vector<std::shared_ptr<fznparser::Annotation>>& annotations,
        const std::vector<std::shared_ptr<Variable>>& arguments);

    explicit Constraint(std::string name) : _name(std::move(name)) {}
    virtual ~Constraint() = default;

    std::string_view name() {
      // How to one-line this such that the implicit conversion kicks in?
      std::string_view view = _name;
      return view;
    }

    virtual bool isFunctional() = 0;
  };

  /**
   * @brief A functional constraint is one which defines one or more output
   * variables in terms of one or more input variables.
   *
   * For example, plus(a, b, c) functionally defines c in terms of a and b.
   */
  class FunctionalConstraint : public Constraint {
  private:
    std::vector<std::shared_ptr<Variable>> _inputs;
    std::vector<std::shared_ptr<Variable>> _outputs;

  public:
    FunctionalConstraint(std::string name, std::vector<std::shared_ptr<Variable>> in,
                         std::vector<std::shared_ptr<Variable>> out)
        : Constraint(std::move(name)), _inputs(std::move(in)), _outputs(std::move(out)) {}
    ~FunctionalConstraint() override = default;

    bool isFunctional() override { return true; }

    const std::vector<std::shared_ptr<Variable>>& inputs() { return _inputs; }
    const std::vector<std::shared_ptr<Variable>>& outputs() { return _outputs; }
  };

  /**
   * @brief A non-functional constraint is one which constrains variables but
   * does not functionally define them.
   *
   * For example, less_than(a, b) does not define a or b in terms of the other
   * variable.
   */
  class NonFunctionalConstraint : public Constraint {
  private:
    std::vector<std::shared_ptr<Variable>> _variables;

  public:
    NonFunctionalConstraint(std::string name, std::vector<std::shared_ptr<Variable>> vars)
        : Constraint(std::move(name)), _variables(std::move(vars)) {}
    ~NonFunctionalConstraint() override = default;

    bool isFunctional() override { return false; }

    const std::vector<std::shared_ptr<Variable>>& variables() { return _variables; }
  };
}  // namespace fznparser