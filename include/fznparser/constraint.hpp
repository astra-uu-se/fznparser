#pragma once

#include <array>
#include <functional>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string>
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
  std::string _identifier;
  std::vector<Arg> _arguments;
  std::vector<Annotation> _annotations;
  std::optional<std::reference_wrapper<const Variable>> _definedVariable;

 public:
  Constraint(const Constraint&) = default;
  Constraint(Constraint&&) = default;
  Constraint(const std::string&, std::vector<Arg>&&,
             std::vector<Annotation>&& = {});

  const std::vector<Annotation>& annotations() const;
  const std::string& identifier() const;

  void addAnnotation(const Annotation&);
  void addAnnotation(const std::string&);
  void addAnnotation(const std::string&, AnnotationExpression&&);
  void addAnnotation(const std::string&, std::vector<AnnotationExpression>&&);

  void interpretAnnotations(const std::unordered_map<std::string, Variable>&);

  const std::vector<Arg>& arguments() const;
  std::optional<std::reference_wrapper<const Variable>> definedVariable() const;

  bool operator==(const Constraint&) const;
  bool operator!=(const Constraint&) const;
  std::string toString() const;
};

};  // namespace fznparser