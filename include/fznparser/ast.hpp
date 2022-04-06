#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace fznparser {

  using Int = int64_t;
  using Identifier = std::string;

  struct IntervalSet {
    const Int lowerBound;
    const Int upperBound;
  };

  using Set = std::variant<IntervalSet, std::vector<Int>>;
  using Value = std::variant<Int, bool, Set>;

  using ArrayElement = std::variant<Identifier, Value>;
  using Array = std::vector<ArrayElement>;

  struct Parameter {
    const Identifier name;
    const std::variant<Value, std::vector<Value>> value;
  };

  struct OutputVariableAnnotation {};

  struct OutputArrayAnnotation {
    /**
     * The sizes of the different dimensions of the variable array this annotation is attached to.
     * All array index sets in FlatZinc are in the form of 1..x, so the values in this vector are
     * the 'x' for the different dimensions.
     */
    const std::vector<Int> sizes;
  };

  struct DefinesVariableAnnotation {
    const Identifier definedVariable;
  };

  using Annotation
      = std::variant<DefinesVariableAnnotation, OutputArrayAnnotation, OutputVariableAnnotation>;

  struct SearchVariable {
    enum class Type { INT, BOOL };

    const Identifier name;
    const Type type;
    const std::optional<ArrayElement> value;
    const std::vector<Annotation> annotations;
  };

  struct VariableArray {
    const Identifier name;
    const SearchVariable::Type type;
    const std::vector<ArrayElement> contents;
    const std::vector<Annotation> annotations;

    [[nodiscard]] inline auto size() const noexcept { return contents.size(); }
    [[nodiscard]] inline auto begin() const noexcept { return contents.begin(); }
    [[nodiscard]] inline auto end() const noexcept { return contents.end(); }
    [[nodiscard]] const ArrayElement& operator[](size_t index) const { return contents[index]; }
  };

  struct Constraint {
    using Argument = std::variant<Value, Identifier, Array>;

    const Identifier name;
    const std::vector<Argument> arguments;
    const std::vector<Annotation> annotations;
  };

  struct Satisfy {};

  struct Minimise {
    const Identifier variable;
  };

  struct Maximise {
    const Identifier variable;
  };

  using Objective = std::variant<Satisfy, Minimise, Maximise>;
}  // namespace fznparser
