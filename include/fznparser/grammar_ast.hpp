#pragma once

#include <algorithm>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <optional>
#include <string>
#include <vector>

namespace fznparser {
namespace x3 = ::boost::spirit::x3;

struct IntRange {
  int64_t lowerBound;
  int64_t upperBound;
};

struct FloatRange {
  double lowerBound;
  double upperBound;
};

/*
Forward Declaration
*/

// <identifier> ::= [A-Za-z][A-Za-z0-9_]*

/*
<basic-par-type> ::= "bool"
                   | "int"
                   | "float"
                   | "set of int"
*/
enum class BasicParType { BOOL, INT, FLOAT, SET_OF_INT };

// <index-set> ::= "1" ".." <int-literal>
struct IndexSet {
  int64_t upperBound;
};

// "array" "[" <index-set> "]" "of" <basic-par-type>
struct BasicParTypeArray {
  IndexSet indexSet;
  BasicParType type;
};

/*
<par-type> ::= <basic-par-type>
             | "array" "[" <index-set> "]" "of" <basic-par-type>
*/
using ParType = boost::variant<BasicParType, BasicParTypeArray>;

struct BoolVar {};

// var bool
struct BasicVarBoolType {};

struct BasicVarIntTypeUnbounded {};

/*
  "var" "int"
| "var" <int-literal> ".." <int-literal>
| "var" "{" <int-literal> "," ... "}"
*/
struct BasicVarIntTypeBounded : public IntRange {};

struct BasicVarIntTypeSet : public std::vector<int64_t> {};

/*
| "var" "float"
*/
struct BasicVarFloatTypeUnbounded {};

/*
| "var" <float-literal> ".." <float-literal>
*/
struct BasicVarFloatTypeBounded : public FloatRange {};

/*
| "{" <int-literal> "," ... "}"
| "set" "of" <int-literal> .. <int-literal>
| "set" "of" "{" [  <int-literal> "," ... ] "}"
*/
struct BasicVarSetTypeBounded : public IntRange {};

struct BasicVarSetTypeSet : public std::vector<int64_t> {};

using BasicVarType =
    boost::variant<BasicVarBoolType, BasicVarIntTypeUnbounded,
                   BasicVarIntTypeBounded, BasicVarIntTypeSet,
                   BasicVarFloatTypeBounded, BasicVarFloatTypeUnbounded,
                   BasicVarSetTypeBounded, BasicVarSetTypeSet>;

// <array-var-type> ::= "array" "[" <index-set> "]" "of" <basic-var-type>
struct ArrayVarType {
  IndexSet indexSet;
  BasicVarType type;
};

/*
<pred-index-set> ::= <index-set>
                   | "int"
*/
struct IndexSetUnbounded {};
using PredIndexSet = boost::variant<IndexSet, IndexSetUnbounded>;

struct BasicPredParamTypeIntBounded : public IntRange {};

struct BasicPredParamTypeFloatBounded : public FloatRange {};

struct BasicPredParamTypeIntSet : public std::vector<int64_t> {};

struct BasicPredParamTypeSetBounded : public IntRange {};

struct BasicPredParamTypeSetSet : public std::vector<int64_t> {};
/*
<basic-pred-param-type> ::= <basic-par-type>
                          | <basic-var-type>
                          | <int-literal> ".." <int-literal>
                          | <float-literal> ".." <float-literal>
                          | "{" <int-literal> "," ... "}"
                          | "set" "of" <int-literal> .. <int-literal>
                          | "set" "of" "{" [  <int-literal> "," ... ] "}"
*/
using BasicPredParamType =
    boost::variant<BasicParType, BasicVarBoolType, BasicVarIntTypeUnbounded,
                   BasicVarIntTypeBounded, BasicVarIntTypeSet,
                   BasicVarFloatTypeBounded, BasicVarFloatTypeUnbounded,
                   BasicVarSetTypeBounded, BasicVarSetTypeSet,
                   BasicPredParamTypeIntBounded, BasicPredParamTypeFloatBounded,
                   BasicPredParamTypeIntSet, BasicPredParamTypeSetBounded,
                   BasicPredParamTypeSetSet>;

// | "array" "[" <pred-index-set> "]" "of" <basic-pred-param-type>
struct PredParamArrayType {
  PredIndexSet predIndexSet;
  BasicPredParamType basicPredParamType;
};

/*
<pred-param-type> ::= <basic-pred-param-type>
                    | "array" "[" <pred-index-set> "]" "of"
                      <basic-pred-param-type>
*/
using PredParamType =
    boost::variant<BasicParType, BasicVarBoolType, BasicVarIntTypeUnbounded,
                   BasicVarIntTypeBounded, BasicVarIntTypeSet,
                   BasicVarFloatTypeBounded, BasicVarFloatTypeUnbounded,
                   BasicVarSetTypeBounded, BasicVarSetTypeSet,
                   BasicPredParamTypeIntBounded, BasicPredParamTypeFloatBounded,
                   BasicPredParamTypeIntSet, BasicPredParamTypeSetBounded,
                   BasicPredParamTypeSetSet, PredParamArrayType>;

// <pred-param-type> : <identifier>
struct PredParam {
  PredParamType type;
  std::string identifier;
};

struct SetLiteralEmpty {};
struct IntSetLiteralBounded : public IntRange {};
struct IntSetLiteralSet : public std::vector<int64_t> {};
struct FloatSetLiteralBounded : public FloatRange {};
struct FloatSetLiteralSet : public std::vector<double> {};

/*
<basic-literal-expr> ::= <bool-literal>
                     | <int-literal>
                     | <float-literal>
                     | <set-literal>
*/
using BasicLiteralExpr =
    boost::variant<bool, int64_t, double, SetLiteralEmpty, IntSetLiteralBounded,
                   IntSetLiteralSet, FloatSetLiteralBounded,
                   FloatSetLiteralSet>;

/*
<basic-expr> ::= <basic-literal-expr>
            | <var-par-identifier>
*/
using BasicExpr =
    boost::variant<bool, int64_t, double, SetLiteralEmpty, IntSetLiteralBounded,
                   IntSetLiteralSet, FloatSetLiteralBounded, FloatSetLiteralSet,
                   std::string>;

// <array-literal> ::= "[" [ <basic-expr> "," ... ] "]"
using ArrayLiteral = std::vector<BasicExpr>;

/*
<expr>       ::= <basic-expr>
               | <array-literal>
*/
using Expr =
    boost::variant<bool, int64_t, double, SetLiteralEmpty, IntSetLiteralBounded,
                   IntSetLiteralSet, FloatSetLiteralBounded, FloatSetLiteralSet,
                   std::string, ArrayLiteral>;

// <par-array-literal> ::= "[" [ <basic-literal-expr> "," ... ] "]"
using ParArrayLiteral = std::vector<BasicLiteralExpr>;

/*
<par-expr>   ::= <basic-literal-expr>
               | <par-array-literal>
*/
using ParExpr =
    boost::variant<bool, int64_t, double, SetLiteralEmpty, IntSetLiteralBounded,
                   IntSetLiteralSet, FloatSetLiteralBounded, FloatSetLiteralSet,
                   ParArrayLiteral>;
/*
<var-par-identifier> ::= [A-Za-z_][A-Za-z0-9_]*

% Boolean literals
<bool-literal> ::= "false"
                 | "true"

% Integer literals
<int-literal> ::= [-]?[0-9]+
                | [-]?0x[0-9A-Fa-f]+
                | [-]?0o[0-7]+

% Float literals
<float-literal> ::= [-]?[0-9]+.[0-9]+
                  | [-]?[0-9]+.[0-9]+[Ee][-+]?[0-9]+
                  | [-]?[0-9]+[Ee][-+]?[0-9]+
*/

// <par-decl-item> ::= <par-type> ":" <var-par-identifier> "=" <par-expr> ";"
struct ParDeclItem {
  ParType type;
  std::string identifier;
  ParExpr expr;
};

struct Annotation;  // Forward declaration
/*
<basic-ann-expr>   := <basic-literal-expr>
                    | <string-literal>
                    | <annotation>
*/
using BasicAnnExpr =
    boost::variant<bool, int64_t, double, SetLiteralEmpty, IntSetLiteralBounded,
                   IntSetLiteralSet, FloatSetLiteralBounded, FloatSetLiteralSet,
                   std::string, x3::forward_ast<Annotation>>;
/*
<ann-expr>   := <basic-ann-expr>
              | "[" [ <basic-ann-expr> "," ... ] "]"
*/
using AnnExpr = std::vector<BasicAnnExpr>;

/*
<annotation> ::= <identifier>
               | <identifier> "(" <ann-expr> "," ... ")"
*/
struct Annotation {
  std::string identifier;
  std::vector<AnnExpr> expressions;
};

/*
<annotations> ::= [ "::" <annotation> ]*
*/
using Annotations = std::vector<Annotation>;

// | <basic-var-type> ":" <var-par-identifier> <annotations> [ "=" <basic-expr>
// ] ";"
struct BasicVarDecl {
  BasicVarType type;
  std::string identifier;
  Annotations annotations;
  std::optional<BasicExpr> expr;
};

// | <array-var-type> ":" <var-par-identifier> <annotations> "=" <array-literal>
// ";"
struct ArrayVarDecl {
  ArrayVarType type;
  std::string identifier;
  Annotations annotations;
  ArrayLiteral literals;
};

/*
<var-decl-item> ::= <basic-var-type> ":" <var-par-identifier> <annotations>
                    [ "=" <basic-expr> ] ";"
                  | <array-var-type> ":" <var-par-identifier> <annotations>
                    "=" <array-literal> ";"
*/
using VarDeclItem = boost::variant<BasicVarDecl, ArrayVarDecl>;

/*
<constraint-item> ::= "constraint" <identifier> "(" [ <expr> "," ... ] ")"
<annotations> ";"
*/
struct ConstraintItem {
  std::string identifier;
  std::vector<Expr> expressions;
  std::vector<Annotation> annotations;
};

/*
| "satisfy"
| "minimize"
| "maximize"
*/
enum class OptimizationType : bool { MINIMIZE, MAXIMIZE };

struct SolveSatisfy {
  Annotations annotations;
};

struct SolveOptimize {
  Annotations annotations;
  OptimizationType type;
  BasicExpr expr;
};

using SolveItem = boost::variant<SolveSatisfy, SolveOptimize>;

/*
<predicate-item> ::= "predicate" <identifier> "(" [ <pred-param-type> :
<identifier> "," ... ] ")" ";"
*/
struct PredicateItem {
  std::string identifier;
  std::vector<PredParam> params;
};

/*
<model> ::=
  [ <predicate-item> ]*
  [ <par-decl-item> ]*
  [ <var-decl-item> ]*
  [ <constraint-item> ]*
  <solve-item>
*/
struct Model {
  std::vector<PredicateItem> predicateItems;
  std::vector<ParDeclItem> parDeclItems;
  std::vector<VarDeclItem> varDeclItems;
  std::vector<ConstraintItem> constraintItems;
  SolveItem solveItem;
};
}  // namespace fznparser

