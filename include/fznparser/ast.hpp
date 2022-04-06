#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace fznparser {

  using Int = int64_t;
  using Identifier = std::string;

  struct IntRange {
    const Int lowerBound;
    const Int upperBound;

    bool operator==(const IntRange& other) const noexcept {
      return lowerBound == other.lowerBound && upperBound == other.upperBound;
    }

    bool operator!=(const IntRange& other) const noexcept { return !(*this == other); }
  };

  using Set = std::variant<IntRange, std::vector<Int>>;
  using Value = std::variant<Int, bool, Set>;

  using BasicExpr = std::variant<Identifier, Value>;
  using Array = std::vector<BasicExpr>;

  struct Parameter {
    using ParamValue = std::variant<Value, std::vector<Value>>;

    const Identifier name;
    const ParamValue value;

    bool operator==(const Parameter& other) const noexcept {
      return name == other.name && value == other.value;
    }

    bool operator!=(const Parameter& other) const noexcept { return !(*this == other); }
  };

  struct TagAnnotation {
    const std::string tag;

    bool operator==(const TagAnnotation& other) const noexcept { return tag == other.tag; }
    bool operator!=(const TagAnnotation& other) const noexcept { return !(*this == other); }
  };

  struct OutputArrayAnnotation {
    /**
     * The sizes of the different dimensions of the variable array this annotation is attached to.
     * All array index sets in FlatZinc are in the form of 1..x, so the values in this vector are
     * the 'x' for the different dimensions.
     */
    const std::vector<Int> sizes;

    bool operator==(const OutputArrayAnnotation& other) const noexcept {
      return sizes == other.sizes;
    }

    bool operator!=(const OutputArrayAnnotation& other) const noexcept { return !(*this == other); }
  };

  struct DefinesVariableAnnotation {
    const Identifier definedVariable;

    bool operator==(const DefinesVariableAnnotation& other) const noexcept {
      return definedVariable == other.definedVariable;
    }

    bool operator!=(const DefinesVariableAnnotation& other) const noexcept {
      return !(*this == other);
    }
  };

  using Annotation = std::variant<DefinesVariableAnnotation, OutputArrayAnnotation, TagAnnotation>;

  template <typename Element> struct BasicDomain {
    bool operator==(const BasicDomain<Element>&) const noexcept { return true; }
    bool operator!=(const BasicDomain<Element>& other) const noexcept { return !(*this == other); }
  };

  using Domain = std::variant<BasicDomain<bool>, BasicDomain<Int>, IntRange, std::vector<Int>>;

  struct SearchVariable {
    const Identifier name;
    const Domain domain;
    const std::vector<Annotation> annotations;
    const std::optional<BasicExpr> value;

    bool operator==(const SearchVariable& other) const noexcept {
      return name == other.name && domain == other.domain && value == other.value
             && annotations == other.annotations;
    }

    bool operator!=(const SearchVariable& other) const noexcept { return !(*this == other); }
  };

  struct VariableArray {
    const Identifier name;
    const std::vector<BasicExpr> contents;
    const std::vector<Annotation> annotations;

    [[nodiscard]] inline auto size() const noexcept { return contents.size(); }
    [[nodiscard]] inline auto begin() const noexcept { return contents.begin(); }
    [[nodiscard]] inline auto end() const noexcept { return contents.end(); }
    [[nodiscard]] const BasicExpr& operator[](size_t index) const { return contents[index]; }

    bool operator==(const VariableArray& other) const noexcept {
      return name == other.name && contents == other.contents && annotations == other.annotations;
    }

    bool operator!=(const VariableArray& other) const noexcept { return !(*this == other); }
  };

  using Variable = std::variant<VariableArray, SearchVariable>;

  struct Constraint {
    using Argument = std::variant<Value, Identifier, Array>;

    const Identifier name;
    const std::vector<Argument> arguments;
    const std::vector<Annotation> annotations;

    bool operator==(const Constraint& other) const noexcept {
      return name == other.name && arguments == other.arguments && annotations == other.annotations;
    }

    bool operator!=(const Constraint& other) const noexcept { return !(*this == other); }
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
