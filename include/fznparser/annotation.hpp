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

#include "fznparser/types.hpp"

namespace fznparser {

class AnnotationExpression;

class Annotation {
  std::string _identifier;
  std::vector<std::vector<AnnotationExpression>> _expressions;

 public:
  Annotation(const Annotation&) = default;
  Annotation(Annotation&&) = default;

  Annotation(const std::string&);
  Annotation(const std::string&,
             std::vector<std::vector<AnnotationExpression>>&&);
  const std::string& identifier() const { return _identifier; }
  const std::vector<std::vector<AnnotationExpression>>& expressions() const {
    return _expressions;
  }

  const std::vector<AnnotationExpression>& addAnnotationExpression(
      std::vector<AnnotationExpression>&&);
  const std::vector<AnnotationExpression>& addAnnotationExpression(
      AnnotationExpression&&);

  bool operator==(const Annotation&) const;
  bool operator!=(const Annotation&) const;
  std::string toString() const;
};

class AnnotationExpression
    : public std::variant<bool, int64_t, double, IntSet, FloatSet, std::string,
                          Annotation> {
 public:
  using std::variant<bool, int64_t, double, IntSet, FloatSet, std::string,
                     Annotation>::variant;
  bool operator==(const AnnotationExpression&) const;
  bool operator!=(const AnnotationExpression&) const;
  std::string toString() const;
};

}  // namespace fznparser