BOOST_FUSION_ADAPT_STRUCT(fznparser::BasicParType);
BOOST_FUSION_ADAPT_STRUCT(fznparser::IndexSet, upperBound);
BOOST_FUSION_ADAPT_STRUCT(fznparser::BasicParTypeArray, indexSet, type);
BOOST_FUSION_ADAPT_STRUCT(fznparser::BoolVar);
BOOST_FUSION_ADAPT_STRUCT(fznparser::BasicVarBoolType);
BOOST_FUSION_ADAPT_STRUCT(fznparser::BasicVarIntTypeUnbounded);
BOOST_FUSION_ADAPT_STRUCT(fznparser::BasicVarIntTypeBounded, lowerBound,
                          upperBound);
BOOST_FUSION_ADAPT_STRUCT(fznparser::BasicVarFloatTypeUnbounded);
BOOST_FUSION_ADAPT_STRUCT(fznparser::BasicVarFloatTypeBounded, lowerBound,
                          upperBound);
BOOST_FUSION_ADAPT_STRUCT(fznparser::BasicVarSetTypeBounded, lowerBound,
                          upperBound);
BOOST_FUSION_ADAPT_STRUCT(fznparser::ArrayVarType, indexSet, type);
BOOST_FUSION_ADAPT_STRUCT(fznparser::IndexSetUnbounded);
BOOST_FUSION_ADAPT_STRUCT(fznparser::BasicPredParamTypeIntBounded, lowerBound,
                          upperBound);
