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