#pragma once
#include <gtest/gtest.h>

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <string>
#include <vector>

#include "fznparser/parser/grammar_ast.hpp"

namespace fznparser::testing {

using namespace fznparser::parser;

using boost::get;

void expect_eq(const int64_t &, const int64_t &, const std::string &);

void expect_eq(const double &, const double &, const std::string &);

void expect_eq(const std::string &, const std::string &, const std::string &);

void expect_eq(const BasicParType &, const BasicParType &, const std::string &);

void expect_eq(const IndexSet &, const IndexSet &, const std::string &);

void expect_eq(const BasicParTypeArray &, const BasicParTypeArray &,
               const std::string &);

void expect_eq(const ParType &, const ParType &, const std::string &);

void expect_eq(const BoolVar &, const BoolVar &, const std::string &);

void expect_eq(const BasicVarBoolType &, const BasicVarBoolType &,
               const std::string &);

void expect_eq(const BasicVarIntTypeUnbounded &,
               const BasicVarIntTypeUnbounded &, const std::string &);

void expect_eq(const BasicVarIntTypeBounded &, const BasicVarIntTypeBounded &,
               const std::string &);

void expect_eq(const BasicVarIntTypeSet &, const BasicVarIntTypeSet &,
               const std::string &);

void expect_eq(const BasicVarFloatTypeUnbounded &,
               const BasicVarFloatTypeUnbounded &, const std::string &);

void expect_eq(const BasicVarFloatTypeBounded &,
               const BasicVarFloatTypeBounded &, const std::string &);

void expect_eq(const BasicVarSetTypeBounded &, const BasicVarSetTypeBounded &,
               const std::string &);

void expect_eq(const BasicVarSetTypeSet &, const BasicVarSetTypeSet &,
               const std::string &);

void expect_eq(const BasicVarType &, const BasicVarType &, const std::string &);

void expect_eq(const ArrayVarType &, const ArrayVarType &, const std::string &);

void expect_eq(const IndexSetUnbounded &, const IndexSetUnbounded &,
               const std::string &);

void expect_eq(const PredIndexSet &, const PredIndexSet &, const std::string &);

void expect_eq(const BasicPredParamTypeIntBounded &,
               const BasicPredParamTypeIntBounded &, const std::string &);

void expect_eq(const BasicPredParamTypeFloatBounded &,
               const BasicPredParamTypeFloatBounded &, const std::string &);

void expect_eq(const BasicPredParamTypeIntSet &,
               const BasicPredParamTypeIntSet &, const std::string &);

void expect_eq(const BasicPredParamTypeSetBounded &,
               const BasicPredParamTypeSetBounded &, const std::string &);

void expect_eq(const BasicPredParamTypeSetSet &,
               const BasicPredParamTypeSetSet &, const std::string &);

void expect_eq(const BasicPredParamType &, const BasicPredParamType &,
               const std::string &);

void expect_eq(const PredParamArrayType &, const PredParamArrayType &,
               const std::string &);

void expect_eq(const PredParamType &, const PredParamType &,
               const std::string &);

void expect_eq(const PredParam &, const PredParam &, const std::string &);

void expect_eq(const SetLiteralEmpty &, const SetLiteralEmpty &,
               const std::string &);

void expect_eq(const IntSetLiteralBounded &, const IntSetLiteralBounded &,
               const std::string &);

void expect_eq(const IntSetLiteralSet &, const IntSetLiteralSet &,
               const std::string &);

void expect_eq(const FloatSetLiteralBounded &, const FloatSetLiteralBounded &,
               const std::string &);

void expect_eq(const FloatSetLiteralSet &, const FloatSetLiteralSet &,
               const std::string &);

void expect_eq(const BasicLiteralExpr &, const BasicLiteralExpr &,
               const std::string &);

void expect_eq(const BasicExpr &, const BasicExpr &, const std::string &);

void expect_eq(const ArrayLiteral &, const ArrayLiteral &, const std::string &);

void expect_eq(const Expr &, const Expr &, const std::string &);

void expect_eq(const ParArrayLiteral &, const ParArrayLiteral &,
               const std::string &);

void expect_eq(const ParExpr &, const ParExpr &, const std::string &);

void expect_eq(const ParDeclItem &, const ParDeclItem &, const std::string &);

void expect_eq(const PredParamArrayType &, const PredParamArrayType &,
               const std::string &);

void expect_eq(const Annotation &, const Annotation &, const std::string &);

void expect_eq(const BasicAnnExpr &, const BasicAnnExpr &, const std::string &);

void expect_eq(const AnnExpr &, const AnnExpr &, const std::string &);

void expect_eq(const Annotations &, const Annotations &, const std::string &);

void expect_eq(const BasicVarDecl &, const BasicVarDecl &, const std::string &);

void expect_eq(const ArrayVarDecl &, const ArrayVarDecl &, const std::string &);

void expect_eq(const VarDeclItem &, const VarDeclItem &, const std::string &);

void expect_eq(const ConstraintItem &, const ConstraintItem &,
               const std::string &);

void expect_eq(const OptimizationType &, const OptimizationType &,
               const std::string &);

void expect_eq(const SolveSatisfy &, const SolveSatisfy &, const std::string &);

void expect_eq(const SolveOptimize &, const SolveOptimize &,
               const std::string &);

void expect_eq(const SolveItem &, const SolveItem &, const std::string &);

void expect_eq(const PredicateItem &, const PredicateItem &,
               const std::string &);

void expect_eq(const Model &, const Model &, const std::string &);

}  // namespace fznparser::testing