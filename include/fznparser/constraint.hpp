#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

#include "variable.hpp"

namespace fznparser {
  typedef std::variant<std::vector<std::shared_ptr<Literal>>, std::shared_ptr<Literal>>
      ConstraintArgument;

  class Constraint {
  private:
    std::string _name;

  public:
    static std::shared_ptr<Constraint> create(
        const std::string& name,
        const std::vector<std::shared_ptr<fznparser::Annotation>>& annotations,
        const std::vector<ConstraintArgument>& arguments);

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
    std::vector<ConstraintArgument> _inputs;
    std::vector<ConstraintArgument> _outputs;

  public:
    FunctionalConstraint(std::string name, std::vector<ConstraintArgument> in,
                         std::vector<ConstraintArgument> out)
        : Constraint(std::move(name)), _inputs(std::move(in)), _outputs(std::move(out)) {}
    ~FunctionalConstraint() override = default;

    bool isFunctional() override { return true; }

    const std::vector<ConstraintArgument>& inputs() { return _inputs; }
    const std::vector<ConstraintArgument>& outputs() { return _outputs; }
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
    std::vector<ConstraintArgument> _arguments;

  public:
    NonFunctionalConstraint(std::string name, std::vector<ConstraintArgument> args)
        : Constraint(std::move(name)), _arguments(std::move(args)) {}
    ~NonFunctionalConstraint() override = default;

    bool isFunctional() override { return false; }

    const std::vector<ConstraintArgument>& arguments() { return _arguments; }
  };
}  // namespace fznparser