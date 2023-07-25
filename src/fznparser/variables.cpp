#include "fznparser/variables.hpp"

#include "fznparser/model.hpp"

namespace fznparser {

using std::get;
using std::reference_wrapper;

VarBase::VarBase(const std::string_view& identifier,
                 std::vector<Annotation>&& annotations)
    : _identifier(identifier), _annotations(std::move(annotations)) {}

const std::string_view& VarBase::identifier() const { return _identifier; }

const std::vector<Annotation>& VarBase::annotations() const {
  return _annotations;
}

void VarBase::interpretAnnotations(
    const std::unordered_map<std::string_view, Variable>&) {
  for (const Annotation& annotation : _annotations) {
    if (annotation.identifier() == "output_var") {
      _isOutput = true;
    } else if (annotation.identifier() == "is_defined_var") {
      _isDefinedVar = true;
    }
  }
}

void VarBase::addAnnotation(const std::string_view& identifier) {
  _annotations.push_back(Annotation(identifier));
}

void VarBase::addAnnotation(const Annotation& annotation) {
  _annotations.push_back(annotation);
}

void VarBase::addAnnotation(const std::string_view& identifier,
                            std::vector<AnnotationExpression>&& expression) {
  _annotations.push_back(Annotation(
      identifier, std::move(std::vector<std::vector<AnnotationExpression>>{
                      std::move(expression)})));
}

void VarBase::addAnnotation(const std::string_view& identifier,
                            AnnotationExpression&& expression) {
  _annotations.push_back(Annotation(
      identifier,
      std::move(std::vector<std::vector<AnnotationExpression>>{
          std::move(std::vector<AnnotationExpression>{expression})})));
}

bool VarBase::isOutput() const { return _isOutput; }
bool VarBase::isDefinedVar() const { return _isDefinedVar; }

BoolVar::BoolVar(const std::string_view& identifier,
                 std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(2) {}

BoolVar::BoolVar(bool b, const std::string_view& identifier,
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
  std::string s = "var bool: " + std::string(identifier());
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

IntVar::IntVar(const std::string_view& identifier,
               std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)),
      _domain(IntSet(std::numeric_limits<int64_t>::min(),
                     std::numeric_limits<int64_t>::max())) {}

IntVar::IntVar(int64_t val, const std::string_view& identifier,
               std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(val) {}

IntVar::IntVar(int64_t lb, int64_t ub, const std::string_view& identifier,
               std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(lb, ub) {}

IntVar::IntVar(std::vector<int64_t>&& vals, const std::string_view& identifier,
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
  std::string s =
      "var " + _domain.toString() + ": " + std::string(identifier());
  if (!annotations().empty()) {
    for (const auto& annotation : annotations()) {
      s += " :: " + annotation.toString();
    }
  }
  return s;
}

FloatVar::FloatVar(const std::string_view& identifier,
                   std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)),
      _domain(FloatSet(std::numeric_limits<double>::min(),
                       std::numeric_limits<double>::max())) {}

FloatVar::FloatVar(double val, const std::string_view& identifier,
                   std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(val) {}

FloatVar::FloatVar(double lb, double ub, const std::string_view& identifier,
                   std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(lb, ub) {}

FloatVar::FloatVar(std::vector<double>&& vals,
                   const std::string_view& identifier,
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
  std::string s =
      "var " + _domain.toString() + ": " + std::string(identifier());
  if (!annotations().empty()) {
    for (const auto& annotation : annotations()) {
      s += " :: " + annotation.toString();
    }
  }
  return s;
}

SetVar::SetVar(int64_t lb, int64_t ub, const std::string_view& identifier,
               std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(lb, ub) {}

SetVar::SetVar(std::vector<int64_t>&& vals, const std::string_view& identifier,
               std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(std::move(vals)) {}

SetVar::SetVar(IntSet&& domain, const std::string_view& identifier,
               std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)), _domain(std::move(domain)) {}

SetVar::SetVar(const IntSet& domain, const std::string_view& identifier,
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
  std::string s =
      "var " + _domain.toString() + ": " + std::string(identifier());
  if (!annotations().empty()) {
    for (const auto& annotation : annotations()) {
      s += " :: " + annotation.toString();
    }
  }
  return s;
}

VarArrayBase::VarArrayBase(const std::string_view& identifier,
                           std::vector<Annotation>&& annotations)
    : VarBase(identifier, std::move(annotations)) {}

void VarArrayBase::interpretAnnotations(
    const std::unordered_map<std::string_view, Variable>&) {
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
        const IntSet& set = get<IntSet>(element);
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

BoolVarArray::BoolVarArray(const std::string_view& identifier,
                           std::vector<Annotation>&& annotations)
    : VarArrayTemplate<bool, BoolVar>(identifier, std::move(annotations)) {}

std::vector<reference_wrapper<const BoolVar>> BoolVarArray::toVarVector(
    fznparser::Model& model) {
  std::vector<reference_wrapper<const BoolVar>> params;
  params.clear();
  for (size_t i = 0; i < this->size(); ++i) {
    if (std::holds_alternative<bool>((*this)[i])) {
      params.emplace_back(reference_wrapper<const BoolVar>(
          model.boolVarPar(get<bool>((*this)[i]))));
    } else {
      params.emplace_back(get<reference_wrapper<const BoolVar>>((*this)[i]));
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
    if (holds_alternative<bool>(at(i))) {
      if (!holds_alternative<bool>(other.at(i)) ||
          get<bool>(at(i)) != get<bool>(other.at(i))) {
        return false;
      }
    }
    if (holds_alternative<reference_wrapper<const BoolVar>>(at(i))) {
      if (!holds_alternative<reference_wrapper<const BoolVar>>(other.at(i)) ||
          get<reference_wrapper<const BoolVar>>(at(i)).get().operator!=(
              get<reference_wrapper<const BoolVar>>(other.at(i)).get())) {
        return false;
      }
    }
  }
  return true;
  for (size_t i = 0; i < annotations().size(); ++i) {
    if (annotations().at(i).operator!=(other.annotations().at(i))) {
      return false;
    }
  }
}

bool BoolVarArray::operator!=(const BoolVarArray& other) const {
  return !operator==(other);
}

std::string BoolVarArray::toString() const {
  std::string s = identifier().empty()
                      ? "["
                      : ("array[1.." + std::to_string(size()) + "] of" +
                         (isParArray() ? "" : " var") +
                         " bool: " + std::string(identifier()) + " = [");
  for (size_t i = 0; i < size(); ++i) {
    if (i != 0) {
      s += ", ";
    }
    if (holds_alternative<bool>(at(i))) {
      s += get<bool>(at(i)) ? "true" : "false";
    } else {
      s += get<reference_wrapper<const BoolVar>>(at(i)).get().identifier();
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

IntVarArray::IntVarArray(const std::string_view& identifier,
                         std::vector<Annotation>&& annotations)
    : VarArrayTemplate<int64_t, IntVar>(identifier, std::move(annotations)) {}

std::vector<reference_wrapper<const IntVar>> IntVarArray::toVarVector(
    fznparser::Model& model) {
  std::vector<reference_wrapper<const IntVar>> params;
  params.clear();
  for (size_t i = 0; i < this->size(); ++i) {
    if (std::holds_alternative<int64_t>((*this)[i])) {
      params.emplace_back(reference_wrapper<const IntVar>(
          model.addIntVarPar(get<int64_t>((*this)[i]))));
    } else {
      params.emplace_back(get<reference_wrapper<const IntVar>>((*this)[i]));
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
    if (holds_alternative<int64_t>(at(i))) {
      if (!holds_alternative<int64_t>(other.at(i)) ||
          get<int64_t>(at(i)) != get<int64_t>(other.at(i))) {
        return false;
      }
    }
    if (holds_alternative<reference_wrapper<const IntVar>>(at(i))) {
      if (!holds_alternative<reference_wrapper<const IntVar>>(other.at(i)) ||
          get<reference_wrapper<const IntVar>>(at(i)).get().operator!=(
              get<reference_wrapper<const IntVar>>(other.at(i)).get())) {
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
  std::string s = identifier().empty()
                      ? "["
                      : ("array[1.." + std::to_string(size()) + "] of" +
                         (isParArray() ? "" : " var") +
                         " int: " + std::string(identifier()) + " = [");
  for (size_t i = 0; i < size(); ++i) {
    if (i != 0) {
      s += ", ";
    }
    if (holds_alternative<int64_t>(at(i))) {
      s += std::to_string(get<int64_t>(at(i)));
    } else {
      s += get<reference_wrapper<const IntVar>>(at(i)).get().identifier();
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

FloatVarArray::FloatVarArray(const std::string_view& identifier,
                             std::vector<Annotation>&& annotations)
    : VarArrayTemplate<double, FloatVar>(identifier, std::move(annotations)) {}

std::vector<reference_wrapper<const FloatVar>> FloatVarArray::toVarVector(
    fznparser::Model& model) {
  std::vector<reference_wrapper<const FloatVar>> params;
  params.clear();
  for (size_t i = 0; i < this->size(); ++i) {
    if (std::holds_alternative<double>((*this)[i])) {
      params.emplace_back(reference_wrapper<const FloatVar>(
          model.addFloatVarPar(get<double>((*this)[i]))));
    } else {
      params.emplace_back(get<reference_wrapper<const FloatVar>>((*this)[i]));
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
    if (holds_alternative<double>(at(i))) {
      if (!holds_alternative<double>(other.at(i)) ||
          get<double>(at(i)) != get<double>(other.at(i))) {
        return false;
      }
    }
    if (holds_alternative<reference_wrapper<const FloatVar>>(at(i))) {
      if (!holds_alternative<reference_wrapper<const FloatVar>>(other.at(i)) ||
          get<reference_wrapper<const FloatVar>>(at(i)).get().operator!=(
              get<reference_wrapper<const FloatVar>>(other.at(i)).get())) {
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
  std::string s = identifier().empty()
                      ? "["
                      : ("array[1.." + std::to_string(size()) + "] of" +
                         (isParArray() ? "" : " var") +
                         " float: " + std::string(identifier()) + " = [");
  for (size_t i = 0; i < size(); ++i) {
    if (i != 0) {
      s += ", ";
    }
    if (holds_alternative<double>(at(i))) {
      s += std::to_string(get<double>(at(i)));
    } else {
      s += get<reference_wrapper<const FloatVar>>(at(i)).get().identifier();
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

SetVarArray::SetVarArray(const std::string_view& identifier,
                         std::vector<Annotation>&& annotations)
    : VarArrayTemplate<IntSet, SetVar>(identifier, std::move(annotations)) {}

std::vector<reference_wrapper<const SetVar>> SetVarArray::toVarVector(
    fznparser::Model& model) {
  std::vector<reference_wrapper<const SetVar>> params;
  params.clear();
  for (size_t i = 0; i < this->size(); ++i) {
    if (std::holds_alternative<IntSet>((*this)[i])) {
      params.emplace_back(reference_wrapper<const SetVar>(
          model.addSetVarPar(get<IntSet>((*this)[i]))));
    } else {
      params.emplace_back(get<reference_wrapper<const SetVar>>((*this)[i]));
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
    if (holds_alternative<IntSet>(at(i))) {
      if (!holds_alternative<IntSet>(other.at(i)) ||
          get<IntSet>(at(i)).operator!=(get<IntSet>(other.at(i)))) {
        return false;
      }
    }
    if (holds_alternative<reference_wrapper<const SetVar>>(at(i))) {
      if (!holds_alternative<reference_wrapper<const SetVar>>(other.at(i)) ||
          get<reference_wrapper<const SetVar>>(at(i)).get().operator!=(
              get<reference_wrapper<const SetVar>>(other.at(i)).get())) {
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
                         std::string(identifier()) + " = [");
  for (size_t i = 0; i < size(); ++i) {
    if (i != 0) {
      s += ", ";
    }
    if (holds_alternative<IntSet>(_vars.at(i))) {
      s += get<IntSet>(_vars.at(i)).toString();
    } else {
      s += get<reference_wrapper<const SetVar>>(_vars.at(i)).get().identifier();
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

const std::string_view& Variable::identifier() const {
  if (std::holds_alternative<BoolVar>(*this)) {
    return get<BoolVar>(*this).identifier();
  } else if (std::holds_alternative<IntVar>(*this)) {
    return get<IntVar>(*this).identifier();
  } else if (std::holds_alternative<FloatVar>(*this)) {
    return get<FloatVar>(*this).identifier();
  } else if (std::holds_alternative<SetVar>(*this)) {
    return get<SetVar>(*this).identifier();
  } else if (std::holds_alternative<BoolVarArray>(*this)) {
    return get<BoolVarArray>(*this).identifier();
  } else if (std::holds_alternative<IntVarArray>(*this)) {
    return get<IntVarArray>(*this).identifier();
  } else if (std::holds_alternative<FloatVarArray>(*this)) {
    return get<FloatVarArray>(*this).identifier();
  } else if (std::holds_alternative<SetVarArray>(*this)) {
    return get<SetVarArray>(*this).identifier();
  }
  throw std::runtime_error("Unknown variable type");
}

bool Variable::isArray() const {
  return std::holds_alternative<BoolVarArray>(*this) ||
         std::holds_alternative<IntVarArray>(*this) ||
         std::holds_alternative<FloatVarArray>(*this) ||
         std::holds_alternative<SetVarArray>(*this);
}

bool Variable::operator==(const Variable& other) const {
  if (std::holds_alternative<BoolVar>(*this) &&
      std::holds_alternative<BoolVar>(other)) {
    return get<BoolVar>(*this).operator==(get<BoolVar>(other));
  } else if (std::holds_alternative<IntVar>(*this) &&
             std::holds_alternative<IntVar>(other)) {
    return get<IntVar>(*this).operator==(get<IntVar>(other));
  } else if (std::holds_alternative<FloatVar>(*this) &&
             std::holds_alternative<FloatVar>(other)) {
    return get<FloatVar>(*this).operator==(get<FloatVar>(other));
  } else if (std::holds_alternative<SetVar>(*this) &&
             std::holds_alternative<SetVar>(other)) {
    return get<SetVar>(*this).operator==(get<SetVar>(other));
  } else if (std::holds_alternative<BoolVarArray>(*this) &&
             std::holds_alternative<BoolVarArray>(other)) {
    return get<BoolVarArray>(*this).operator==(get<BoolVarArray>(other));
  } else if (std::holds_alternative<IntVarArray>(*this) &&
             std::holds_alternative<IntVarArray>(other)) {
    return get<IntVarArray>(*this).operator==(get<IntVarArray>(other));
  } else if (std::holds_alternative<FloatVarArray>(*this) &&
             std::holds_alternative<FloatVarArray>(other)) {
    return get<FloatVarArray>(*this).operator==(get<FloatVarArray>(other));
  } else if (std::holds_alternative<SetVarArray>(*this) &&
             std::holds_alternative<SetVarArray>(other)) {
    return get<SetVarArray>(*this).operator==(get<SetVarArray>(other));
  }
  return false;
}

bool Variable::operator!=(const Variable& other) const {
  return !operator==(other);
}

std::string Variable::toString() const {
  if (std::holds_alternative<BoolVar>(*this)) {
    return get<BoolVar>(*this).toString();
  } else if (std::holds_alternative<IntVar>(*this)) {
    return get<IntVar>(*this).toString();
  } else if (std::holds_alternative<FloatVar>(*this)) {
    return get<FloatVar>(*this).toString();
  } else if (std::holds_alternative<SetVar>(*this)) {
    return get<SetVar>(*this).toString();
  } else if (std::holds_alternative<BoolVarArray>(*this)) {
    return get<BoolVarArray>(*this).toString();
  } else if (std::holds_alternative<IntVarArray>(*this)) {
    return get<IntVarArray>(*this).toString();
  } else if (std::holds_alternative<FloatVarArray>(*this)) {
    return get<FloatVarArray>(*this).toString();
  } else if (std::holds_alternative<SetVarArray>(*this)) {
    return get<SetVarArray>(*this).toString();
  }
  return "";
}

void Variable::interpretAnnotations(
    const std::unordered_map<std::string_view, Variable>& variableMapping) {
  if (std::holds_alternative<BoolVar>(*this)) {
    get<BoolVar>(*this).interpretAnnotations(variableMapping);
  } else if (std::holds_alternative<IntVar>(*this)) {
    get<IntVar>(*this).interpretAnnotations(variableMapping);
  } else if (std::holds_alternative<FloatVar>(*this)) {
    get<FloatVar>(*this).interpretAnnotations(variableMapping);
  } else if (std::holds_alternative<SetVar>(*this)) {
    get<SetVar>(*this).interpretAnnotations(variableMapping);
  } else if (std::holds_alternative<BoolVarArray>(*this)) {
    get<BoolVarArray>(*this).interpretAnnotations(variableMapping);
  } else if (std::holds_alternative<IntVarArray>(*this)) {
    get<IntVarArray>(*this).interpretAnnotations(variableMapping);
  } else if (std::holds_alternative<FloatVarArray>(*this)) {
    get<FloatVarArray>(*this).interpretAnnotations(variableMapping);
  } else if (std::holds_alternative<SetVarArray>(*this)) {
    get<SetVarArray>(*this).interpretAnnotations(variableMapping);
  }
}

bool Variable::isOutput() const {
  if (std::holds_alternative<BoolVar>(*this)) {
    get<BoolVar>(*this).isOutput();
  } else if (std::holds_alternative<IntVar>(*this)) {
    get<IntVar>(*this).isOutput();
  } else if (std::holds_alternative<FloatVar>(*this)) {
    get<FloatVar>(*this).isOutput();
  } else if (std::holds_alternative<SetVar>(*this)) {
    get<SetVar>(*this).isOutput();
  } else if (std::holds_alternative<BoolVarArray>(*this)) {
    get<BoolVarArray>(*this).isOutput();
  } else if (std::holds_alternative<IntVarArray>(*this)) {
    get<IntVarArray>(*this).isOutput();
  } else if (std::holds_alternative<FloatVarArray>(*this)) {
    get<FloatVarArray>(*this).isOutput();
  } else if (std::holds_alternative<SetVarArray>(*this)) {
    get<SetVarArray>(*this).isOutput();
  }
  return false;
}

}  // namespace fznparser