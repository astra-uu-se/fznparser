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
    std::vector<ConstraintArgument> _args;
    std::vector<std::shared_ptr<Annotation>> _annotations;

  public:
    explicit Constraint(std::string name, std::vector<ConstraintArgument> args,
                        std::vector<std::shared_ptr<Annotation>> annotations)
        : _name(std::move(name)), _args(std::move(args)), _annotations(std::move(annotations)) {}

    virtual ~Constraint() = default;

    std::string_view name() {
      // How to one-line this such that the implicit conversion kicks in?
      std::string_view view = _name;
      return view;
    }

    [[nodiscard]] const std::vector<ConstraintArgument>& arguments() { return _args; }
    [[nodiscard]] const std::vector<std::shared_ptr<Annotation>>& annotations() {
      return _annotations;
    }
  };
}  // namespace fznparser