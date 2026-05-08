#include "fznparser/parser/toString.hpp"

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <string>
#include <vector>

#include "fznparser/parser/grammarAst.hpp"

namespace fznparser::parser {

using boost::get;
using std::to_string;

std::string toString(const bool b) { return to_string(b); }
std::string toString(const int64_t i) { return to_string(i); }
std::string toString(const double f) { return to_string(f); }

template <class T>
std::string vecToString(const std::vector<T>& vec) {  // NOLINT(*-no-recursion)
  std::string str;
  for (size_t i = 0; i < vec.size(); ++i) {
    if (i != 0) {
      str += ", ";
    }
    str += toString(vec[i]);
  }
  return str;
}

std::string toString(const IntRange& range) {
  return to_string(range.lowerBound) + ".." + to_string(range.upperBound);
}

std::string toString(const FloatRange& range) {
  return to_string(range.lowerBound) + ".." + to_string(range.upperBound);
}

std::string toString(const BasicParType basicParType) {
  switch (basicParType) {
    case BasicParType::BOOL:
      return "bool";
    case BasicParType::INT:
      return "int";
    case BasicParType::FLOAT:
      return "float";
    case BasicParType::SET_OF_INT:
      return "set of int";
    default:
      return "";
  }
}

std::string toString(const IndexSet& indexSet) {
  return "1.." + to_string(indexSet.upperBound);
}

std::string toString(const BasicParTypeArray& array) {
  return "array [" + toString(array.indexSet) + "] of " + toString(array.type);
}

std::string toString(const ParType& parType) {
  return parType.type() == typeid(BasicParType)
             ? toString(get<BasicParType>(parType))
             : toString(get<BasicParTypeArray>(parType));
}

std::string toString(const BasicVarBoolType&) { return "var bool"; }

std::string toString(const BasicVarIntTypeUnbounded&) { return "var int"; }

std::string toString(const BasicVarIntTypeBounded& intVar) {
  return "var " + toString(static_cast<IntRange>(intVar));
}

std::string toString(const BasicVarIntTypeSet& intVar) {
  return "var {" + vecToString(intVar) + "}";
}

std::string toString(const BasicVarFloatTypeUnbounded&) { return "var float"; }

std::string toString(const BasicVarFloatTypeBounded& floatVar) {
  return "var " + toString(static_cast<FloatRange>(floatVar));
}

std::string toString(const BasicVarSetTypeBounded& var) {
  return "set of " + toString(static_cast<IntRange>(var));
}
std::string toString(const BasicVarSetTypeSet& var) {
  return "set of {" + vecToString(var) + "}";
}

std::string toString(const BasicVarType& type) {
  if (type.type() == typeid(BasicVarBoolType)) {
    return toString(get<BasicVarBoolType>(type));
  }
  if (type.type() == typeid(BasicVarIntTypeUnbounded)) {
    return toString(get<BasicVarIntTypeUnbounded>(type));
  }
  if (type.type() == typeid(BasicVarIntTypeBounded)) {
    return toString(get<BasicVarIntTypeBounded>(type));
  }
  if (type.type() == typeid(BasicVarIntTypeSet)) {
    return toString(get<BasicVarIntTypeSet>(type));
  }
  if (type.type() == typeid(BasicVarFloatTypeBounded)) {
    return toString(get<BasicVarFloatTypeBounded>(type));
  }
  if (type.type() == typeid(BasicVarFloatTypeUnbounded)) {
    return toString(get<BasicVarFloatTypeUnbounded>(type));
  }
  if (type.type() == typeid(BasicVarSetTypeBounded)) {
    return toString(get<BasicVarSetTypeBounded>(type));
  }
  if (type.type() == typeid(BasicVarSetTypeSet)) {
    return toString(get<BasicVarSetTypeSet>(type));
  }
  return "";
}
std::string toString(const ArrayVarType& type) {
  return "array [" + toString(type.indexSet) + "] of " + toString(type.type);
}
std::string toString(const IndexSetUnbounded&) { return ""; }
std::string toString(const PredIndexSet&) { return ""; }
std::string toString(const BasicPredParamTypeIntBounded&) { return ""; }
std::string toString(const BasicPredParamTypeFloatBounded&) { return ""; }
std::string toString(const BasicPredParamTypeIntSet&) { return ""; }
std::string toString(const BasicPredParamTypeSetBounded&) { return ""; }
std::string toString(const BasicPredParamTypeSetSet&) { return ""; }
std::string toString(const BasicPredParamType&) { return ""; }
std::string toString(const PredParamArrayType&) { return ""; }
std::string toString(const PredParamType&) { return ""; }
std::string toString(const PredParam&) { return ""; }
std::string toString(const SetLiteralEmpty&) { return "{}"; }
std::string toString(const IntSetLiteralBounded& set) {
  return toString(static_cast<IntRange>(set));
}
std::string toString(const IntSetLiteralSet& set) {
  return "{" + vecToString(set) + "}";
}
std::string toString(const FloatSetLiteralBounded& set) {
  return toString(static_cast<FloatRange>(set));
}
std::string toString(const FloatSetLiteralSet& set) {
  return "{" + vecToString(set) + "}";
}
std::string toString(const BasicLiteralExpr& expr) {
  if (expr.type() == typeid(bool)) {
    return to_string(get<bool>(expr));
  }
  if (expr.type() == typeid(int64_t)) {
    return to_string(get<int64_t>(expr));
  }
  if (expr.type() == typeid(double)) {
    return to_string(get<double>(expr));
  }
  if (expr.type() == typeid(SetLiteralEmpty)) {
    return toString(get<SetLiteralEmpty>(expr));
  }
  if (expr.type() == typeid(IntSetLiteralBounded)) {
    return toString(get<IntSetLiteralBounded>(expr));
  }
  if (expr.type() == typeid(IntSetLiteralSet)) {
    return toString(get<IntSetLiteralSet>(expr));
  }
  if (expr.type() == typeid(FloatSetLiteralBounded)) {
    return toString(get<FloatSetLiteralBounded>(expr));
  }
  if (expr.type() == typeid(FloatSetLiteralSet)) {
    return toString(get<FloatSetLiteralSet>(expr));
  }
  return "";
}
std::string toString(const BasicExpr& expr) {
  if (expr.type() == typeid(bool)) {
    return to_string(get<bool>(expr));
  }
  if (expr.type() == typeid(int64_t)) {
    return to_string(get<int64_t>(expr));
  }
  if (expr.type() == typeid(double)) {
    return to_string(get<double>(expr));
  }
  if (expr.type() == typeid(SetLiteralEmpty)) {
    return toString(get<SetLiteralEmpty>(expr));
  }
  if (expr.type() == typeid(IntSetLiteralBounded)) {
    return toString(get<IntSetLiteralBounded>(expr));
  }
  if (expr.type() == typeid(IntSetLiteralSet)) {
    return toString(get<IntSetLiteralSet>(expr));
  }
  if (expr.type() == typeid(FloatSetLiteralBounded)) {
    return toString(get<FloatSetLiteralBounded>(expr));
  }
  if (expr.type() == typeid(FloatSetLiteralSet)) {
    return toString(get<FloatSetLiteralSet>(expr));
  }
  if (expr.type() == typeid(std::string)) {
    return get<std::string>(expr);
  }
  return "";
}
std::string toString(const ArrayLiteral& array) {
  return "[" + vecToString(array) + "]";
}
std::string toString(const Expr& expr) {
  if (expr.type() == typeid(bool)) {
    return to_string(get<bool>(expr));
  }
  if (expr.type() == typeid(int64_t)) {
    return to_string(get<int64_t>(expr));
  }
  if (expr.type() == typeid(double)) {
    return to_string(get<double>(expr));
  }
  if (expr.type() == typeid(SetLiteralEmpty)) {
    return toString(get<SetLiteralEmpty>(expr));
  }
  if (expr.type() == typeid(IntSetLiteralBounded)) {
    return toString(get<IntSetLiteralBounded>(expr));
  }
  if (expr.type() == typeid(IntSetLiteralSet)) {
    return toString(get<IntSetLiteralSet>(expr));
  }
  if (expr.type() == typeid(FloatSetLiteralBounded)) {
    return toString(get<FloatSetLiteralBounded>(expr));
  }
  if (expr.type() == typeid(FloatSetLiteralSet)) {
    return toString(get<FloatSetLiteralSet>(expr));
  }
  if (expr.type() == typeid(std::string)) {
    return get<std::string>(expr);
  }
  if (expr.type() == typeid(ArrayLiteral)) {
    return toString(get<ArrayLiteral>(expr));
  }
  return "";
}
std::string toString(const ParArrayLiteral& array) {
  return "[" + vecToString(array) + "]";
}
std::string toString(const ParExpr& expr) {
  if (expr.type() == typeid(bool)) {
    return to_string(get<bool>(expr));
  }
  if (expr.type() == typeid(int64_t)) {
    return to_string(get<int64_t>(expr));
  }
  if (expr.type() == typeid(double)) {
    return to_string(get<double>(expr));
  }
  if (expr.type() == typeid(SetLiteralEmpty)) {
    return toString(get<SetLiteralEmpty>(expr));
  }
  if (expr.type() == typeid(IntSetLiteralBounded)) {
    return toString(get<IntSetLiteralBounded>(expr));
  }
  if (expr.type() == typeid(IntSetLiteralSet)) {
    return toString(get<IntSetLiteralSet>(expr));
  }
  if (expr.type() == typeid(FloatSetLiteralBounded)) {
    return toString(get<FloatSetLiteralBounded>(expr));
  }
  if (expr.type() == typeid(FloatSetLiteralSet)) {
    return toString(get<FloatSetLiteralSet>(expr));
  }
  if (expr.type() == typeid(ParArrayLiteral)) {
    return toString(get<ParArrayLiteral>(expr));
  }
  return "";
}
std::string toString(const ParDeclItem&) { return ""; }
std::string toString(const BasicAnnExpr& expr) {  // NOLINT(*-no-recursion)
  if (expr.type() == typeid(bool)) {
    return to_string(get<bool>(expr));
  }
  if (expr.type() == typeid(int64_t)) {
    return to_string(get<int64_t>(expr));
  }
  if (expr.type() == typeid(double)) {
    return to_string(get<double>(expr));
  }
  if (expr.type() == typeid(SetLiteralEmpty)) {
    return toString(get<SetLiteralEmpty>(expr));
  }
  if (expr.type() == typeid(IntSetLiteralBounded)) {
    return toString(get<IntSetLiteralBounded>(expr));
  }
  if (expr.type() == typeid(IntSetLiteralSet)) {
    return toString(get<IntSetLiteralSet>(expr));
  }
  if (expr.type() == typeid(FloatSetLiteralBounded)) {
    return toString(get<FloatSetLiteralBounded>(expr));
  }
  if (expr.type() == typeid(FloatSetLiteralSet)) {
    return toString(get<FloatSetLiteralSet>(expr));
  }
  if (expr.type() == typeid(std::string)) {
    return get<std::string>(expr);
  }
  if (expr.type() == typeid(x3::forward_ast<Annotation>)) {
    return toString(get<x3::forward_ast<Annotation>>(expr).get());
  }
  return "";
}
std::string toString(const AnnExpr& annExpr) {  // NOLINT(*-no-recursion)
  return "[" + vecToString(annExpr) + "]";
}
std::string toString(const Annotation& annotation) {  // NOLINT(*-no-recursion)
  return annotation.identifier +
         (annotation.expressions.empty()
              ? ""
              : "(" + vecToString(annotation.expressions) + ")");
}
std::string toString(const Annotations& annotations) {
  return annotations.empty() ? "" : " :: " + vecToString(annotations);
}
std::string toString(const BasicVarDecl& var) {
  return toString(var.type) + ": " + var.identifier +
         toString(var.annotations) +
         (var.expr.has_value() ? " = " + toString(var.expr.value()) : "");
}

std::string toString(const ArrayVarDecl& array) {
  return toString(array.type) + ": " + array.identifier +
         toString(array.annotations) + " = " + toString(array.literals);
}
std::string toString(const VarDeclItem& var) {
  return var.type() == typeid(BasicVarDecl) ? toString(get<BasicVarDecl>(var))
                                            : toString(get<ArrayVarDecl>(var));
}
std::string toString(const ConstraintItem&) { return ""; }
std::string toString(const OptimizationType&) { return ""; }
std::string toString(const SolveSatisfy&) { return ""; }
std::string toString(const SolveOptimize&) { return ""; }
std::string toString(const SolveItem&) { return ""; }
std::string toString(const PredicateItem&) { return ""; }
std::string toString(const Model&) { return ""; }

}  // namespace fznparser::parser