BOOST_FUSION_ADAPT_STRUCT(fznparser::BasicPredParamTypeFloatBounded, lowerBound,
                          upperBound);
BOOST_FUSION_ADAPT_STRUCT(fznparser::BasicPredParamTypeSetBounded, lowerBound,
                          upperBound);
BOOST_FUSION_ADAPT_STRUCT(fznparser::PredParamArrayType, predIndexSet,
                          basicPredParamType);
BOOST_FUSION_ADAPT_STRUCT(fznparser::PredParam, type, identifier);
BOOST_FUSION_ADAPT_STRUCT(fznparser::SetLiteralEmpty);
BOOST_FUSION_ADAPT_STRUCT(fznparser::IntSetLiteralBounded, lowerBound,
                          upperBound);
BOOST_FUSION_ADAPT_STRUCT(fznparser::FloatSetLiteralBounded, lowerBound,
                          upperBound);
BOOST_FUSION_ADAPT_STRUCT(fznparser::ParDeclItem, type, identifier, expr);
BOOST_FUSION_ADAPT_STRUCT(fznparser::Annotation, identifier, expressions);
BOOST_FUSION_ADAPT_STRUCT(fznparser::BasicVarDecl, type, identifier,
                          annotations, expr);
BOOST_FUSION_ADAPT_STRUCT(fznparser::ArrayVarDecl, type, identifier,
                          annotations, literals);
BOOST_FUSION_ADAPT_STRUCT(fznparser::ConstraintItem, identifier, expressions,
                          annotations);
BOOST_FUSION_ADAPT_STRUCT(fznparser::SolveSatisfy, annotations);
BOOST_FUSION_ADAPT_STRUCT(fznparser::SolveOptimize, annotations, type, expr);
BOOST_FUSION_ADAPT_STRUCT(fznparser::PredicateItem, identifier, params)
BOOST_FUSION_ADAPT_STRUCT(fznparser::Model, predicateItems, parDeclItems,
                          varDeclItems, constraintItems, solveItem);
