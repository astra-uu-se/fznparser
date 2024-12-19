#pragma once

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <optional>
#include <string>
#include <vector>

namespace fznparser::parser {
namespace x3 = boost::spirit::x3;

struct IntRange {
  int64_t lowerBound;
  int64_t upperBound;
  [[nodiscard]] int64_t size() const { return upperBound - lowerBound + 1; }
};

struct FloatRange {
  double lowerBound;
  double upperBound;
  [[nodiscard]] double size() const { return upperBound - lowerBound; }
};

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
  [[nodiscard]] int64_t size() const { return upperBound; }
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

// var bool
struct BasicVarBoolType {};

// "var" "int"
struct BasicVarIntTypeUnbounded {};

// "var" <int-literal> ".." <int-literal>
struct BasicVarIntTypeBounded : IntRange {};

// "var" "{" <int-literal> "," ... "}"
struct BasicVarIntTypeSet : std::vector<int64_t> {};

/*
| "var" "float"
*/
struct BasicVarFloatTypeUnbounded {};

/*
| "var" <float-literal> ".." <float-literal>
*/
struct BasicVarFloatTypeBounded : FloatRange {};

/*
| "{" <int-literal> "," ... "}"
| "set" "of" <int-literal> .. <int-literal>
| "set" "of" "{" [  <int-literal> "," ... ] "}"
*/
struct BasicVarSetTypeBounded : IntRange {};

struct BasicVarSetTypeSet : std::vector<int64_t> {};

struct BasicVarSetTypeUnbounded {};

using BasicVarType = boost::variant<
    BasicVarBoolType, BasicVarIntTypeUnbounded, BasicVarIntTypeBounded,
    BasicVarIntTypeSet, BasicVarFloatTypeBounded, BasicVarFloatTypeUnbounded,
    BasicVarSetTypeBounded, BasicVarSetTypeSet, BasicVarSetTypeUnbounded>;

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

struct BasicPredParamTypeIntBounded : IntRange {};

struct BasicPredParamTypeFloatBounded : FloatRange {};

struct BasicPredParamTypeIntSet : std::vector<int64_t> {};

struct BasicPredParamTypeSetBounded : IntRange {};

struct BasicPredParamTypeSetSet : std::vector<int64_t> {};
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
                   BasicVarSetTypeUnbounded, BasicPredParamTypeIntBounded,
                   BasicPredParamTypeFloatBounded, BasicPredParamTypeIntSet,
                   BasicPredParamTypeSetBounded, BasicPredParamTypeSetSet>;

// | "array" "[" <pred-index-set> "]" "of" <basic-pred-param-type>
struct PredParamArrayType {
  PredIndexSet indexSet;
  BasicPredParamType type;
};

/*
<pred-param-type> ::= <basic-pred-param-type>
                    | "array" "[" <pred-index-set> "]" "of"
                      <basic-pred-param-type>
*/
using PredParamType = boost::variant<
    BasicParType, BasicVarBoolType, BasicVarIntTypeUnbounded,
    BasicVarIntTypeBounded, BasicVarIntTypeSet, BasicVarFloatTypeBounded,
    BasicVarFloatTypeUnbounded, BasicVarSetTypeBounded, BasicVarSetTypeSet,
    BasicVarSetTypeUnbounded, BasicPredParamTypeIntBounded,
    BasicPredParamTypeFloatBounded, BasicPredParamTypeIntSet,
    BasicPredParamTypeSetBounded, BasicPredParamTypeSetSet, PredParamArrayType>;

// <pred-param-type> : <identifier>
struct PredParam {
  PredParamType type;
  std::string identifier;
};

struct SetLiteralEmpty {};
struct IntSetLiteralBounded : IntRange {};
struct IntSetLiteralSet : std::vector<int64_t> {};
struct FloatSetLiteralBounded : FloatRange {};
struct FloatSetLiteralSet : std::vector<double> {};

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
}  // namespace fznparser::parser

BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::BasicParType)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::IndexSet, upperBound)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::BasicParTypeArray, indexSet, type)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::BasicVarBoolType)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::BasicVarIntTypeUnbounded)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::BasicVarIntTypeBounded, lowerBound,
                          upperBound)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::BasicVarFloatTypeUnbounded)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::BasicVarFloatTypeBounded,
                          lowerBound, upperBound)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::BasicVarSetTypeBounded, lowerBound,
                          upperBound)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::BasicVarSetTypeUnbounded)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::ArrayVarType, indexSet, type)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::IndexSetUnbounded)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::BasicPredParamTypeIntBounded,
                          lowerBound, upperBound)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::BasicPredParamTypeFloatBounded,
                          lowerBound, upperBound)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::BasicPredParamTypeSetBounded,
                          lowerBound, upperBound)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::PredParamArrayType, indexSet, type)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::PredParam, type, identifier)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::SetLiteralEmpty)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::IntSetLiteralBounded, lowerBound,
                          upperBound)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::FloatSetLiteralBounded, lowerBound,
                          upperBound)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::ParDeclItem, type, identifier,
                          expr)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::Annotation, identifier,
                          expressions)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::BasicVarDecl, type, identifier,
                          annotations, expr)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::ArrayVarDecl, type, identifier,
                          annotations, literals)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::ConstraintItem, identifier,
                          expressions, annotations)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::SolveSatisfy, annotations)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::SolveOptimize, annotations, type,
                          expr)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::PredicateItem, identifier, params)
BOOST_FUSION_ADAPT_STRUCT(fznparser::parser::Model, predicateItems,
                          parDeclItems, varDeclItems, constraintItems,
                          solveItem)
