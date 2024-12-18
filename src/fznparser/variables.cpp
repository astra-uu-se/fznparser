#include "fznparser/variables.hpp"

#include <functional>
#include <limits>
#include <stdexcept>
#include <utility>

#include "fznparser/except.hpp"
#include "fznparser/model.hpp"

namespace fznparser {

using std::get;
using std::shared_ptr;

VarBase::VarBase(std::string identifier, std::vector<Annotation>&& annotations)
    : _identifier(std::move(identifier)),
      _annotations(std::move(annotations)) {}

const std::string& VarBase::identifier() const { return _identifier; }

const std::vector<Annotation>& VarBase::annotations() const {
  return _annotations;
}

void VarBase::interpretAnnotations(
    const std::unordered_map<std::string, Var>&) {
  for (const Annotation& annotation : _annotations) {
    if (annotation.identifier() == "output_var") {
      _isOutput = true;
    } else if (annotation.identifier() == "is_defined_var") {
      _isDefinedVar = true;
    }
  }
}

void VarBase::addAnnotation(const std::string& identifier) {
  _annotations.emplace_back(identifier);
}

void VarBase::addAnnotation(const Annotation& annotation) {
  _annotations.push_back(annotation);
}

void VarBase::addAnnotation(const std::string& identifier,
                            std::vector<AnnotationExpression>&& expression) {
  _annotations.emplace_back(
      identifier,
      std::vector<std::vector<AnnotationExpression>>{std::move(expression)});
}

void VarBase::addAnnotation(const std::string& identifier,
                            AnnotationExpression&& expression) {
  _annotations.emplace_back(identifier,
                            std::vector<std::vector<AnnotationExpression>>{
                                std::vector<AnnotationExpression>{expression}});
}

bool VarBase::isOutput() const { return _isOutput; }
bool VarBase::isDefinedVar() const { return _isDefinedVar; }

BoolVar::BoolVar(const std::string& identifier,
                 std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(2) {}

BoolVar::BoolVar(bool b, const std::string& identifier,
                 std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)),
      _domain(static_cast<signed char>(b)) {}

bool BoolVar::contains(const bool& val) const {
  return _domain == 2 || (val && _domain == 1) || (!val && _domain == 0);
}
bool BoolVar::lowerBound() const { return _domain == 1; }
bool BoolVar::upperBound() const { return _domain != 0; }

bool BoolVar::operator==(const BoolVar& other) const {
  if (identifier() != other.identifier() || _domain != other._domain ||
      annotations().size() != other.annotations().size()) {
    return false;
  }
  for (size_t i = 0; i < annotations().size(); ++i) {
    if (annotations().at(i).operator!=(other.annotations().at(i))) {
      return false;
    }
  }
  return true;
}

bool BoolVar::operator!=(const BoolVar& other) const {
  return !operator==(other);
}

bool BoolVar::isFixed() const { return _domain != 2; }

std::string BoolVar::toString() const {
  std::string s = "var bool: " + identifier();
  if (_domain == 1) {
    s += " = true";
  } else if (_domain == 0) {
    s += " = false";
  }
  if (!annotations().empty()) {
    for (const auto& annotation : annotations()) {
      s += " :: " + annotation.toString();
    }
  }
  return s;
}

IntVar::IntVar(const std::string& identifier,
               std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)),
      _domain(IntSet(std::numeric_limits<int64_t>::min(),
                     std::numeric_limits<int64_t>::max())) {}

