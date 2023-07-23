#pragma once

#include <array>
#include <functional>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "fznparser/annotation.hpp"
#include "fznparser/arguments.hpp"
#include "fznparser/except.hpp"

namespace fznparser {

class Model;  // forward declaration

class Constraint {
  std::string_view _identifier;
  std::vector<Arg> _arguments;
  std::vector<Annotation> _annotations;

 public:
  Constraint(const Constraint&) = default;
  Constraint(Constraint&&) = default;
  Constraint(const std::string_view&&, std::vector<Arg>,
             std::vector<Annotation> = {});

  const std::vector<Annotation>& annotations() const;
  const std::string_view& identifier() const;

  void addAnnotation(const Annotation&);
  void addAnnotation(const std::string_view&);
  void addAnnotation(const std::string_view&, AnnotationExpression&&);
  void addAnnotation(const std::string_view&,
                     std::vector<AnnotationExpression>&&);

  const std::vector<Arg>& arguments() const;
  std::optional<std::reference_wrapper<const Variable>> definedVariable(
      const Model& model) const;

  bool operator==(const Constraint&) const;
  bool operator!=(const Constraint&) const;
  std::string toString() const;
};

};  // namespace fznparser