#pragma once

#include <boost/spirit/home/x3.hpp>
#include <string>

#include "fznparser/parser/grammar.hpp"
#include "fznparser/parser/grammarAst.hpp"

namespace fznparser::parser {

namespace x3 = ::boost::spirit::x3;
using x3::attr, x3::char_, x3::lit, x3::lexeme, x3::omit, x3::rule, x3::space;

typedef x3::uint_parser<int64_t, 8> octtype;
constexpr octtype oct = octtype();

typedef x3::uint_parser<int64_t, 16> hextype;
constexpr hextype hex = hextype();

const auto negative_int = [](const auto &ctx) {
  int64_t &attribute = x3::_attr(ctx);
  x3::traits::move_to(-attribute, x3::_val(ctx));
  x3::_pass(ctx) = attribute >= 0;
};

const auto double_is_finite = [](const auto &ctx) {
  double &attribute = x3::_attr(ctx);
  x3::traits::move_to(attribute, x3::_val(ctx));
  x3::_pass(ctx) = std::isfinite(attribute);
};

template <typename ValueType>
struct strict_real_policies : x3::strict_real_policies<ValueType> {
  static bool const allow_leading_dot = false;
  static bool const allow_trailing_dot = false;
};
// TODO: parse exponents
const x3::real_parser<double, strict_real_policies<double>> float_parser{};

const auto neg_hex = rule<struct neg_hex, int64_t>{
    "neg_hex"} = lit("-0x") >> (hex[negative_int]);

const auto neg_oct = rule<struct neg_ocr, int64_t>{
    "neg_oct"} = lit("-0o") >> (oct[negative_int]);

const auto int_literal = rule<struct int_literal, int64_t>{"int_literal"} =
    lexeme[(lit("0x") >> hex) | (lit("0o") >> oct) | neg_hex | neg_oct |
           x3::int64];

const auto float_literal = rule<struct float_literal, double>{"float_literal"} =
    float_parser[double_is_finite];

/*
<identifier> ::= [A-Za-z][A-Za-z0-9_]*
*/
const auto identifier = rule<struct identifier, std::string>{"identifier"} =
    lexeme[x3::alpha >> *(x3::alnum | x3::char_('_'))];

/*
<basic-par-type> ::= "bool"
                   | "int"
                   | "float"
                   | "set of int"
*/
const auto basic_par_type =
    rule<struct basic_par_type, BasicParType>{"basic_par_type"} =
        lexeme[basicParTypes_];

/*
                  | "var" "bool"
*/
const auto basic_var_bool_type =
    rule<struct basic_var_bool_type, BasicVarBoolType>{"basic_var_bool_type"} =
        lexeme[lit("var") >> omit[+space] >> lit("bool")];

/*
                  | "var" "int"
                  | "var" <int-literal> ".." <int-literal>
                  | "var" "{" <int-literal> "," ... "}"
*/
const auto basic_var_int_type_unbounded =
    rule<struct basic_var_int_type_unbounded, BasicVarIntTypeUnbounded>{
        "basic_var_int_type_unbounded"} =
        lexeme[lit("var") >> omit[+space] >> lit("int")] >>
        attr(BasicVarIntTypeUnbounded{});

const auto basic_var_int_type_bounded =
    rule<struct basic_var_int_type_bounded, BasicVarIntTypeBounded>{
        "basic_var_int_type_bounded"} = lexeme[lit("var") >> omit[+space]] >>
                                        int_literal >> ".." >> int_literal;

const auto basic_var_int_type_set =
    rule<struct basic_var_int_type_set, BasicVarIntTypeSet>{
        "basic_var_int_type_set"} = lit("var") >> lit('{') >>
                                    -(int_literal % ',') >> lit('}');

/*
                  | "var" "float"
                  | "var" <float-literal> ".." <float-literal>
*/
const auto basic_var_float_type_unbounded =
    rule<struct basic_var_float_type_unbounded, BasicVarFloatTypeUnbounded>{
        "basic_var_float_type_unbounded"} =
        lexeme[lit("var") >> omit[+space] >> lit("float")] >>
        attr(BasicVarFloatTypeUnbounded{});

const auto basic_var_float_type_bounded =
    rule<struct basic_var_float_type_bounded, BasicVarFloatTypeBounded>{
        "basic_var_float_type_bounded"} =
        lexeme[lit("var") >> omit[+space]] >> float_literal >> ".." >>
        float_literal;

/*
<basic-var-type> ::= "var" <basic-par-type>
                   | "var" <int-literal> ".." <int-literal>
                   | "var" "{" <int-literal> "," ... "}"
                   | "var" <float-literal> ".." <float-literal>
                   | "var" "set" "of" <int-literal> ".." <int-literal>
                   | "var" "set" "of" "{" [ <int-literal> "," ... ] "}"
*/
const auto basic_var_set_type_bounded =
    rule<struct basic_var_set_type_bounded, BasicVarSetTypeBounded>{
        "basic_var_set_type_bounded"} =
        lexeme[lit("var") >> omit[+space] >> lit("set") >> omit[+space] >>
               lit("of") >> omit[+space] >> -(lit("int") >> omit[+space])] >>
        int_literal >> lit("..") >> int_literal;

const auto basic_var_set_type_set =
    rule<struct basic_var_set_type_set, BasicVarSetTypeSet>{
        "basic_var_set_type_set"} =
        lexeme[lit("var") >> omit[+space] >> lit("set") >> omit[+space] >>
               lit("of")] >>
        lit('{') >> -(int_literal % ',') >> lit('}');

const auto basic_var_set_type_unbounded =
    rule<struct basic_var_set_type_unbounded, BasicVarSetTypeUnbounded>{
        "basic_var_set_type_unbounded"} =
        lexeme[lit("var") >> omit[+space] >> lit("set of int")];

/*
<basic-var-type> ::= "var" <basic-par-type>
                   | "var" <int-literal> ".." <int-literal>
                   | "var" "{" <int-literal> "," ... "}"
                   | "var" <float-literal> ".." <float-literal>
                   | "var" "set" "of" <int-literal> ".." <int-literal>
                   | "var" "set" "of" "{" [ <int-literal> "," ... ] "}"
*/
const auto basic_var_type =
    rule<struct basic_var_type, BasicVarType>{"basic_var_type"} =
        basic_var_bool_type | basic_var_int_type_unbounded |
        basic_var_int_type_bounded | basic_var_int_type_set |
        basic_var_float_type_unbounded | basic_var_float_type_bounded |
        basic_var_set_type_bounded | basic_var_set_type_set |
        basic_var_set_type_unbounded;

/*
<index-set> ::= "1" ".." <int-literal>
*/
const auto index_set = rule<struct index_set, IndexSet>{
    "index_set"} = lit('1') >> lit("..") >> int_literal;

/*
<array-var-type> ::= "array" "[" <index-set> "]" "of" <basic-var-type>
*/
const auto array_var_type =
    rule<struct array_var_type, ArrayVarType>{"array_var_type"} =
        lit("array") >> lit('[') >> index_set >>
        lit(']') >> lexeme[lit("of") >> omit[+space]] >>
        (basic_var_bool_type | basic_var_int_type_unbounded |
         basic_var_int_type_bounded | basic_var_int_type_set |
         basic_var_float_type_unbounded | basic_var_float_type_bounded |
         basic_var_set_type_bounded | basic_var_set_type_set |
         basic_var_set_type_unbounded);

/*
             | "array" "[" <index-set> "]" "of" <basic-par-type>
*/
const auto basic_par_type_array =
    rule<struct basic_par_type_array, BasicParTypeArray>{
        "basic_par_type_array"} =
        lit("array") >> lit('[') >> index_set >>
        lit(']') >> lexeme[lit("of") >> omit[+space]] >> basic_par_type;

/*
<par-type> ::= <basic-par-type>
             | "array" "[" <index-set> "]" "of" <basic-par-type>
*/
const auto par_type = rule<struct par_type, ParType>{"par_type"} =
    basic_par_type | basic_par_type_array;

/*
                          | <int-literal> ".." <int-literal>
*/
const auto basic_pred_param_type_int_bounded =
    rule<struct basic_pred_param_type_int_bounded,
         BasicPredParamTypeIntBounded>{
        "basic_pred_param_type_int_bounded"} = int_literal >>
                                               lit("..") >> int_literal;

/*
                          | <float-literal> ".." <float-literal>
*/
const auto basic_pred_param_type_float_bounded =
    rule<struct basic_pred_param_type_float_bounded,
         BasicPredParamTypeFloatBounded>{
        "basic_pred_param_type_float_bounded"} = float_literal >>
                                                 lit("..") >> float_literal;
/*
                          | "{" <int-literal> "," ... "}"
*/
const auto basic_pred_param_type_int_set =
    rule<struct bounded_int_set, BasicPredParamTypeIntSet>{
        "basic_pred_param_type_int_set"} = lit('{') >> -(int_literal % ',') >>
                                           lit('}');

/*
                          | "set" "of" <int-literal> .. <int-literal>
*/
const auto basic_pred_param_type_set_bounded =
    rule<struct basic_pred_param_type_set_bounded,
         BasicPredParamTypeSetBounded>{"basic_pred_param_type_set_bounded"} =
        lexeme[lit("set") >> omit[+space] >> lit("of") >> omit[+space]] >>
        int_literal >> lit("..") >> int_literal;
/*
                          | "set" "of" "{" [  <int-literal> "," ... ] "}"
*/

const auto basic_pred_param_type_set_set =
    rule<struct basic_pred_param_type_set_set, BasicPredParamTypeSetSet>{
        "basic_pred_param_type_set_set"} =
        lexeme[lit("set") >> omit[+space] >> lit("of")] >> lit('{') >>
        -(int_literal % ',') >> lit('}');

/*
<basic-pred-param-type> ::= <basic-par-type>
                          | <basic-var-type>
                          | <int-literal> ".." <int-literal>
                          | <float-literal> ".." <float-literal>
                          | "{" <int-literal> "," ... "}"
                          | "set" "of" <int-literal> .. <int-literal>
                          | "set" "of" "{" [  <int-literal> "," ... ] "}"
*/
const auto basic_pred_param_type =
    rule<struct basic_pred_param_type, BasicPredParamType>{
        "basic_pred_param_type"} =
        basic_par_type | basic_var_type | basic_pred_param_type_int_bounded |
        basic_pred_param_type_float_bounded | basic_pred_param_type_int_set |
        basic_pred_param_type_set_bounded | basic_pred_param_type_set_set;

/*
<pred-index-set> ::= <index-set>
                   | "int"
*/
const auto index_set_unbounded =
    rule<struct index_set_unbounded, IndexSetUnbounded>{
        "index_set_unbounded"} = lit("int") >> attr(IndexSetUnbounded{});

const auto pred_index_set =
    rule<struct pred_index_set, PredIndexSet>{"pred_index_set"} =
        index_set | index_set_unbounded;

/*
                    | "array" "[" <pred-index-set> "]" "of"
                      <basic-pred-param-type>
*/
const auto pred_param_array_type =
    rule<struct pred_param_array_type, PredParamArrayType>{
        "pred_param_array_type"} =
        (lit("array") >> lit('[') >> pred_index_set >> lit(']')) >>
        ((lexeme[lit("of") >> omit[+space]] >> basic_par_type) |
         (lexeme[lit("of") >> omit[+space]] >> basic_var_type) |
         (lexeme[lit("of") >> omit[+space]] >>
          basic_pred_param_type_int_bounded) |
         (lexeme[lit("of") >> omit[+space]] >>
          basic_pred_param_type_float_bounded) |
         (lit("of") >> basic_pred_param_type_int_set) |
         (lexeme[lit("of") >> omit[+space]] >>
          basic_pred_param_type_set_bounded) |
         (lexeme[lit("of") >> omit[+space]] >> basic_pred_param_type_set_set));

/*
<pred-param-type> ::= <basic-pred-param-type>
                    | "array" "[" <pred-index-set> "]" "of"
                      <basic-pred-param-type>
*/
const auto pred_param_type =
    rule<struct pred_param_type, PredParamType>{"pred_param_type"} =
        basic_pred_param_type | pred_param_array_type;

/*
<pred-param-type> : <identifier>
*/

const auto pred_param = rule<struct pred_param, PredParam>{
    "pred_param"} = pred_param_type >> lit(':') >> identifier;

/*
<predicate-item> ::= "predicate" <identifier> "(" [ <pred-param-type> :
                     <identifier> "," ... ] ")" ";"
*/

const auto predicate_item =
    rule<struct predicate_item, PredicateItem>{"predicate_item"} =
        lexeme[lit("predicate") >> omit[+space] >> identifier] >> lit('(') >>
        -(pred_param % ',') >> lit(')') >> lit(';');

const auto set_literal_empty = rule<struct set_literal_empty, SetLiteralEmpty>{
    "set_literal_empty"} = lit('{') >> lit('}');

/*
<set-literal> ::= "{" [ <int-literal> "," ... ] "}"
                | <int-literal> ".." <int-literal>
*/
const auto int_set_literal_set =
    rule<struct int_set_literal_set, IntSetLiteralSet>{
        "int_set_literal_set"} = lit('{') >> (int_literal % ',') >> lit('}');

const auto int_set_literal_bounded =
    rule<struct int_set_literal_bounded, IntSetLiteralBounded>{
        "int_set_literal_bounded"} = int_literal >> lit("..") >> int_literal;

/*
                | "{" [ <float-literal> "," ... ] "}"
                | <float-literal> ".." <float-literal>
*/

const auto float_set_literal_set =
    rule<struct float_set_literal_set, FloatSetLiteralSet>{
        "float_set_literal_set"} = lit('{') >> (float_literal % ',') >>
                                   lit('}');

const auto float_set_literal_bounded =
    rule<struct float_set_literal_bounded, FloatSetLiteralBounded>{
        "float_set_literal_bounded"} = float_literal >>
                                       lit("..") >> float_literal;

/*
<basic-literal-expr> ::= <bool-literal>
                       | <int-literal>
                       | <float-literal>
                       | <set-literal>
*/
const auto basic_literal_expr =
    rule<struct basic_literal_expr, BasicLiteralExpr>{"basic_literal_expr"} =
        x3::bool_ | set_literal_empty | float_set_literal_set |
        float_set_literal_bounded | int_set_literal_set |
        int_set_literal_bounded |
        lexeme[int_literal >> !&(lit('.') | lit('e') | lit('E'))] |
        lexeme[float_literal >> !&(lit('.') | lit('e') | lit('E'))];

/*
<var-par-identifier> ::= [A-Za-z_][A-Za-z0-9_]*
*/

const auto var_par_identifier =
    rule<struct var_par_identifier, std::string>{"var_par_identifier"} =
        lexeme[(x3::alpha | x3::char_('_')) >> *(x3::alnum | x3::char_('_'))];

/*
<basic-expr> ::= <basic-literal-expr>
               | <var-par-identifier>
*/
const auto basic_expr = rule<struct basic_expr, BasicExpr>{"basic_expr"} =
    x3::bool_ | set_literal_empty | float_set_literal_set |
    float_set_literal_bounded | int_set_literal_set | int_set_literal_bounded |
    lexeme[int_literal >> !(lit('.') | lit('e') | lit('E'))] |
    lexeme[float_literal >> !(lit('.') | lit('e') | lit('E'))] |
    var_par_identifier;

/*
<array-literal> ::= "[" [ <basic-expr> "," ... ] "]"
*/
const auto array_literal = rule<struct array_literal, ArrayLiteral>{
    "array_literal"} = lit('[') >> -(basic_expr % ',') >> lit(']');

/*
<expr>       ::= <basic-expr>
*/
const auto expr = rule<struct expr, Expr>{"expr"} = basic_expr | array_literal;

/*
<par-array-literal> ::= "[" [ <basic-literal-expr> "," ... ] "]"
*/
const auto par_array_literal = rule<struct par_array_literal, ParArrayLiteral>{
    "par_array_literal"} = lit('[') >> -(basic_literal_expr % ',') >> lit(']');

/*
<par-expr>   ::= <basic-literal-expr>
*/
const auto par_expr = rule<struct par_expr, ParExpr>{"par_expr"} =
    basic_literal_expr | par_array_literal;

/*
<par-decl-item> ::= <par-type> ":" <var-par-identifier> "=" <par-expr> ";"
*/
const auto par_decl_item =
    rule<struct par_decl_item, ParDeclItem>{
        "par_decl_item"} = par_type >> lit(':') >> var_par_identifier >>
                           lit('=') >> par_expr >> lit(';');

/*
<string-contents> ::= ([^"\n\] | \[^\n(])*
<string-literal> ::= """ <string-contents> """
*/
const auto string_literal =
    rule<struct string_literal, std::string>{"string_literal"} =
        lexeme[x3::char_('"') >>
               *((lit("\\\"") >> attr('"')) | ~x3::char_('"')) >>
               x3::char_('"')];

const annotation_type annotation{"annotation"};
const basic_ann_expr_type basic_ann_expr{"basic_ann_expr"};
const ann_expr_type ann_expr{"ann_expr"};

/*
<annotation> ::= <identifier>
              | <identifier> "(" <ann-expr> "," ... ")"
*/
const auto annotation_def = identifier >>
                            -(lit('(') >> (ann_expr % ',') >> lit(')'));

/*
<basic-ann-expr>   := <basic-literal-expr>
                    | <string-literal>
                    | <annotation>
*/
const auto basic_ann_expr_def =
    basic_literal_expr | string_literal | annotation;

/*
<ann-expr>   := <basic-ann-expr>
              | "[" [ <basic-ann-expr> "," ... ] "]"
*/
const auto ann_expr_def =
    (basic_ann_expr %
     "ABCDEFGHIJKLMNOPQRSTUVWXYZ")  // hack to make a single basic-ann-expr a
                                    // singleton array
    | lit('[') >> -(basic_ann_expr % ',') >> lit(']');

BOOST_SPIRIT_DEFINE(annotation, basic_ann_expr, ann_expr)

/*
<annotations> ::= [ "::" <annotation> ]*
*/
const auto annotations = rule<struct annotations, Annotations>{"annotations"} =
    *(lit("::") >> annotation);

/*
<var-decl-item> ::= <basic-var-type> ":" <var-par-identifier> <annotations>
                    [ "=" <basic-expr> ] ";"
                  | <array-var-type> ":" <var-par-identifier> <annotations>
                    "=" <array-literal> ";"
*/

const auto basic_var_decl =
    rule<struct basic_var_decl, BasicVarDecl>{
        "basic_var_decl"} = basic_var_type >> lit(':') >> var_par_identifier >>
                            -annotations >> -(lit('=') >> basic_expr) >>
                            lit(';');

const auto array_var_decl =
    rule<struct array_var_decl, ArrayVarDecl>{
        "array_var_decl"} = array_var_type >> lit(':') >> var_par_identifier >>
                            -annotations >> lit('=') >> array_literal >>
                            lit(';');

const auto var_decl_item =
    rule<struct var_decl_item, VarDeclItem>{"var_decl_item"} =
        basic_var_decl | array_var_decl;

/*
<constraint-item> ::= "constraint" <identifier> "(" [ <expr> "," ... ] ")"
<annotations> ";" */
const auto constraint_item =
    rule<struct constraint_item, ConstraintItem>{"constraint_item"} =
        lexeme[lit("constraint") >> omit[+space] >> identifier] >> lit('(') >>
        -(expr % ',') >> lit(')') >> annotations >> lit(';');

/*
<solve-item> ::= "solve" <annotations> "satisfy" ";"
               | "solve" <annotations> "minimize" <basic-expr> ";"
               | "solve" <annotations> "maximize" <basic-expr> ";"
               */
const auto solve_satisfy =
    rule<struct solve_satisfy, SolveSatisfy>{
        "solve_satisfy"} = lit("solve") >> annotations >> lit("satisfy") >>
                           lit(';');

const auto solve_optimize =
    rule<struct solve_optimize, SolveOptimize>{
        "solve_optimize"} = lit("solve") >> annotations >> optimizationType_ >>
                            basic_expr >> lit(';');

const auto solve_item = rule<struct solve_item, SolveItem>{"solve_item"} =
    solve_satisfy | solve_optimize;

/*
<model> ::=
  [ <predicate-item> ]*
  [ <par-decl-item> ]*
  [ <var-decl-item> ]*
  [ <constraint-item> ]*
  <solve-item>
*/

const auto model = rule<struct model, Model>{"model"} =
    *(predicate_item) >> *(par_decl_item) >> *(var_decl_item) >>
    *(constraint_item) >> solve_item;

}  // namespace fznparser::parser