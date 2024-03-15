#pragma once

#include <string>

#include "grammarAst.hpp"

namespace fznparser::parser {

std::string toString(const IntRange&);
std::string toString(const FloatRange&);
std::string toString(BasicParType basicParType);
std::string toString(const IndexSet&);
std::string toString(const BasicParTypeArray&);
std::string toString(const ParType&);
std::string toString(const BasicVarBoolType&);
std::string toString(const BasicVarIntTypeUnbounded&);
std::string toString(const BasicVarIntTypeBounded&);
std::string toString(const BasicVarIntTypeSet&);
std::string toString(const BasicVarFloatTypeUnbounded&);
std::string toString(const BasicVarFloatTypeBounded&);
std::string toString(const BasicVarSetTypeBounded&);
std::string toString(const BasicVarSetTypeSet&);
std::string toString(const BasicVarType&);
std::string toString(const ArrayVarType&);
std::string toString(const IndexSetUnbounded&);
std::string toString(const PredIndexSet&);
std::string toString(const BasicPredParamTypeIntBounded&);
std::string toString(const BasicPredParamTypeFloatBounded&);
std::string toString(const BasicPredParamTypeIntSet&);
std::string toString(const BasicPredParamTypeSetBounded&);
std::string toString(const BasicPredParamTypeSetSet&);
std::string toString(const BasicPredParamType&);
std::string toString(const PredParamArrayType&);
std::string toString(const PredParamType&);
std::string toString(const PredParam&);
std::string toString(const SetLiteralEmpty&);
std::string toString(const IntSetLiteralBounded&);
std::string toString(const IntSetLiteralSet&);
std::string toString(const FloatSetLiteralBounded&);
std::string toString(const FloatSetLiteralSet&);
std::string toString(const BasicLiteralExpr&);
std::string toString(const BasicExpr&);
std::string toString(const ArrayLiteral&);
std::string toString(const Expr&);
std::string toString(const ParArrayLiteral&);
std::string toString(const ParExpr&);
std::string toString(const ParDeclItem&);
std::string toString(const BasicAnnExpr&);
std::string toString(const AnnExpr&);
std::string toString(const Annotation&);
std::string toString(const Annotations&);
std::string toString(const BasicVarDecl&);
std::string toString(const ArrayVarDecl&);
std::string toString(const VarDeclItem&);
std::string toString(const ConstraintItem&);
std::string toString(const OptimizationType&);
std::string toString(const SolveSatisfy&);
std::string toString(const SolveOptimize&);
std::string toString(const SolveItem&);
std::string toString(const PredicateItem&);
std::string toString(const Model&);

}  // namespace fznparser::parser