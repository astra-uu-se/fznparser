#include "fznparser/constraint.hpp"

namespace fznparser {
Constraint::Constraint(const std::string_view&& identifier,
                       std::vector<Arg> arguments,
                       std::vector<Annotation> annotations)
    : _identifier(identifier),
      _arguments(std::move(arguments)),
      _annotations(std::move(annotations)) {}

const std::string_view& Constraint::identifier() const { return _identifier; }

void Constraint::addAnnotation(const std::string_view& identifier) {
  _annotations.push_back(Annotation(identifier));
}

void Constraint::addAnnotation(const Annotation& annotation) {
  _annotations.push_back(annotation);
}

void Constraint::addAnnotation(const std::string_view& identifier,
                               std::vector<AnnotationExpression>&& expression) {
  _annotations.push_back(Annotation(
      identifier, std::move(std::vector<std::vector<AnnotationExpression>>{
                      std::move(expression)})));
}

void Constraint::addAnnotation(const std::string_view& identifier,
                               AnnotationExpression&& expression) {
  _annotations.push_back(Annotation(
      identifier,
      std::move(std::vector<std::vector<AnnotationExpression>>{
          std::move(std::vector<AnnotationExpression>{expression})})));
}

const std::vector<Arg>& Constraint::arguments() const { return _arguments; }

std::optional<std::reference_wrapper<const Variable>>
Constraint::definedVariable(const fznparser::Model& model) const {
  for (const auto& annotation : annotations()) {
    if (annotation.identifier() != "defines_var") {
      continue;
    }
    if (annotation.expressions().size() != 1 ||
        annotation.expressions().front().size() != 1) {
      throw FznException(
          "defines_var annotation must define exactly one "
          "variable");
    }
    const auto& expression = annotation.expressions().front().front();
    if (!std::holds_alternative<std::string_view>(expression)) {
      throw FznException(
          "defines_var annotation argument must be an identifier");
    }
    const std::string_view& identifier = get<std::string_view>(expression);
    if (!model.hasVariable(identifier)) {
      throw FznException("Variable " + std::string(identifier) +
                         " is not defined");
    }
    return std::reference_wrapper(model.variable(identifier));
  }
  return std::nullopt;
}

bool Constraint::operator==(const Constraint& other) const {
  if (_identifier != other._identifier ||
      _arguments.size() != other._arguments.size() ||
      _annotations.size() != other._annotations.size()) {
    return false;
  }
  for (size_t i = 0; i < _arguments.size(); ++i) {
    if (_arguments[i].operator!=(other._arguments[i])) {
      return false;
    }
  }
  for (size_t i = 0; i < _annotations.size(); ++i) {
    if (_annotations[i].operator!=(other._annotations[i])) {
      return false;
    }
  }
  return true;
}

bool Constraint::operator!=(const Constraint& other) const {
  return !operator==(other);
}

std::string Constraint::toString() const {
  std::string s = std::string(_identifier) + "(";
  for (size_t i = 0; i < _arguments.size(); ++i) {
    if (i != 0) {
      s += ", ";
    }
    s += _arguments[i].toString();
  }
  s += ")";
  if (!_annotations.empty()) {
    for (const auto& annotation : _annotations) {
      s += " :: " + annotation.toString();
    }
  }
  return s;
}

}  // namespace fznparser