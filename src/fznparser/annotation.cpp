#include "fznparser/annotation.hpp"

#include <array>
#include <functional>
#include <string>
#include <utility>

namespace fznparser {

using std::get;

Annotation::Annotation(std::string identifier)
    : _identifier(std::move(identifier)) {}

Annotation::Annotation(
    std::string identifier,
    std::vector<std::vector<AnnotationExpression>>&& expressions)
    : _identifier(std::move(identifier)),
      _expressions(std::move(expressions)) {}

const std::vector<AnnotationExpression>& Annotation::addAnnotationExpression(
    std::vector<AnnotationExpression>&& expression) {
  return _expressions.emplace_back(std::move(expression));
}

const std::vector<AnnotationExpression>& Annotation::addAnnotationExpression(
    AnnotationExpression&& expression) {
  return _expressions.emplace_back(
      std::vector<AnnotationExpression>{std::move(expression)});
}

bool Annotation::operator==(const Annotation& other) const {
  if (_identifier != other._identifier ||
      _expressions.size() != other._expressions.size()) {
    return false;
  }
  for (size_t i = 0; i < _expressions.size(); ++i) {
    if (_expressions.at(i).size() != other._expressions.at(i).size()) {
      return false;
    }
    for (size_t j = 0; j < _expressions.at(i).size(); j++) {
      if (_expressions.at(i).at(j).operator!=(other._expressions.at(i).at(j))) {
        return false;
      }
    }
  }
  return true;
}

bool Annotation::operator!=(const Annotation& other) const {
  return !operator==(other);
}

bool AnnotationExpression::operator==(const AnnotationExpression& other) const {
  if (holds_alternative<bool>(*this) && holds_alternative<bool>(other)) {
    return get<bool>(*this) == get<bool>(other);
  }
  if (holds_alternative<int64_t>(*this) && holds_alternative<int64_t>(other)) {
    return get<int64_t>(*this) == get<int64_t>(other);
  }
  if (holds_alternative<double>(*this) && holds_alternative<double>(other)) {
    return get<double>(*this) == get<double>(other);
  }
  if (holds_alternative<IntSet>(*this) && holds_alternative<IntSet>(other)) {
    return get<IntSet>(*this).operator==(get<IntSet>(other));
  }
  if (holds_alternative<FloatSet>(*this) &&
      holds_alternative<FloatSet>(other)) {
    return get<FloatSet>(*this).operator==(get<FloatSet>(other));
  }
  if (holds_alternative<std::string>(*this) &&
      holds_alternative<std::string>(other)) {
    return get<std::string>(*this) == get<std::string>(other);
  }
  if (holds_alternative<Annotation>(*this) &&
      holds_alternative<Annotation>(other)) {
    return get<Annotation>(*this).operator==(get<Annotation>(other));
  }
  return false;
}

bool AnnotationExpression::operator!=(const AnnotationExpression& other) const {
  return !operator==(other);
}

}  // namespace fznparser