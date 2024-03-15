#include "fznparser/transformer/modelTransformer.hpp"

#include <algorithm>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <variant>
#include <vector>

#include "fznparser/except.hpp"
#include "fznparser/parser/toString.hpp"

namespace fznparser {

using namespace fznparser::parser;
using boost::get;
using parser::toString;
using std::shared_ptr;

std::string getIdentifier(VarDeclItem& t) {
  return t.type() == typeid(BasicVarDecl) ? get<BasicVarDecl>(t).identifier
                                          : get<ArrayVarDecl>(t).identifier;
}

template <typename T>
IntSet toIntSet(const T& intSet) {
  if (intSet.type() == typeid(SetLiteralEmpty)) {
    return IntSet(std::vector<int64_t>());
  }
  if (intSet.type() == typeid(IntSetLiteralBounded)) {
    return IntSet(get<IntSetLiteralBounded>(intSet).lowerBound,
                  get<IntSetLiteralBounded>(intSet).upperBound);
  } else if (intSet.type() == typeid(IntSetLiteralSet)) {
    return IntSet(
        static_cast<std::vector<int64_t>>(get<IntSetLiteralSet>(intSet)));
  }
  throw FznException("Invalid int set variant type: " + toString(intSet));
}

template <typename T>
FloatSet toFloatSet(const T& floatSet) {
  if (floatSet.type() == typeid(SetLiteralEmpty)) {
    return FloatSet(std::vector<double>());
  }
  if (floatSet.type() == typeid(FloatSetLiteralBounded)) {
    return FloatSet(get<FloatSetLiteralBounded>(floatSet).lowerBound,
                    get<FloatSetLiteralBounded>(floatSet).upperBound);
  } else if (floatSet.type() == typeid(FloatSetLiteralSet)) {
    return FloatSet(
        static_cast<std::vector<double>>(get<FloatSetLiteralSet>(floatSet)));
  }
  throw FznException("Invalid float set variant type: " + toString(floatSet));
}

IntVar toIntVar(const BasicVarDecl& var,
                std::vector<fznparser::Annotation>&& annotations) {
  if (var.type.type() == typeid(BasicVarIntTypeUnbounded)) {
    return IntVar{std::numeric_limits<int64_t>::min(),
                  std::numeric_limits<int64_t>::max(), var.identifier,
                  std::move(annotations)};
  } else if (var.type.type() == typeid(BasicVarIntTypeBounded)) {
    return IntVar{get<BasicVarIntTypeBounded>(var.type).lowerBound,
                  get<BasicVarIntTypeBounded>(var.type).upperBound,
                  var.identifier, std::move(annotations)};
  } else if (var.type.type() == typeid(BasicVarIntTypeSet)) {
    return IntVar{std::vector<int64_t>(get<BasicVarIntTypeSet>(var.type)),
                  var.identifier, std::move(annotations)};
  }
  throw FznException("Invalid var int declaration: " + toString(var));
}

FloatVar toFloatVar(const BasicVarDecl& var,
                    std::vector<fznparser::Annotation>&& annotations) {
  if (var.type.type() == typeid(BasicVarFloatTypeUnbounded)) {
    return FloatVar{std::numeric_limits<double>::min(),
                    std::numeric_limits<double>::max(), var.identifier,
                    std::move(annotations)};
  } else if (var.type.type() == typeid(BasicVarFloatTypeBounded)) {
    return FloatVar{get<BasicVarFloatTypeBounded>(var.type).lowerBound,
                    get<BasicVarFloatTypeBounded>(var.type).upperBound,
                    var.identifier, std::move(annotations)};
  }
  throw FznException("Invalid var float declaration: " + toString(var));
}

SetVar toSetVar(const BasicVarDecl& var,
                std::vector<fznparser::Annotation>&& annotations) {
  if (var.type.type() == typeid(BasicVarSetTypeBounded)) {
    return SetVar{get<BasicVarSetTypeBounded>(var.type).lowerBound,
                  get<BasicVarSetTypeBounded>(var.type).upperBound,
                  var.identifier, std::move(annotations)};
  } else if (var.type.type() == typeid(BasicVarSetTypeSet)) {
    return SetVar{std::vector<int64_t>(get<BasicVarSetTypeSet>(var.type)),
                  var.identifier, std::move(annotations)};
  }
  throw FznException("Invalid set var declaration: " + toString(var));
}

// NOTE: that these functions return true if a variable can be assigned to the
// value. An int var can be assigned to a set of integers.

bool isBool(BasicParType t) { return t == BasicParType::BOOL; }
bool isBool(const std::type_info& t) { return t == typeid(bool); }

bool isInt(BasicParType t) { return t == BasicParType::INT; }
bool isInt(const std::type_info& t) { return t == typeid(int64_t); }

bool isFloat(BasicParType t) { return t == BasicParType::FLOAT; }
bool isFloat(const std::type_info& t) { return t == typeid(double); }

bool isIntSet(BasicParType t) { return t == BasicParType::SET_OF_INT; }

bool isVarArray(const Var& t) {
  return holds_alternative<std::shared_ptr<BoolVarArray>>(t) ||
         holds_alternative<std::shared_ptr<IntVarArray>>(t) ||
         holds_alternative<std::shared_ptr<FloatVarArray>>(t) ||
         holds_alternative<std::shared_ptr<SetVarArray>>(t);
}

bool isIntSet(const std::type_info& t) {
  return t == typeid(SetLiteralEmpty) || t == typeid(IntSetLiteralBounded) ||
         t == typeid(IntSetLiteralSet);
}

bool isFloatSet(const std::type_info& t) {
  return t == typeid(SetLiteralEmpty) || t == typeid(FloatSetLiteralBounded) ||
         t == typeid(FloatSetLiteralSet);
}

bool isBoolVarExpression(const std::type_info& t) { return isBool(t); }

bool isIntVarExpression(const std::type_info& t) {
  return t == typeid(int64_t) || t == typeid(IntSetLiteralBounded) ||
         t == typeid(IntSetLiteralSet);
}

bool isFloatVarExpression(const std::type_info& t) {
  return t == typeid(double) || t == typeid(FloatSetLiteralBounded) ||
         t == typeid(FloatSetLiteralSet);
}

bool isSetVarExpression(const std::type_info& t) {
  return t == typeid(SetLiteralEmpty) || t == typeid(IntSetLiteralBounded) ||
         t == typeid(IntSetLiteralSet);
}

bool isBoolVar(const std::type_info& t) {
  return t == typeid(BasicVarBoolType);
}

bool isBoolVar(const ArrayVarDecl& array) {
  return isBoolVar(array.type.type.type());
}

bool isBoolVar(const BasicVarDecl& var) { return isBoolVar(var.type.type()); }

bool isIntVar(const std::type_info& t) {
  return t == typeid(BasicVarIntTypeUnbounded) ||
         t == typeid(BasicVarIntTypeBounded) || t == typeid(BasicVarIntTypeSet);
}

bool isIntVar(const ArrayVarDecl& array) {
  return isIntVar(array.type.type.type());
}

bool isIntVar(const BasicVarDecl& var) { return isIntVar(var.type.type()); }

bool isFloatVar(const std::type_info& t) {
  return t == typeid(BasicVarFloatTypeUnbounded) ||
         t == typeid(BasicVarFloatTypeBounded);
}

bool isFloatVar(const ArrayVarDecl& array) {
  return isFloatVar(array.type.type.type());
}

bool isFloatVar(const BasicVarDecl& var) { return isFloatVar(var.type.type()); }

bool isSetVar(const std::type_info& t) {
  return t == typeid(BasicVarSetTypeBounded) || t == typeid(BasicVarSetTypeSet);
}

bool isSetVar(const ArrayVarDecl& array) {
  return isSetVar(array.type.type.type());
}

bool isSetVar(const BasicVarDecl& var) { return isSetVar(var.type.type()); }

bool sameType(const BasicVarType& a, const BasicExpr& b) {
  return (isBoolVar(a.type()) && isBoolVarExpression(b.type())) ||
         (isIntVar(a.type()) && isIntVarExpression(b.type())) ||
         (isFloatVar(a.type()) && isFloatVarExpression(b.type())) ||
         (isSetVar(a.type()) && isSetVarExpression(b.type()));
}

bool sameType(const BasicVarType& a, const BasicVarType& b) {
  return (isBoolVar(a.type()) && isBoolVar(b.type())) ||
         (isIntVar(a.type()) && isIntVar(b.type())) ||
         (isFloatVar(a.type()) && isFloatVar(b.type())) ||
         (isSetVar(a.type()) && isSetVar(b.type()));
}

bool sameType(const ArrayVarDecl& a, const BasicExpr& b) {
  return sameType(a.type.type, b);
}

bool sameType(const ArrayVarDecl& a, const BasicVarType& b) {
  return sameType(a.type.type, b);
}

bool sameType(const BasicParType& a, const ParExpr& b) {
  return (isBool(a) && isBool(b.type())) || (isInt(a) && isInt(b.type())) ||
         (isFloat(a) && isFloat(b.type())) ||
         (isIntSet(a) && isIntSet(b.type()));
}

BasicExpr toBasicExpr(const ParExpr& parExpr) {
  if (parExpr.type() == typeid(bool)) {
    return get<bool>(parExpr);
  } else if (parExpr.type() == typeid(int64_t)) {
    return get<int64_t>(parExpr);
  } else if (parExpr.type() == typeid(double)) {
    return get<double>(parExpr);
  } else if (parExpr.type() == typeid(SetLiteralEmpty)) {
    return get<SetLiteralEmpty>(parExpr);
  } else if (parExpr.type() == typeid(IntSetLiteralBounded)) {
    return get<IntSetLiteralBounded>(parExpr);
  } else if (parExpr.type() == typeid(FloatSetLiteralBounded)) {
    return get<FloatSetLiteralBounded>(parExpr);
  } else if (parExpr.type() == typeid(FloatSetLiteralSet)) {
    return get<FloatSetLiteralSet>(parExpr);
  }
  throw FznException("Exception when transforming \"" + toString(parExpr) +
                     "\": parameter has invalid type");
}

ArrayLiteral toArrayLiteral(const ParArrayLiteral& parArrayLiteral) {
  ArrayLiteral arrayLiteral;
  for (const BasicLiteralExpr& expr : parArrayLiteral) {
    arrayLiteral.push_back(toBasicExpr(expr));
  }
  return arrayLiteral;
}

Expr toExpr(const ParExpr& parExpr) {
  if (parExpr.type() == typeid(bool)) {
    return get<bool>(parExpr);
  } else if (parExpr.type() == typeid(int64_t)) {
    return get<int64_t>(parExpr);
  } else if (parExpr.type() == typeid(double)) {
    return get<double>(parExpr);
  } else if (parExpr.type() == typeid(SetLiteralEmpty)) {
    return get<SetLiteralEmpty>(parExpr);
  } else if (parExpr.type() == typeid(IntSetLiteralBounded)) {
    return get<IntSetLiteralBounded>(parExpr);
  } else if (parExpr.type() == typeid(FloatSetLiteralBounded)) {
    return get<FloatSetLiteralBounded>(parExpr);
  } else if (parExpr.type() == typeid(FloatSetLiteralSet)) {
    return get<FloatSetLiteralSet>(parExpr);
  } else if (parExpr.type() == typeid(ParArrayLiteral)) {
    return toArrayLiteral(get<ParArrayLiteral>(parExpr));
  }
  throw FznException("Exception when transforming \"" + toString(parExpr) +
                     "\": parameter has invalid type");
}

void ModelTransformer::replaceParameters(VarDeclItem& varDeclItem) {
  if (varDeclItem.type() == typeid(BasicVarDecl)) {
    replaceParameters(get<BasicVarDecl>(varDeclItem));
  } else if (varDeclItem.type() == typeid(ArrayVarDecl)) {
    replaceParameters(get<ArrayVarDecl>(varDeclItem));
  } else {
    throw std::runtime_error("replaceParameter(): invalid variant type: " +
                             toString(varDeclItem));
  }
}

void ModelTransformer::replaceParameters(BasicVarDecl& basicVarDecl) {
  if (!basicVarDecl.expr.has_value() ||
      basicVarDecl.expr.value().type() != typeid(std::string)) {
    return;
  }
  const std::string& identifier = get<std::string>(basicVarDecl.expr.value());
  if (_varDeclItems.find(identifier) != _varDeclItems.end()) {
    return;
  }
  if (_parDeclItems.find(identifier) == _parDeclItems.end()) {
    throw FznException("Error when replacing parameters for variable \"" +
                       toString(basicVarDecl) +
                       "\": Reference to undefined variable or parameter " +
                       "\"" + identifier + "\".");
  }
  basicVarDecl.expr = toBasicExpr(_parDeclItems.at(identifier).expr);
}

void ModelTransformer::replaceParameters(ArrayVarDecl& arrayVarDecl) {
  for (size_t i = 0; i < arrayVarDecl.literals.size(); ++i) {
    if (arrayVarDecl.literals.at(i).type() != typeid(std::string)) {
      continue;
    }
    const std::string& identifier =
        get<std::string>(arrayVarDecl.literals.at(i));
    if (_varDeclItems.find(identifier) != _varDeclItems.end()) {
      continue;
    }
    if (_parDeclItems.find(identifier) == _parDeclItems.end()) {
      throw FznException("Error when validating array literal \"" +
                         toString(arrayVarDecl.literals.at(i)) +
                         "\" in array \"" + toString(arrayVarDecl) +
                         "\" at index " + std::to_string(i + 1) +
                         ": Reference to undefined variable or parameter \"" +
                         identifier + "\".");
    }
    arrayVarDecl.literals.at(i) =
        toBasicExpr(_parDeclItems.at(identifier).expr);
  }
}

void ModelTransformer::replaceParameters(ArrayLiteral& array) {
  for (size_t i = 0; i < array.size(); ++i) {
    if (array.at(i).type() != typeid(std::string)) {
      continue;
    }
    const std::string& identifier = get<std::string>(array.at(i));
    if (_varDeclItems.find(identifier) != _varDeclItems.end()) {
      continue;
    }
    if (_parDeclItems.find(identifier) == _parDeclItems.end()) {
      throw FznException("Error when replacing parameters for constraint \"" +
                         toString(array) +
                         "\": Reference to undefined variable or parameter " +
                         "\"" + identifier + "\".");
    }
    array.at(i) = toBasicExpr(_parDeclItems.at(identifier).expr);
  }
}

void ModelTransformer::replaceParameters(ConstraintItem& constraint) {
  for (size_t i = 0; i < constraint.expressions.size(); ++i) {
    if (constraint.expressions.at(i).type() == typeid(ArrayLiteral)) {
      replaceParameters(get<ArrayLiteral>(constraint.expressions.at(i)));
      continue;
    }
    if (constraint.expressions.at(i).type() != typeid(std::string)) {
      continue;
    }
    const std::string& identifier =
        get<std::string>(constraint.expressions.at(i));
    if (_varDeclItems.find(identifier) != _varDeclItems.end()) {
      continue;
    }
    if (_parDeclItems.find(identifier) == _parDeclItems.end()) {
      throw FznException("Error when replacing parameters for constraint \"" +
                         toString(constraint) +
                         "\": Reference to undefined variable or parameter " +
                         "\"" + identifier + "\".");
    }

    constraint.expressions.at(i) = toExpr(_parDeclItems.at(identifier).expr);
  }
}

void typeCheck(BasicParType type, const ParExpr& parExpr) {
  if (!sameType(type, parExpr)) {
    throw FznException(
        "type mismatch, expected parameter type \"" + toString(type) +
        "\" but parameter expression has type \"" + toString(parExpr) + "\"");
  }
}

void typeCheck(const BasicParTypeArray& type, const ParExpr& parExpr) {
  if (parExpr.type() != typeid(ParArrayLiteral)) {
    throw FznException("type mismatch, expected array literal");
  }
  const auto& parArrayLiteral = get<ParArrayLiteral>(parExpr);
  if (type.indexSet.size() != static_cast<int64_t>(parArrayLiteral.size())) {
    throw FznException("Array literal size does not match index set size");
  }
  for (const BasicLiteralExpr& pe : parArrayLiteral) {
    typeCheck(type.type, pe);
  }
}

void typeCheck(const ParType& type, const ParExpr& parExpr) {
  if (type.type() == typeid(BasicParType)) {
    typeCheck(get<BasicParType>(type), parExpr);
  } else {
    typeCheck(get<BasicParTypeArray>(type), parExpr);
  }
}

void typeCheck(const ParDeclItem& parDeclItem) {
  typeCheck(parDeclItem.type, parDeclItem.expr);
}

void typeCheck(const BasicVarType& type, const BasicExpr& expr) {
  if (expr.type() != typeid(std::string) && !sameType(type, expr)) {
    throw FznException("type mismatch between var type \"" + toString(type) +
                       "\" and expression \"" + toString(expr) + "\"");
  }
}

bool isStringLiteral(const std::string& s) {
  return !s.empty() && s.front() == '"';
}

void ModelTransformer::validate(const parser::Annotation& annotation) const {
  for (const AnnExpr& annExpr : annotation.expressions) {
    if (annExpr.empty()) {
      continue;
    }
    const auto& t = annExpr.front().type();
    for (size_t i = 1; i < annExpr.size(); ++i) {
      if (annExpr.at(i).type() != t) {
        throw FznException("Error when validating annotation \"" +
                           toString(annotation) + "\": type mismatch");
      }
    }
    if (t == typeid(std::string)) {
      bool isLit = isStringLiteral(get<std::string>(annExpr.front()));
      for (size_t i = 1; i < annExpr.size(); ++i) {
        if (isStringLiteral(get<std::string>(annExpr.at(i))) != isLit) {
          throw FznException("Error when validating annotation \"" +
                             toString(annotation) +
                             "\": mix of string literals and identifiers");
        }
      }
    } else if (t == typeid(x3::forward_ast<parser::Annotation>)) {
      for (const BasicAnnExpr& basicAnnExpr : annExpr) {
        validate(get<x3::forward_ast<parser::Annotation>>(basicAnnExpr).get());
      }
    }
  }
}

void ModelTransformer::validate(const parser::BasicVarDecl& var) const {
  if (var.expr.has_value()) {
    typeCheck(var.type, var.expr.value());
  }
  for (const parser::Annotation& annotation : var.annotations) {
    validate(annotation);
  }
}

void ModelTransformer::validate(const ArrayVarDecl& arrayVarDecl) const {
  if (arrayVarDecl.type.indexSet.size() !=
      static_cast<int64_t>(arrayVarDecl.literals.size())) {
    throw FznException("Var literal array size does not match index set size");
  }
  for (size_t i = 0; i < arrayVarDecl.literals.size(); ++i) {
    if (arrayVarDecl.literals.at(i).type() != typeid(std::string)) {
      if (!sameType(arrayVarDecl, arrayVarDecl.literals.at(i))) {
        throw FznException(
            "Error when validating variable array \"" + toString(arrayVarDecl) +
            "\" at index " + std::to_string(i + 1) + ": \"" +
            toString(arrayVarDecl.literals.at(i)) + "\". Type mismatch.");
      }
      continue;
    }
    const std::string identifier =
        get<std::string>(arrayVarDecl.literals.at(i));
    if (_varDeclItems.find(identifier) == _varDeclItems.end()) {
      throw FznException(
          "Error when validating array literal \"" +
          toString(arrayVarDecl.literals.at(i)) + "\" in array \"" +
          toString(arrayVarDecl) + "\" at index " + std::to_string(i + 1) +
          ": Reference to undefined variable \"" + identifier + "\".");
    }
    const VarDeclItem& varDeclItem = _varDeclItems.at(identifier);
    if (varDeclItem.type() != typeid(BasicVarDecl)) {
      throw FznException("Error when validating array literal \"" +
                         toString(arrayVarDecl.literals.at(i)) +
                         "\"in array \"" + toString(arrayVarDecl) +
                         "\" at index " + std::to_string(i + 1) +
                         ": variable \"" + toString(varDeclItem) +
                         "\" has incompatible type.");
    }
    if (!sameType(arrayVarDecl, get<BasicVarDecl>(varDeclItem).type)) {
      throw FznException(
          "Error when validating variable array \"" + toString(arrayVarDecl) +
          "\" at index " + std::to_string(i + 1) + ": \"" +
          toString(arrayVarDecl.literals.at(i)) +
          "\". Type mismatch for variable \"" + toString(varDeclItem) + "\"");
    }
  }
  for (const parser::Annotation& annotation : arrayVarDecl.annotations) {
    validate(annotation);
  }
}

void ModelTransformer::validate(const VarDeclItem& varDeclItem) const {
  if (varDeclItem.type() == typeid(BasicVarDecl)) {
    validate(get<BasicVarDecl>(varDeclItem));
  } else {
    validate(get<ArrayVarDecl>(varDeclItem));
  }
}

void replaceEmptySets(ParDeclItem& parDeclItem) {
  if (parDeclItem.expr.type() == typeid(SetLiteralEmpty)) {
    parDeclItem.expr = IntSetLiteralSet{};
  } else if (parDeclItem.expr.type() == typeid(ParArrayLiteral)) {
    for (BasicLiteralExpr& expr : get<ParArrayLiteral>(parDeclItem.expr)) {
      if (expr.type() == typeid(SetLiteralEmpty)) {
        expr = IntSetLiteralSet{};
      }
    }
  }
}

void replaceEmptySets(ArrayVarDecl& arrayVarDecl) {
  for (auto& literal : arrayVarDecl.literals) {
    if (literal.type() == typeid(SetLiteralEmpty)) {
      literal = IntSetLiteralSet{};
    }
  }
}

void replaceEmptySets(BasicVarDecl& var) {
  if (var.expr.has_value() &&
      var.expr.value().type() == typeid(SetLiteralEmpty)) {
    var.expr = IntSetLiteralSet{};
  }
}

void replaceEmptySets(VarDeclItem& var) {
  if (var.type() == typeid(BasicVarDecl)) {
    replaceEmptySets(get<BasicVarDecl>(var));
  } else if (var.type() == typeid(ArrayVarDecl)) {
    replaceEmptySets(get<ArrayVarDecl>(var));
  } else {
    throw std::runtime_error("replaceEmptySets(): Invalid variant type: " +
                             toString(var));
  }
}

ModelTransformer::ModelTransformer(parser::Model&& model)
    : _model(std::move(model)),
      _predDeclItems(),
      _parDeclItems(),
      _varDeclItems() {
  for (ParDeclItem& parDeclItem : _model.parDeclItems) {
    replaceEmptySets(parDeclItem);
    typeCheck(parDeclItem);
    _parDeclItems.emplace(parDeclItem.identifier, parDeclItem);
  }
  for (VarDeclItem& varDeclItem : _model.varDeclItems) {
    replaceEmptySets(varDeclItem);
    replaceParameters(varDeclItem);
    _varDeclItems.emplace(getIdentifier(varDeclItem), varDeclItem);
  }
  for (ConstraintItem& constraintItem : _model.constraintItems) {
    replaceParameters(constraintItem);
  }
  for (const VarDeclItem& varDeclItem : _model.varDeclItems) {
    validate(varDeclItem);
  }
}

fznparser::Annotation transformAnnotation(const parser::Annotation& annotation);

AnnotationExpression transformAnnotationExpression(
    const parser::BasicAnnExpr& expr) {
  if (expr.type() == typeid(bool)) {
    return AnnotationExpression{get<bool>(expr)};
  } else if (expr.type() == typeid(int64_t)) {
    return AnnotationExpression{get<int64_t>(expr)};
  } else if (expr.type() == typeid(double)) {
    return AnnotationExpression{get<double>(expr)};
  } else if (isIntSet(expr.type())) {
    return AnnotationExpression{toIntSet(expr)};
  } else if (isFloatSet(expr.type())) {
    return AnnotationExpression{toFloatSet(expr)};
  } else if (expr.type() == typeid(x3::forward_ast<parser::Annotation>)) {
    return AnnotationExpression{transformAnnotation(
        get<x3::forward_ast<parser::Annotation>>(expr).get())};
  }
  throw FznException(
      "Invalid Annotation variant type for annotation expression \"" +
      toString(expr) + "\"");
}

fznparser::Annotation transformAnnotation(
    const parser::Annotation& annotation) {
  std::vector<std::vector<AnnotationExpression>> expressions;
  for (const parser::AnnExpr& annExpr : annotation.expressions) {
    expressions.emplace_back();
    for (const parser::BasicAnnExpr& basicAnnExpr : annExpr) {
      expressions.back().emplace_back(
          transformAnnotationExpression(basicAnnExpr));
    }
  }
  return {annotation.identifier, std::move(expressions)};
}

Var ModelTransformer::transformVar(const std::unordered_map<std::string, Var>& vars,
                                   const BasicVarDecl& var) {
  std::vector<fznparser::Annotation> annotations;
  annotations.reserve(var.annotations.size());
  for (const parser::Annotation& ann : var.annotations) {
    annotations.push_back(transformAnnotation(ann));
  }
  if (var.expr.has_value()) {
    const BasicExpr expr = var.expr.value();
    if (expr.type() == typeid(bool)) {
      return Var{std::make_shared<BoolVar>(
          BoolVar{get<bool>(expr), var.identifier, std::move(annotations)})};
    } else if (expr.type() == typeid(int64_t)) {
      return Var{std::make_shared<IntVar>(
          IntVar{get<int64_t>(expr), var.identifier, std::move(annotations)})};
    } else if (expr.type() == typeid(double)) {
      return Var{std::make_shared<FloatVar>(
          FloatVar{get<double>(expr), var.identifier, std::move(annotations)})};
    } else if (isIntSet(expr.type())) {
      return Var{std::make_shared<SetVar>(
          SetVar{toIntSet(expr), var.identifier, std::move(annotations)})};
    } else if (expr.type() == typeid(std::string)) {
      const auto& refIdentifier = get<std::string>(expr);
      if (vars.contains(refIdentifier)) {
        return Var{std::make_shared<VarReference>(var.identifier, vars.at(refIdentifier), std::move(annotations))};
      }
    }
  } else if (isBoolVar(var)) {
    return Var{std::make_shared<BoolVar>(
        BoolVar{var.identifier, std::move(annotations)})};
  } else if (isIntVar(var)) {
    return Var{std::make_shared<IntVar>(toIntVar(var, std::move(annotations)))};
  } else if (isFloatVar(var)) {
    return Var{
        std::make_shared<FloatVar>(toFloatVar(var, std::move(annotations)))};
  } else if (isSetVar(var)) {
    return Var{std::make_shared<SetVar>(toSetVar(var, std::move(annotations)))};
  }
  throw FznException("transformVar(): Invalid variant type: " + toString(var));
}

template <class ArrayType, class VarType, typename ParType>
std::shared_ptr<ArrayType> generateVarArray(
    const std::unordered_map<std::string, Var>& vars,
    const ArrayVarDecl& arrayVarDecl) {
  std::vector<fznparser::Annotation> annotations;
  annotations.reserve(arrayVarDecl.annotations.size());
  for (const parser::Annotation& ann : arrayVarDecl.annotations) {
    annotations.push_back(transformAnnotation(ann));
  }
  std::shared_ptr<ArrayType> res = std::make_shared<ArrayType>(
      arrayVarDecl.identifier, std::move(annotations));
  for (const BasicExpr& basicExpr : arrayVarDecl.literals) {
    if (basicExpr.type() == typeid(std::string)) {
      const std::string& identifier = get<std::string>(basicExpr);
      if (vars.find(identifier) == vars.end()) {
        throw FznException(
            "Error when validating variable \"" + toString(arrayVarDecl) +
            "\": Reference to undefined " + toString(arrayVarDecl.type.type) +
            " variable with identifier \"" + identifier + "\"");
      }
      const Var& var = vars.at(identifier);
      if (!std::holds_alternative<std::shared_ptr<VarType>>(var)) {
        throw FznException("Reference to non-" +
                           toString(arrayVarDecl.type.type) +
                           " variable with identifier \"" + identifier + "\"");
      }
      res->append(get<std::shared_ptr<VarType>>(var));
    } else if (basicExpr.type() == typeid(ParType)) {
      res->append(get<ParType>(basicExpr));
    } else {
      throw FznException("Invalid " + toString(arrayVarDecl.type.type) +
                         " variable array literal");
    }
  }
  return res;
}

std::shared_ptr<SetVarArray> generateSetVarArray(
    const std::unordered_map<std::string, Var>& vars,
    const ArrayVarDecl& arrayVarDecl) {
  std::vector<fznparser::Annotation> annotations;
  annotations.reserve(arrayVarDecl.annotations.size());
  for (const parser::Annotation& ann : arrayVarDecl.annotations) {
    annotations.push_back(transformAnnotation(ann));
  }
  std::shared_ptr<SetVarArray> res = std::make_shared<SetVarArray>(
      arrayVarDecl.identifier, std::move(annotations));
  for (const BasicExpr& basicExpr : arrayVarDecl.literals) {
    if (basicExpr.type() == typeid(std::string)) {
      const std::string& identifier = get<std::string>(basicExpr);
      if (vars.find(identifier) == vars.end()) {
        throw FznException(
            "Reference to undefined set variable with identifier \"" +
            identifier + "\"");
      }
      Var var = vars.at(identifier);
      if (!std::holds_alternative<std::shared_ptr<SetVar>>(var)) {
        throw FznException("Reference to non-set variable with identifier \"" +
                           identifier + "\"");
      }
      res->append(get<std::shared_ptr<SetVar>>(var));
    } else if (isIntSet(basicExpr.type())) {
      res->append(toIntSet(basicExpr));
    } else {
      throw FznException("Invalid bool variable array literal");
    }
  }
  return res;
}

template <class ArrayType, class VarType, typename ParType>
std::shared_ptr<ArrayType> generateArgArray(
    const std::unordered_map<std::string, Var>& vars,
    const ArrayLiteral& arrayLiteral) {
  std::shared_ptr<ArrayType> res = std::make_shared<ArrayType>("");
  for (const BasicExpr& basicExpr : arrayLiteral) {
    if (basicExpr.type() == typeid(std::string)) {
      const std::string& identifier = get<std::string>(basicExpr);
      if (vars.find(identifier) == vars.end()) {
        throw FznException(
            "Error when validating array literal \"" + toString(arrayLiteral) +
            "\": Reference to undefined variable with identifier \"" +
            identifier + "\"");
      }
      const Var& var = vars.at(identifier);
      if (!std::holds_alternative<std::shared_ptr<VarType>>(var)) {
        throw FznException(
            "Reference type mismatch when parsing array literal \"" +
            toString(arrayLiteral) + "\" and variable \"" + var.toString() +
            "\"");
      }
      res->append(get<std::shared_ptr<VarType>>(var));
    } else if (basicExpr.type() == typeid(ParType)) {
      res->append(get<ParType>(basicExpr));
    } else {
      throw FznException("Invalid " + toString(arrayLiteral) +
                         " variable array literal");
    }
  }
  return res;
}

std::shared_ptr<SetVarArray> generateSetVarArray(
    const std::unordered_map<std::string, Var>& vars,
    const ArrayLiteral& arrayLiteral) {
  std::shared_ptr<SetVarArray> res = std::make_shared<SetVarArray>("");
  for (const BasicExpr& basicExpr : arrayLiteral) {
    if (basicExpr.type() == typeid(std::string)) {
      const std::string& identifier = get<std::string>(basicExpr);
      if (vars.find(identifier) == vars.end()) {
        throw FznException(
            "Error when validating array literal \"" + toString(arrayLiteral) +
            "\": Reference to undefined variable with identifier \"" +
            identifier + "\"");
      }
      Var var = vars.at(identifier);
      if (!std::holds_alternative<std::shared_ptr<SetVar>>(var)) {
        throw FznException(
            "Reference type mismatch when parsing array literal \"" +
            toString(arrayLiteral) + "\" and variable \"" + var.toString() +
            "\"");
      }
      res->append(get<std::shared_ptr<SetVar>>(var));
    } else if (isIntSet(basicExpr.type())) {
      res->append(toIntSet(basicExpr));
    } else {
      throw FznException("Invalid " + toString(arrayLiteral) +
                         " variable array literal");
    }
  }
  return res;
}

const std::type_info& ModelTransformer::arrayType(
    const std::unordered_map<std::string, Var>&,
    const ArrayLiteral& array) const {
  bool onlyEmptySets = true;
  for (const BasicExpr& expr : array) {
    if (expr.type() == typeid(std::string)) {
      onlyEmptySets = false;
      const std::string& identifier = get<std::string>(expr);
      if (_varDeclItems.find(identifier) == _varDeclItems.end() &&
          _parDeclItems.find(identifier) == _parDeclItems.end()) {
        throw FznException("Reference to undefined variable or parameter \"" +
                           identifier + "\" in array literal: \"" +
                           toString(array) + "\"");
      }
      if (_varDeclItems.find(identifier) != _varDeclItems.end()) {
        const VarDeclItem& varDeclItem = _varDeclItems.at(identifier);
        if (varDeclItem.type() != typeid(BasicVarDecl)) {
          throw FznException("Invalid array literal in argument array \"" +
                             toString(array) + "\", variable \"" +
                             toString(varDeclItem) + "\" is a variable array");
        }
        const auto& t = get<BasicVarDecl>(varDeclItem).type.type();
        if (isBoolVar(t)) {
          return typeid(bool);
        } else if (isIntVar(t)) {
          return typeid(int64_t);
        } else if (isFloatVar(t)) {
          return typeid(double);
        } else if (isSetVar(t)) {
          return typeid(IntSetLiteralBounded);
        }
        throw FznException("Invalid array literal in argument array \"" +
                           toString(array) + "\", variable \"" +
                           toString(varDeclItem) + "\" has invalid type");
      }
      return _parDeclItems.at(identifier).expr.type();
    } else if (expr.type() != typeid(SetLiteralEmpty)) {
      return expr.type();
    }
  }
  return onlyEmptySets ? typeid(SetLiteralEmpty) : typeid(std::string);
}

Arg ModelTransformer::transformArgArray(
    const std::unordered_map<std::string, Var>& vars,
    const ArrayLiteral& array) {
  const auto& t = arrayType(vars, array);
  if (t == typeid(std::string)) {
    // only variables

  } else if (t == typeid(bool)) {
    return Arg{generateArgArray<BoolVarArray, BoolVar, bool>(vars, array)};
  } else if (t == typeid(int64_t)) {
    return Arg{generateArgArray<IntVarArray, IntVar, int64_t>(vars, array)};
  } else if (t == typeid(double)) {
    return Arg{generateArgArray<FloatVarArray, FloatVar, double>(vars, array)};
  } else if (isIntSet(t)) {
    return Arg{generateSetVarArray(vars, array)};
  } else if (isFloatSet(t)) {
    std::shared_ptr<FloatSetArray> res = std::make_shared<FloatSetArray>();
    for (const BasicExpr& expr : array) {
      if (!isFloatSet(expr.type())) {
        throw FznException("Invalid float set array literal");
      }
      res->push_back(toFloatSet(expr));
    }
    return res;
  }
  throw FznException("transformArgArray(): Invalid variant type: " +
                     toString(array));
}

Var ModelTransformer::transformVarArray(
    const std::unordered_map<std::string, Var>& vars,
    const ArrayVarDecl& arrayVarDecl) {
  if (isBoolVar(arrayVarDecl)) {
    return Var{
        generateVarArray<BoolVarArray, BoolVar, bool>(vars, arrayVarDecl)};
  } else if (isIntVar(arrayVarDecl)) {
    return Var{
        generateVarArray<IntVarArray, IntVar, int64_t>(vars, arrayVarDecl)};
  } else if (isFloatVar(arrayVarDecl)) {
    return Var{
        generateVarArray<FloatVarArray, FloatVar, double>(vars, arrayVarDecl)};
  } else if (isSetVar(arrayVarDecl)) {
    return Var{generateSetVarArray(vars, arrayVarDecl)};
  }
  throw FznException("transformVarArray(): Invalid variant type: " +
                     toString(arrayVarDecl));
}

Var ModelTransformer::transform(
    const std::unordered_map<std::string, fznparser::Var>& vars,
    const VarDeclItem& varDeclItem) {
  return varDeclItem.type() == typeid(BasicVarDecl)
             ? transformVar(vars, get<BasicVarDecl>(varDeclItem))
             : transformVarArray(vars, get<ArrayVarDecl>(varDeclItem));
}

SolveType ModelTransformer::transform(
    const std::unordered_map<std::string, Var>& vars,
    const parser::SolveItem& solveItem) {
  const std::vector<parser::Annotation> parserAnns =
      solveItem.type() == typeid(SolveSatisfy)
          ? get<SolveSatisfy>(solveItem).annotations
          : get<SolveOptimize>(solveItem).annotations;

  std::vector<fznparser::Annotation> annotations;
  annotations.reserve(parserAnns.size());
  for (const parser::Annotation& ann : parserAnns) {
    annotations.push_back(transformAnnotation(ann));
  }

  if (solveItem.type() == typeid(SolveSatisfy)) {
    return SolveType(std::move(annotations));
  }
  const SolveOptimize& solveOptimize = get<SolveOptimize>(solveItem);
  if (solveOptimize.expr.type() != typeid(std::string)) {
    return SolveType(std::move(annotations));
  }
  const std::string& identifier = get<std::string>(solveOptimize.expr);
  if (vars.find(identifier) == vars.end()) {
    if (_parDeclItems.find(identifier) != _parDeclItems.end()) {
      return SolveType(std::move(annotations));
    }
    throw FznException(
        "Error when transforming solve type \"" + toString(solveItem) +
        "\": Reference to undefined variable \"" + identifier + "\"");
  }
  const Var& var = vars.at(identifier);
  if (isVarArray(var)) {
    throw FznException(
        "Error when transforming solve item \"" + toString(solveItem) +
        "\": Cannot optimize over array variable with identifier \"" +
        identifier + "\"");
  }
  return {solveOptimize.type == OptimizationType::MINIMIZE
              ? ProblemType::MINIMIZE
              : ProblemType::MAXIMIZE,
          var, std::move(annotations)};
}

Arg ModelTransformer::transformArgument(
    const std::unordered_map<std::string, Var>& vars,
    const parser::Expr& expr) {
  if (expr.type() == typeid(bool)) {
    return Arg{BoolArg{get<bool>(expr)}};
  } else if (expr.type() == typeid(int64_t)) {
    return Arg{IntArg{get<int64_t>(expr)}};
  } else if (expr.type() == typeid(double)) {
    return Arg{FloatArg{get<double>(expr)}};
  } else if (isIntSet(expr.type())) {
    return Arg{IntSetArg(toIntSet(expr))};
  } else if (isFloatSet(expr.type())) {
    return Arg{toFloatSet(expr)};
  } else if (expr.type() == typeid(ArrayLiteral)) {
    return Arg{transformArgArray(vars, get<ArrayLiteral>(expr))};
  } else if (expr.type() == typeid(std::string)) {
    const std::string& identifier = get<std::string>(expr);
    if (vars.find(identifier) == vars.end()) {
      throw FznException(
          "Error when transforming argument \"" + toString(expr) +
          "\": Reference to undefined variable \"" + identifier + "\"");
    }
    Var var = vars.at(identifier);
    if (std::holds_alternative<std::shared_ptr<BoolVar>>(var)) {
      return Arg{std::get<std::shared_ptr<BoolVar>>(var)};
    } else if (std::holds_alternative<std::shared_ptr<IntVar>>(var)) {
      return Arg{std::get<std::shared_ptr<IntVar>>(var)};
    } else if (std::holds_alternative<std::shared_ptr<FloatVar>>(var)) {
      return Arg{std::get<std::shared_ptr<FloatVar>>(var)};
    } else if (std::holds_alternative<std::shared_ptr<SetVar>>(var)) {
      return Arg{std::get<std::shared_ptr<SetVar>>(var)};
    } else if (std::holds_alternative<std::shared_ptr<BoolVarArray>>(var)) {
      return Arg{std::get<std::shared_ptr<BoolVarArray>>(var)};
    } else if (std::holds_alternative<std::shared_ptr<IntVarArray>>(var)) {
      return Arg{std::get<std::shared_ptr<IntVarArray>>(var)};
    } else if (std::holds_alternative<std::shared_ptr<FloatVarArray>>(var)) {
      return Arg{std::get<std::shared_ptr<FloatVarArray>>(var)};
    } else if (std::holds_alternative<std::shared_ptr<SetVarArray>>(var)) {
      return Arg{std::get<std::shared_ptr<SetVarArray>>(var)};
    }
  }
  throw FznException("Invalid variant type for argument \"" + toString(expr) +
                     "\"");
}

Constraint ModelTransformer::transform(
    const std::unordered_map<std::string, Var>& vars,
    const parser::ConstraintItem& constraintItem) {
  std::vector<fznparser::Annotation> annotations;
  annotations.reserve(constraintItem.annotations.size());
  for (const parser::Annotation& ann : constraintItem.annotations) {
    annotations.push_back(transformAnnotation(ann));
  }

  std::vector<Arg> arguments;
  arguments.reserve(constraintItem.expressions.size());
  for (const Expr& expr : constraintItem.expressions) {
    arguments.push_back(transformArgument(vars, expr));
  }

  return {constraintItem.identifier, std::move(arguments),
          std::move(annotations)};
}

fznparser::Model ModelTransformer::generateModel() {
  std::unordered_map<std::string, Var> vars;
  for (const VarDeclItem& varDeclItem : _model.varDeclItems) {
    Var var = transform(vars, varDeclItem);
    std::string identifier(var.identifier());
    vars.emplace(std::make_pair<std::string, Var>(std::move(identifier),
                                                  std::move(var)))
        .first->second.interpretAnnotations(vars);
  }
  std::vector<Constraint> constraints;
  constraints.clear();
  for (const ConstraintItem& constraintItem : _model.constraintItems) {
    constraints.push_back(transform(vars, constraintItem));
    constraints.back().interpretAnnotations(vars);
  }

  return {std::move(vars), std::move(constraints),
          transform(vars, _model.solveItem)};
}

}  // namespace fznparser