IntVar::IntVar(int64_t val, const std::string& identifier,
               std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(val) {}

IntVar::IntVar(int64_t lb, int64_t ub, const std::string& identifier,
               std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(lb, ub) {}

IntVar::IntVar(std::vector<int64_t>&& vals, const std::string& identifier,
               std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(std::move(vals)) {}

const IntSet& IntVar::domain() const { return _domain; }

bool IntVar::contains(const int64_t& val) const {
  return _domain.contains(val);
}

int64_t IntVar::lowerBound() const { return _domain.lowerBound(); }
int64_t IntVar::upperBound() const { return _domain.upperBound(); }

bool IntVar::operator==(const IntVar& other) const {
  if (identifier() != other.identifier() || _domain.operator!=(other._domain) ||
      annotations().size() != other.annotations().size()) {
    return false;
  }
  for (size_t i = 0; i < annotations().size(); ++i) {
    if (annotations().at(i).operator!=(other.annotations().at(i))) {
      return false;
    }
  }
  return true;
}

bool IntVar::operator!=(const IntVar& other) const {
  return !operator==(other);
}

bool IntVar::isFixed() const { return _domain.size() == 1; }

std::string IntVar::toString() const {
  std::string s = "var " + _domain.toString() + ": " + identifier();
  if (!annotations().empty()) {
    for (const auto& annotation : annotations()) {
      s += " :: " + annotation.toString();
    }
  }
  return s;
}

FloatVar::FloatVar(const std::string& identifier,
                   std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)),
      _domain(FloatSet(std::numeric_limits<double>::min(),
                       std::numeric_limits<double>::max())) {}

FloatVar::FloatVar(double val, const std::string& identifier,
                   std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(val) {}

FloatVar::FloatVar(double lb, double ub, const std::string& identifier,
                   std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(lb, ub) {}

FloatVar::FloatVar(std::vector<double>&& vals, const std::string& identifier,
                   std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(std::move(vals)) {}

const FloatSet& FloatVar::domain() const { return _domain; }

bool FloatVar::contains(const double& val) const {
  return _domain.contains(val);
}

double FloatVar::lowerBound() const { return _domain.lowerBound(); }
double FloatVar::upperBound() const { return _domain.upperBound(); }

bool FloatVar::operator==(const FloatVar& other) const {
  if (identifier() != other.identifier() || _domain.operator!=(other._domain) ||
      annotations().size() != other.annotations().size()) {
    return false;
  }
  for (size_t i = 0; i < annotations().size(); ++i) {
    if (annotations().at(i).operator!=(other.annotations().at(i))) {
      return false;
    }
  }
  return true;
}

bool FloatVar::operator!=(const FloatVar& other) const {
  return !operator==(other);
}

bool FloatVar::isFixed() const {
  return _domain.lowerBound() == _domain.upperBound();
}

std::string FloatVar::toString() const {
  std::string s = "var " + _domain.toString() + ": " + identifier();
  if (!annotations().empty()) {
    for (const auto& annotation : annotations()) {
      s += " :: " + annotation.toString();
    }
  }
  return s;
}

SetVar::SetVar(int64_t lb, int64_t ub, const std::string& identifier,
               std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(lb, ub) {}

SetVar::SetVar(std::vector<int64_t>&& vals, const std::string& identifier,
               std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(std::move(vals)) {}

SetVar::SetVar(IntSet&& domain, const std::string& identifier,
               std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(std::move(domain)) {}

SetVar::SetVar(const IntSet& domain, const std::string& identifier,
               std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(domain) {}

bool SetVar::contains(const IntSet&) const {
  throw FznException("Not yet implemented");
}
IntSet SetVar::lowerBound() const { return _domain; }
IntSet SetVar::upperBound() const { return _domain; }

bool SetVar::operator==(const SetVar& other) const {
  if (identifier() != other.identifier() || _domain.operator!=(other._domain) ||
      annotations().size() != other.annotations().size()) {
    return false;
  }
  for (size_t i = 0; i < annotations().size(); ++i) {
    if (annotations().at(i).operator!=(other.annotations().at(i))) {
      return false;
    }
  }
  return true;
}

bool SetVar::operator!=(const SetVar& other) const {
  return !operator==(other);
}

bool SetVar::isFixed() const { return false; }

std::string SetVar::toString() const {
  std::string s = "var " + _domain.toString() + ": " + identifier();
  if (!annotations().empty()) {
    for (const auto& annotation : annotations()) {
      s += " :: " + annotation.toString();
    }
  }
  return s;
}

VarArrayBase::VarArrayBase(const std::string& identifier,
                           std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)) {}

void VarArrayBase::interpretAnnotations(
    const std::unordered_map<std::string, Var>&) {
  for (const Annotation& annotation : annotations()) {
    if (annotation.identifier() == "output_array") {
      if (annotation.expressions().size() != 1) {
        throw FznException(
            "output_array annotation must define exactly one "
            "variable");
      }
      const auto& expressions = annotation.expressions().front();
      for (const auto& element : expressions) {
        if (!std::holds_alternative<IntSet>(element)) {
          throw FznException(
              "output_array annotation argument must be an array of int sets");
        }
        const auto& set = get<IntSet>(element);
        if (!set.isInterval() || set.lowerBound() != 1) {
          throw FznException("integer set must be an interval starting at 1");
        }
        _outputIndexSetSizes.emplace_back(set.upperBound());
      }
      _isOutput = true;
      break;
    }
  }
}

const std::vector<int64_t>& VarArrayBase::outputIndexSetSizes() const {
  return _outputIndexSetSizes;
}

BoolVarArray::BoolVarArray(const std::string& identifier,
                           std::vector<Annotation>&& annotations)
    : VarArrayTemplate<bool, BoolVar>(identifier, std::move(annotations)) {}

std::vector<shared_ptr<const BoolVar>> BoolVarArray::toVarVector(
    fznparser::Model& model) {
  std::vector<shared_ptr<const BoolVar>> params;
  params.clear();
  for (size_t i = 0; i < this->size(); ++i) {
    if (std::holds_alternative<bool>((*this)[i])) {
      params.emplace_back(
          shared_ptr<const BoolVar>(model.boolVarPar(get<bool>((*this)[i]))));
    } else {
      params.emplace_back(get<shared_ptr<const BoolVar>>((*this)[i]));
    }
  }
  return params;
}

bool BoolVarArray::operator==(const BoolVarArray& other) const {
  if (identifier() != other.identifier() || size() != other.size() ||
      annotations().size() != other.annotations().size()) {
    return false;
  }
  for (size_t i = 0; i < size(); ++i) {
    if (std::holds_alternative<bool>(at(i))) {
      if (!std::holds_alternative<bool>(other.at(i)) ||
          get<bool>(at(i)) != get<bool>(other.at(i))) {
        return false;
      }
    }
    if (std::holds_alternative<shared_ptr<const BoolVar>>(at(i))) {
      if (!std::holds_alternative<shared_ptr<const BoolVar>>(other.at(i)) ||
          get<shared_ptr<const BoolVar>>(at(i))->operator!=(
              *get<shared_ptr<const BoolVar>>(other.at(i)))) {
        return false;
      }
    }
  }
  for (size_t i = 0; i < annotations().size(); ++i) {
    if (annotations().at(i).operator!=(other.annotations().at(i))) {
      return false;
    }
  }
  return true;
}

bool BoolVarArray::operator!=(const BoolVarArray& other) const {
  return !operator==(other);
}

std::string BoolVarArray::toString() const {
  std::string s =
      identifier().empty()
          ? "["
          : ("array[1.." + std::to_string(size()) + "] of" +
             (isParArray() ? "" : " var") + " bool: " + identifier() + " = [");
  for (size_t i = 0; i < size(); ++i) {
    if (i != 0) {
      s += ", ";
    }
    if (std::holds_alternative<bool>(at(i))) {
      s += get<bool>(at(i)) ? "true" : "false";
    } else {
      s += get<shared_ptr<const BoolVar>>(at(i))->identifier();
    }
  }
  s += "]";
  if (!annotations().empty()) {
    for (const auto& annotation : annotations()) {
      s += " :: " + annotation.toString();
    }
  }
  return s;
}

IntVarArray::IntVarArray(const std::string& identifier,
                         std::vector<Annotation>&& annotations)
    : VarArrayTemplate<int64_t, IntVar>(identifier, std::move(annotations)) {}

std::vector<shared_ptr<const IntVar>> IntVarArray::toVarVector(
    fznparser::Model& model) {
  std::vector<shared_ptr<const IntVar>> params;
  params.clear();
  for (size_t i = 0; i < this->size(); ++i) {
    if (std::holds_alternative<int64_t>((*this)[i])) {
      params.emplace_back(shared_ptr<const IntVar>(
          model.addIntVarPar(get<int64_t>((*this)[i]))));
    } else {
      params.emplace_back(get<shared_ptr<const IntVar>>((*this)[i]));
    }
  }
  return params;
}

bool IntVarArray::operator==(const IntVarArray& other) const {
  if (identifier() != other.identifier() || size() != other.size() ||
      annotations().size() != other.annotations().size()) {
    return false;
  }
  for (size_t i = 0; i < size(); ++i) {
    if (std::holds_alternative<int64_t>(at(i))) {
      if (!std::holds_alternative<int64_t>(other.at(i)) ||
          get<int64_t>(at(i)) != get<int64_t>(other.at(i))) {
        return false;
      }
    }
    if (std::holds_alternative<shared_ptr<const IntVar>>(at(i))) {
      if (!std::holds_alternative<shared_ptr<const IntVar>>(other.at(i)) ||
          get<shared_ptr<const IntVar>>(at(i))->operator!=(
              *get<shared_ptr<const IntVar>>(other.at(i)))) {
        return false;
      }
    }
  }
  for (size_t i = 0; i < annotations().size(); ++i) {
    if (annotations().at(i).operator!=(other.annotations().at(i))) {
      return false;
    }
  }
  return true;
}

bool IntVarArray::operator!=(const IntVarArray& other) const {
  return !operator==(other);
}

std::string IntVarArray::toString() const {
  std::string s =
      identifier().empty()
          ? "["
          : ("array[1.." + std::to_string(size()) + "] of" +
             (isParArray() ? "" : " var") + " int: " + identifier() + " = [");
  for (size_t i = 0; i < size(); ++i) {
    if (i != 0) {
      s += ", ";
    }
    if (std::holds_alternative<int64_t>(at(i))) {
      s += std::to_string(get<int64_t>(at(i)));
    } else {
      s += get<shared_ptr<const IntVar>>(at(i))->identifier();
    }
  }
  s += "]";
  if (!annotations().empty()) {
    for (const auto& annotation : annotations()) {
      s += " :: " + annotation.toString();
    }
  }
  return s;
}

FloatVarArray::FloatVarArray(const std::string& identifier,
                             std::vector<Annotation>&& annotations)
    : VarArrayTemplate<double, FloatVar>(identifier, std::move(annotations)) {}

std::vector<shared_ptr<const FloatVar>> FloatVarArray::toVarVector(
    fznparser::Model& model) {
  std::vector<shared_ptr<const FloatVar>> params;
  params.clear();
  for (size_t i = 0; i < this->size(); ++i) {
    if (std::holds_alternative<double>((*this)[i])) {
      params.emplace_back(shared_ptr<const FloatVar>(
          model.addFloatVarPar(get<double>((*this)[i]))));
    } else {
      params.emplace_back(get<shared_ptr<const FloatVar>>((*this)[i]));
    }
  }
  return params;
}

bool FloatVarArray::operator==(const FloatVarArray& other) const {
  if (identifier() != other.identifier() || size() != other.size() ||
      annotations().size() != other.annotations().size()) {
    return false;
  }
  for (size_t i = 0; i < size(); ++i) {
    if (std::holds_alternative<double>(at(i))) {
      if (!std::holds_alternative<double>(other.at(i)) ||
          get<double>(at(i)) != get<double>(other.at(i))) {
        return false;
      }
    }
    if (std::holds_alternative<shared_ptr<const FloatVar>>(at(i))) {
      if (!std::holds_alternative<shared_ptr<const FloatVar>>(other.at(i)) ||
          get<shared_ptr<const FloatVar>>(at(i))->operator!=(
              *get<shared_ptr<const FloatVar>>(other.at(i)))) {
        return false;
      }
    }
  }
  for (size_t i = 0; i < annotations().size(); ++i) {
    if (annotations().at(i).operator!=(other.annotations().at(i))) {
      return false;
    }
  }
  return true;
}

bool FloatVarArray::operator!=(const FloatVarArray& other) const {
  return !operator==(other);
}

std::string FloatVarArray::toString() const {
  std::string s =
      identifier().empty()
          ? "["
          : ("array[1.." + std::to_string(size()) + "] of" +
             (isParArray() ? "" : " var") + " float: " + identifier() + " = [");
  for (size_t i = 0; i < size(); ++i) {
    if (i != 0) {
      s += ", ";
    }
    if (std::holds_alternative<double>(at(i))) {
      s += std::to_string(get<double>(at(i)));
    } else {
      s += get<shared_ptr<const FloatVar>>(at(i))->identifier();
    }
  }
  s += "]";
  if (!annotations().empty()) {
    for (const auto& annotation : annotations()) {
      s += " :: " + annotation.toString();
    }
  }
  return s;
}

SetVarArray::SetVarArray(const std::string& identifier,
                         std::vector<Annotation>&& annotations)
    : VarArrayTemplate<IntSet, SetVar>(identifier, std::move(annotations)) {}

std::vector<shared_ptr<const SetVar>> SetVarArray::toVarVector(
    fznparser::Model& model) {
  std::vector<shared_ptr<const SetVar>> params;
  params.clear();
  for (size_t i = 0; i < this->size(); ++i) {
    if (std::holds_alternative<IntSet>((*this)[i])) {
      params.emplace_back(shared_ptr<const SetVar>(
          model.addSetVarPar(get<IntSet>((*this)[i]))));
    } else {
      params.emplace_back(get<shared_ptr<const SetVar>>((*this)[i]));
    }
  }
  return params;
}

bool SetVarArray::operator==(const SetVarArray& other) const {
  if (identifier() != other.identifier() || size() != other.size() ||
      annotations().size() != other.annotations().size()) {
    return false;
  }
  for (size_t i = 0; i < size(); ++i) {
    if (std::holds_alternative<IntSet>(at(i))) {
      if (!std::holds_alternative<IntSet>(other.at(i)) ||
          get<IntSet>(at(i)).operator!=(get<IntSet>(other.at(i)))) {
        return false;
      }
    }
    if (std::holds_alternative<shared_ptr<const SetVar>>(at(i))) {
      if (!std::holds_alternative<shared_ptr<const SetVar>>(other.at(i)) ||
          get<shared_ptr<const SetVar>>(at(i))->operator!=(
              *get<shared_ptr<const SetVar>>(other.at(i)))) {
        return false;
      }
    }
  }
  for (size_t i = 0; i < annotations().size(); ++i) {
    if (annotations().at(i).operator!=(other.annotations().at(i))) {
      return false;
    }
  }
  return true;
}

bool SetVarArray::operator!=(const SetVarArray& other) const {
  return !operator==(other);
}

std::string SetVarArray::toString() const {
  std::string s = identifier().empty()
                      ? "["
                      : ("array[1.." + std::to_string(size()) + "] of" +
                         (isParArray() ? " int set" : " var set") + ": " +
                         identifier() + " = [");
  for (size_t i = 0; i < size(); ++i) {
    if (i != 0) {
      s += ", ";
    }
    if (std::holds_alternative<IntSet>(_vars.at(i))) {
      s += get<IntSet>(_vars.at(i)).toString();
    } else {
      s += get<shared_ptr<const SetVar>>(_vars.at(i))->identifier();
    }
  }
  s += "]";
  if (!annotations().empty()) {
    for (const auto& annotation : annotations()) {
      s += " :: " + annotation.toString();
    }
  }
  return s;
}

const std::string& Var::identifier() const {
  if (std::holds_alternative<std::shared_ptr<BoolVar>>(*this)) {
    return get<std::shared_ptr<BoolVar>>(*this)->identifier();
  } else if (std::holds_alternative<std::shared_ptr<IntVar>>(*this)) {
    return get<std::shared_ptr<IntVar>>(*this)->identifier();
  } else if (std::holds_alternative<std::shared_ptr<FloatVar>>(*this)) {
    return get<std::shared_ptr<FloatVar>>(*this)->identifier();
  } else if (std::holds_alternative<std::shared_ptr<SetVar>>(*this)) {
    return get<std::shared_ptr<SetVar>>(*this)->identifier();
  } else if (std::holds_alternative<std::shared_ptr<BoolVarArray>>(*this)) {
    return get<std::shared_ptr<BoolVarArray>>(*this)->identifier();
  } else if (std::holds_alternative<std::shared_ptr<IntVarArray>>(*this)) {
    return get<std::shared_ptr<IntVarArray>>(*this)->identifier();
  } else if (std::holds_alternative<std::shared_ptr<FloatVarArray>>(*this)) {
    return get<std::shared_ptr<FloatVarArray>>(*this)->identifier();
  } else if (std::holds_alternative<std::shared_ptr<SetVarArray>>(*this)) {
    return get<std::shared_ptr<SetVarArray>>(*this)->identifier();
  } else if (std::holds_alternative<std::shared_ptr<VarReference>>(*this)) {
    return get<std::shared_ptr<VarReference>>(*this)->identifier();
  }
  throw FznException("Unknown variable type");
}

bool Var::isArray() const {
  return std::holds_alternative<std::shared_ptr<BoolVarArray>>(*this) ||
         std::holds_alternative<std::shared_ptr<IntVarArray>>(*this) ||
         std::holds_alternative<std::shared_ptr<FloatVarArray>>(*this) ||
         std::holds_alternative<std::shared_ptr<SetVarArray>>(*this);
}

bool Var::operator==(const Var& other) const {
  if (std::holds_alternative<std::shared_ptr<BoolVar>>(*this) &&
      std::holds_alternative<std::shared_ptr<BoolVar>>(other)) {
    return get<std::shared_ptr<BoolVar>>(*this)->operator==(
        *get<std::shared_ptr<BoolVar>>(other));
  } else if (std::holds_alternative<std::shared_ptr<IntVar>>(*this) &&
             std::holds_alternative<std::shared_ptr<IntVar>>(other)) {
    return get<std::shared_ptr<IntVar>>(*this)->operator==(
        *get<std::shared_ptr<IntVar>>(other));
  } else if (std::holds_alternative<std::shared_ptr<FloatVar>>(*this) &&
             std::holds_alternative<std::shared_ptr<FloatVar>>(other)) {
    return get<std::shared_ptr<FloatVar>>(*this)->operator==(
        *get<std::shared_ptr<FloatVar>>(other));
  } else if (std::holds_alternative<std::shared_ptr<SetVar>>(*this) &&
             std::holds_alternative<std::shared_ptr<SetVar>>(other)) {
    return get<std::shared_ptr<SetVar>>(*this)->operator==(
        *get<std::shared_ptr<SetVar>>(other));
  } else if (std::holds_alternative<std::shared_ptr<BoolVarArray>>(*this) &&
             std::holds_alternative<std::shared_ptr<BoolVarArray>>(other)) {
    return get<std::shared_ptr<BoolVarArray>>(*this)->operator==(
        *get<std::shared_ptr<BoolVarArray>>(other));
  } else if (std::holds_alternative<std::shared_ptr<IntVarArray>>(*this) &&
             std::holds_alternative<std::shared_ptr<IntVarArray>>(other)) {
    return get<std::shared_ptr<IntVarArray>>(*this)->operator==(
        *get<std::shared_ptr<IntVarArray>>(other));
  } else if (std::holds_alternative<std::shared_ptr<FloatVarArray>>(*this) &&
             std::holds_alternative<std::shared_ptr<FloatVarArray>>(other)) {
    return get<std::shared_ptr<FloatVarArray>>(*this)->operator==(
        *get<std::shared_ptr<FloatVarArray>>(other));
  } else if (std::holds_alternative<std::shared_ptr<SetVarArray>>(*this) &&
             std::holds_alternative<std::shared_ptr<SetVarArray>>(other)) {
    return get<std::shared_ptr<SetVarArray>>(*this)->operator==(
        *get<std::shared_ptr<SetVarArray>>(other));
  } else if (std::holds_alternative<std::shared_ptr<VarReference>>(*this) &&
             std::holds_alternative<std::shared_ptr<VarReference>>(other)) {
    return get<std::shared_ptr<VarReference>>(*this)->operator==(
        *get<std::shared_ptr<VarReference>>(other));
  }
  return false;
}

bool Var::operator!=(const Var& other) const { return !operator==(other); }

std::string Var::toString() const {
  if (std::holds_alternative<std::shared_ptr<BoolVar>>(*this)) {
    return get<std::shared_ptr<BoolVar>>(*this)->toString();
  } else if (std::holds_alternative<std::shared_ptr<IntVar>>(*this)) {
    return get<std::shared_ptr<IntVar>>(*this)->toString();
  } else if (std::holds_alternative<std::shared_ptr<FloatVar>>(*this)) {
    return get<std::shared_ptr<FloatVar>>(*this)->toString();
  } else if (std::holds_alternative<std::shared_ptr<SetVar>>(*this)) {
    return get<std::shared_ptr<SetVar>>(*this)->toString();
  } else if (std::holds_alternative<std::shared_ptr<BoolVarArray>>(*this)) {
    return get<std::shared_ptr<BoolVarArray>>(*this)->toString();
  } else if (std::holds_alternative<std::shared_ptr<IntVarArray>>(*this)) {
    return get<std::shared_ptr<IntVarArray>>(*this)->toString();
  } else if (std::holds_alternative<std::shared_ptr<FloatVarArray>>(*this)) {
    return get<std::shared_ptr<FloatVarArray>>(*this)->toString();
  } else if (std::holds_alternative<std::shared_ptr<SetVarArray>>(*this)) {
    return get<std::shared_ptr<SetVarArray>>(*this)->toString();
  } else if (std::holds_alternative<std::shared_ptr<VarReference>>(*this)) {
    return get<std::shared_ptr<VarReference>>(*this)->toString();
  }
  return "";
}

void Var::interpretAnnotations(
    const std::unordered_map<std::string, Var>& varMapping) {
  if (std::holds_alternative<std::shared_ptr<BoolVar>>(*this)) {
    get<std::shared_ptr<BoolVar>>(*this)->interpretAnnotations(varMapping);
  } else if (std::holds_alternative<std::shared_ptr<IntVar>>(*this)) {
    get<std::shared_ptr<IntVar>>(*this)->interpretAnnotations(varMapping);
  } else if (std::holds_alternative<std::shared_ptr<FloatVar>>(*this)) {
    get<std::shared_ptr<FloatVar>>(*this)->interpretAnnotations(varMapping);
  } else if (std::holds_alternative<std::shared_ptr<SetVar>>(*this)) {
    get<std::shared_ptr<SetVar>>(*this)->interpretAnnotations(varMapping);
  } else if (std::holds_alternative<std::shared_ptr<BoolVarArray>>(*this)) {
    get<std::shared_ptr<BoolVarArray>>(*this)->interpretAnnotations(varMapping);
  } else if (std::holds_alternative<std::shared_ptr<IntVarArray>>(*this)) {
    get<std::shared_ptr<IntVarArray>>(*this)->interpretAnnotations(varMapping);
  } else if (std::holds_alternative<std::shared_ptr<FloatVarArray>>(*this)) {
    get<std::shared_ptr<FloatVarArray>>(*this)->interpretAnnotations(
        varMapping);
  } else if (std::holds_alternative<std::shared_ptr<SetVarArray>>(*this)) {
    get<std::shared_ptr<SetVarArray>>(*this)->interpretAnnotations(varMapping);
  } else if (std::holds_alternative<std::shared_ptr<VarReference>>(*this)) {
    get<std::shared_ptr<VarReference>>(*this)->interpretAnnotations(varMapping);
  }
}

bool Var::isFixed() const {
  if (std::holds_alternative<std::shared_ptr<BoolVar>>(*this)) {
    return get<std::shared_ptr<BoolVar>>(*this)->isFixed();
  } else if (std::holds_alternative<std::shared_ptr<IntVar>>(*this)) {
    return get<std::shared_ptr<IntVar>>(*this)->isFixed();
  } else if (std::holds_alternative<std::shared_ptr<FloatVar>>(*this)) {
    return get<std::shared_ptr<FloatVar>>(*this)->isFixed();
  } else if (std::holds_alternative<std::shared_ptr<SetVar>>(*this)) {
    return get<std::shared_ptr<SetVar>>(*this)->isFixed();
  } else if (std::holds_alternative<std::shared_ptr<BoolVarArray>>(*this)) {
    return get<std::shared_ptr<BoolVarArray>>(*this)->isFixed();
  } else if (std::holds_alternative<std::shared_ptr<IntVarArray>>(*this)) {
    return get<std::shared_ptr<IntVarArray>>(*this)->isFixed();
  } else if (std::holds_alternative<std::shared_ptr<FloatVarArray>>(*this)) {
    return get<std::shared_ptr<FloatVarArray>>(*this)->isFixed();
  } else if (std::holds_alternative<std::shared_ptr<SetVarArray>>(*this)) {
    return get<std::shared_ptr<SetVarArray>>(*this)->isFixed();
  } else if (std::holds_alternative<std::shared_ptr<VarReference>>(*this)) {
    return get<std::shared_ptr<VarReference>>(*this)->isFixed();
  }
  return false;
}

bool Var::isOutput() const {
  if (std::holds_alternative<std::shared_ptr<BoolVar>>(*this)) {
    return get<std::shared_ptr<BoolVar>>(*this)->isOutput();
  } else if (std::holds_alternative<std::shared_ptr<IntVar>>(*this)) {
    return get<std::shared_ptr<IntVar>>(*this)->isOutput();
  } else if (std::holds_alternative<std::shared_ptr<FloatVar>>(*this)) {
    return get<std::shared_ptr<FloatVar>>(*this)->isOutput();
  } else if (std::holds_alternative<std::shared_ptr<SetVar>>(*this)) {
    return get<std::shared_ptr<SetVar>>(*this)->isOutput();
  } else if (std::holds_alternative<std::shared_ptr<BoolVarArray>>(*this)) {
    return get<std::shared_ptr<BoolVarArray>>(*this)->isOutput();
  } else if (std::holds_alternative<std::shared_ptr<IntVarArray>>(*this)) {
    return get<std::shared_ptr<IntVarArray>>(*this)->isOutput();
  } else if (std::holds_alternative<std::shared_ptr<FloatVarArray>>(*this)) {
    return get<std::shared_ptr<FloatVarArray>>(*this)->isOutput();
  } else if (std::holds_alternative<std::shared_ptr<SetVarArray>>(*this)) {
    return get<std::shared_ptr<SetVarArray>>(*this)->isOutput();
  } else if (std::holds_alternative<std::shared_ptr<VarReference>>(*this)) {
    return get<std::shared_ptr<VarReference>>(*this)->isOutput();
  }
  return false;
}

VarReference::VarReference(const std::string& identifier, fznparser::Var source,
                           std::vector<fznparser::Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _source(std::move(source)) {}

const Var& VarReference::source() const { return _source; }

bool VarReference::isFixed() const { return _source.isFixed(); }

bool VarReference::operator==(const fznparser::VarReference& other) const {
  if (identifier() != other.identifier() ||
      annotations().size() != other.annotations().size()) {
    return false;
  }
  for (size_t i = 0; i < annotations().size(); ++i) {
    if (annotations().at(i).operator!=(other.annotations().at(i))) {
      return false;
    }
  }
  return _source.operator==(other._source);
}

bool VarReference::operator!=(const fznparser::VarReference& other) const {
  return !operator==(other);
}

std::string VarReference::toString() const {
  std::string s = "var: " + identifier() + " = " + _source.identifier();
  for (const auto& annotation : annotations()) {
    s += " :: " + annotation.toString();
  }
  s += ";";
  return s;
}

}  // namespace fznparser