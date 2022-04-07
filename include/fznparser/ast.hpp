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

  template <typename T> using Set
      = std::conditional_t<std::is_same_v<T, Int>, std::variant<IntRange, std::vector<Int>>,
                           std::vector<T>>;

  template <typename T> using BasicExpr = std::variant<Identifier, T>;
  template <typename T> using Array = std::vector<BasicExpr<T>>;

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

  template <typename Type> struct BaseParameter {
    const Identifier name;
    const Type value;

    bool operator==(const BaseParameter<Type>& other) const noexcept {
      return name == other.name && value == other.value;
    }

    bool operator!=(const BaseParameter<Type>& other) const noexcept { return !(*this == other); }
  };

  using IntParameter = BaseParameter<Int>;
  using SetOfIntParameter = BaseParameter<Set<Int>>;
  using ArrayOfIntParameter = BaseParameter<std::vector<Int>>;
  using ArrayOfSetOfIntParameter = BaseParameter<std::vector<Set<Int>>>;

  using BoolParameter = BaseParameter<bool>;
  using ArrayOfBoolParameter = BaseParameter<std::vector<bool>>;

  using Parameter = std::variant<IntParameter, SetOfIntParameter, ArrayOfIntParameter,
                                 ArrayOfSetOfIntParameter, BoolParameter, ArrayOfBoolParameter>;

  template <typename Type> using Domain = std::conditional_t<
      std::is_same_v<Type, bool>, BasicDomain<bool>,
      std::conditional_t<std::is_same_v<Type, Int>,
                         std::variant<BasicDomain<Int>, IntRange, std::vector<Int>>,
                         std::false_type>>;

  enum class ValueType { INT, BOOL };

  template <typename Type> struct SearchVariable {
    const Identifier name;
    const Domain<Type> domain;
    const std::vector<Annotation> annotations;
    const std::optional<BasicExpr<Type>> value;

    bool operator==(const SearchVariable<Type>& other) const noexcept {
      return name == other.name && domain == other.domain && value == other.value
             && annotations == other.annotations;
    }

    bool operator!=(const SearchVariable<Type>& other) const noexcept { return !(*this == other); }

    [[nodiscard]] ValueType type() const noexcept {
      if constexpr (std::is_same_v<Type, Int>) {
        return ValueType::INT;
      } else if constexpr (std::is_same_v<Type, bool>) {
        return ValueType::BOOL;
      } else {
        // static_assert(false,...) would be ill-formed.
        static_assert(!sizeof(Type), "Only types 'Int' or 'bool' are supported.");
      }
    }
  };

  template <typename Type> struct VariableArray {
    const Identifier name;
    const Array<Type> contents;
    const std::vector<Annotation> annotations;

    [[nodiscard]] inline auto size() const noexcept { return contents.size(); }
    [[nodiscard]] inline auto begin() const noexcept { return contents.begin(); }
    [[nodiscard]] inline auto end() const noexcept { return contents.end(); }
    [[nodiscard]] const BasicExpr<Type>& operator[](size_t index) const { return contents[index]; }

    bool operator==(const VariableArray<Type>& other) const noexcept {
      return name == other.name && contents == other.contents && annotations == other.annotations;
    }

    bool operator!=(const VariableArray<Type>& other) const noexcept { return !(*this == other); }

    [[nodiscard]] ValueType type() const noexcept {
      if constexpr (std::is_same_v<Type, Int>) {
        return ValueType::INT;
      } else if constexpr (std::is_same_v<Type, bool>) {
        return ValueType::BOOL;
      } else {
        // static_assert(false,...) would be ill-formed.
        static_assert(!sizeof(Type), "Only types 'Int' or 'bool' are supported.");
      }
    }
  };

  using IntVariable = SearchVariable<Int>;
  using BoolVariable = SearchVariable<bool>;
  using IntVariableArray = VariableArray<Int>;
  using BoolVariableArray = VariableArray<bool>;
  using Variable = std::variant<IntVariable, BoolVariable, IntVariableArray, BoolVariableArray>;

  using UnknownVariableType = std::variant<bool, Int, Set<Int>>;

  struct Constraint {
    using Argument = std::variant<bool, Int, Array<UnknownVariableType>, Set<Int>, Identifier>;

    const Identifier name;
    const std::vector<Argument> arguments;
    const std::vector<Annotation> annotations;

    bool operator==(const Constraint& other) const noexcept {
      return name == other.name && arguments == other.arguments && annotations == other.annotations;
    }

    bool operator!=(const Constraint& other) const noexcept { return !(*this == other); }
  };

  struct Satisfy {
    constexpr bool operator==(const Satisfy&) const noexcept { return true; }
    constexpr bool operator!=(const Satisfy&) const noexcept { return false; }
  };

  struct Minimise {
    const Identifier variable;

    bool operator==(const Minimise& other) const noexcept { return variable == other.variable; }
    bool operator!=(const Minimise& other) const noexcept { return !(*this == other); }
  };

  struct Maximise {
    const Identifier variable;

    bool operator==(const Maximise& other) const noexcept { return variable == other.variable; }
    bool operator!=(const Maximise& other) const noexcept { return !(*this == other); }
  };

  using Objective = std::variant<Satisfy, Minimise, Maximise>;
}  // namespace fznparser
