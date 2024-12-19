#include "fznparser/constraint.hpp"

#include <string>
#include <utility>
#include <variant>

#include "fznparser/annotation.hpp"
#include "fznparser/arguments.hpp"
#include "fznparser/except.hpp"
#include "fznparser/model.hpp"

namespace fznparser {
Constraint::Constraint(std::string identifier, std::vector<Arg>&& arguments,
                       std::vector<Annotation>&& annotations)
    : _identifier(std::move(identifier)),
      _arguments(std::move(arguments)),
      _annotations(std::move(annotations)),
      _definedVar(std::nullopt) {}

const std::string& Constraint::identifier() const { return _identifier; }

void Constraint::addAnnotation(const std::string& identifier) {
  _annotations.emplace_back(identifier);
}

void Constraint::addAnnotation(const Annotation& annotation) {
  _annotations.push_back(annotation);
}

void Constraint::addAnnotation(const std::string& identifier,
                               std::vector<AnnotationExpression>&& expression) {
  _annotations.emplace_back(
      identifier,
      std::vector<std::vector<AnnotationExpression>>{std::move(expression)});
}

void Constraint::addAnnotation(const std::string& identifier,
                               AnnotationExpression&& expression) {
  _annotations.emplace_back(identifier,
                            std::vector<std::vector<AnnotationExpression>>{
                                std::vector<AnnotationExpression>{expression}});
}

const std::vector<Annotation>& Constraint::annotations() const {
  return _annotations;
}

void Constraint::interpretAnnotations(
    const std::unordered_map<std::string, Var>& varMapping) {
  for (const Annotation& annotation : _annotations) {
    if (annotation.identifier() == "defines_var") {
      if (annotation.expressions().size() != 1 ||
          annotation.expressions().front().size() != 1) {
        throw FznException(
            "defines_var annotation must define exactly one "
            "variable");
      }
      const auto& expression = annotation.expressions().front().front();
      // we can't really tell the difference between identifiers and annotations

      if (!std::holds_alternative<Annotation>(expression)) {
        throw FznException(
            "defines_var annotation argument must be an identifier");
      }
      const std::string& varIdentifier =
          std::get<Annotation>(expression).identifier();

      if (!varMapping.contains(varIdentifier)) {
        throw FznException("Var with identifier " + varIdentifier +
                           " is not defined");
      }
      _definedVar = varMapping.at(varIdentifier);
    }
  }
}

const std::vector<Arg>& Constraint::arguments() const { return _arguments; }

std::optional<const Var> Constraint::definedVar() const { return _definedVar; }

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

}  // namespace fznparser