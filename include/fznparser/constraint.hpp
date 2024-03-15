#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>

#include "fznparser/annotation.hpp"
#include "fznparser/arguments.hpp"
#include "fznparser/variables.hpp"

namespace fznparser {

class Model;  // forward declaration

class Constraint {
  std::string _identifier;
  std::vector<Arg> _arguments;
  std::vector<Annotation> _annotations;
  std::optional<Var> _definedVar;

 public:
  Constraint(const Constraint&) = default;
  Constraint(Constraint&&) = default;
  Constraint(std::string, std::vector<Arg>&&, std::vector<Annotation>&& = {});

  [[nodiscard]] const std::vector<Annotation>& annotations() const;
  [[nodiscard]] const std::string& identifier() const;

  void addAnnotation(const Annotation&);
  void addAnnotation(const std::string&);
  void addAnnotation(const std::string&, AnnotationExpression&&);
  void addAnnotation(const std::string&, std::vector<AnnotationExpression>&&);

  void interpretAnnotations(const std::unordered_map<std::string, Var>&);

  [[nodiscard]] const std::vector<Arg>& arguments() const;
  [[nodiscard]] std::optional<const Var> definedVar() const;

  bool operator==(const Constraint&) const;
  bool operator!=(const Constraint&) const;
  [[nodiscard]] std::string toString() const;
};

}  // namespace fznparser