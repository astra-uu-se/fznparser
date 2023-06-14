#pragma once

#include <gtest/gtest.h>

#include <algorithm>
#include <boost/spirit/home/x3.hpp>
#include <filesystem>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "./expect_eq.hpp"
#include "./test_data.hpp"
#include "fznparser/modelFactory.hpp"
#include "fznparser/parser/grammar.hpp"
#include "fznparser/parser/grammar_def.hpp"

namespace fznparser::testing {

using namespace fznparser::parser;

namespace x3 = boost::spirit::x3;

#define create_rule_test(T, rule)                                     \
  do {                                                                \
    for (const std::pair<std::vector<std::string>, T> data :          \
         rule##_data_pos()) {                                         \
      for (const std::string &p : padding()) {                        \
        const std::string input = flatten(data.first, p);             \
        T actual;                                                     \
        auto iter = input.begin();                                    \
        EXPECT_TRUE(x3::phrase_parse(iter, input.end(), rule,         \
                                     x3::standard::space, actual))    \
            << ("\"" + input + "\"");                                 \
        EXPECT_TRUE(iter == input.end()) << ("\"" + input + "\"");    \
        expect_eq(actual, data.second, input);                        \
        if (data.first.size() == 1) {                                 \
          break;                                                      \
        }                                                             \
      }                                                               \
    }                                                                 \
    for (const auto &input : rule##_data_neg()) {                     \
      EXPECT_FALSE(x3::phrase_parse(input.begin(), input.end(), rule, \
                                    x3::standard::space))             \
          << ("\"" + input + "\"");                                   \
    }                                                                 \
  } while (false);

TEST(int_literal_test, test_data) { create_rule_test(int64_t, int_literal); }

TEST(float_literal_test, test_data) { create_rule_test(double, float_literal); }
TEST(identifier_test, test_data) { create_rule_test(std::string, identifier); }
TEST(VarParIdentifier_test, test_data) {
  create_rule_test(std::string, var_par_identifier);
}
TEST(basic_par_type_test, test_data) {
  create_rule_test(BasicParType, basic_par_type);
}
TEST(float_set_literal_set_test, test_data) {
  create_rule_test(FloatSetLiteralSet, float_set_literal_set);
}
TEST(float_set_literal_bounded_test, test_data) {
  create_rule_test(FloatSetLiteralBounded, float_set_literal_bounded);
}
TEST(int_set_literal_set_test, test_data) {
  create_rule_test(IntSetLiteralSet, int_set_literal_set);
}
TEST(int_set_literal_bounded_test, test_data) {
  create_rule_test(IntSetLiteralBounded, int_set_literal_bounded);
}
TEST(string_literal_test, test_data) {
  create_rule_test(std::string, string_literal);
}
TEST(index_set_test, test_data) { create_rule_test(IndexSet, index_set); }
TEST(basic_par_type_array_test, test_data) {
  create_rule_test(BasicParTypeArray, basic_par_type_array);
}
TEST(par_type_test, test_data) { create_rule_test(ParType, par_type); }
TEST(basic_var_bool_type_test, test_data) {
  create_rule_test(BasicVarBoolType, basic_var_bool_type);
}
TEST(basic_var_int_type_unbounded_test, test_data) {
  create_rule_test(BasicVarIntTypeUnbounded, basic_var_int_type_unbounded);
}
TEST(basic_var_int_type_bounded_test, test_data) {
  create_rule_test(BasicVarIntTypeBounded, basic_var_int_type_bounded);
}
TEST(basic_var_int_type_set_test, test_data) {
  create_rule_test(BasicVarIntTypeSet, basic_var_int_type_set);
}
TEST(basic_var_float_type_unbounded_test, test_data) {
  create_rule_test(BasicVarFloatTypeUnbounded, basic_var_float_type_unbounded);
}
TEST(basic_var_float_type_bounded_test, test_data) {
  create_rule_test(BasicVarFloatTypeBounded, basic_var_float_type_bounded);
}
TEST(basic_var_set_type_bounded_test, test_data) {
  create_rule_test(BasicVarSetTypeBounded, basic_var_set_type_bounded);
}
TEST(basic_var_set_type_set_test, test_data) {
  create_rule_test(BasicVarSetTypeSet, basic_var_set_type_set);
}
TEST(basic_var_type_test, test_data) {
  create_rule_test(BasicVarType, basic_var_type);
}
TEST(array_var_type_test, test_data) {
  create_rule_test(ArrayVarType, array_var_type);
}
TEST(index_set_unbounded_test, test_data) {
  create_rule_test(IndexSetUnbounded, index_set_unbounded);
}
TEST(pred_index_set_test, test_data) {
  create_rule_test(PredIndexSet, pred_index_set);
}
TEST(basic_pred_param_type_int_bounded_test, test_data) {
  create_rule_test(BasicPredParamTypeIntBounded,
                   basic_pred_param_type_int_bounded);
}
TEST(basic_pred_param_type_float_bounded_test, test_data) {
  create_rule_test(BasicPredParamTypeFloatBounded,
                   basic_pred_param_type_float_bounded);
}
TEST(basic_pred_param_type_int_set_test, test_data) {
  create_rule_test(BasicPredParamTypeIntSet, basic_pred_param_type_int_set);
}
TEST(basic_pred_param_type_set_bounded_test, test_data) {
  create_rule_test(BasicPredParamTypeSetBounded,
                   basic_pred_param_type_set_bounded);
}
TEST(basic_pred_param_type_set_set_test, test_data) {
  create_rule_test(BasicPredParamTypeSetSet, basic_pred_param_type_set_set);
}
TEST(basic_pred_param_type_test, test_data) {
  create_rule_test(BasicPredParamType, basic_pred_param_type);
}
TEST(pred_param_type_test, test_data) {
  create_rule_test(PredParamType, pred_param_type);
}
TEST(pred_param_test, test_data) { create_rule_test(PredParam, pred_param); }
TEST(basic_literal_expr_test, test_data) {
  create_rule_test(BasicLiteralExpr, basic_literal_expr);
}
TEST(basic_expr_test, test_data) { create_rule_test(BasicExpr, basic_expr); }
TEST(array_literal_test, test_data) {
  create_rule_test(ArrayLiteral, array_literal);
}
TEST(expr_test, test_data) { create_rule_test(Expr, expr); }
TEST(par_array_literal_test, test_data) {
  create_rule_test(ParArrayLiteral, par_array_literal);
}
TEST(par_expr_test, test_data) { create_rule_test(ParExpr, par_expr); }
TEST(par_decl_item_test, test_data) {
  create_rule_test(ParDeclItem, par_decl_item);
}
TEST(pred_param_array_type_test, test_data) {
  create_rule_test(PredParamArrayType, pred_param_array_type);
}
TEST(basic_ann_expr_test, test_data) {
  create_rule_test(BasicAnnExpr, basic_ann_expr);
}
TEST(ann_expr_test, test_data) { create_rule_test(AnnExpr, ann_expr); }
TEST(annotation_test, test_data) { create_rule_test(Annotation, annotation); }
TEST(annotations_test, test_data) {
  create_rule_test(Annotations, annotations);
}
TEST(basic_var_decl_test, test_data) {
  create_rule_test(BasicVarDecl, basic_var_decl);
}
TEST(array_var_decl_test, test_data) {
  create_rule_test(ArrayVarDecl, array_var_decl);
}
TEST(var_decl_item_test, test_data) {
  create_rule_test(VarDeclItem, var_decl_item);
}
TEST(constraint_item_test, test_data) {
  create_rule_test(ConstraintItem, constraint_item);
}
TEST(solve_satisfy_test, test_data) {
  create_rule_test(SolveSatisfy, solve_satisfy);
}
TEST(solve_optimize_test, test_data) {
  create_rule_test(SolveOptimize, solve_optimize);
}
TEST(solve_item_test, test_data) { create_rule_test(SolveItem, solve_item); }
TEST(predicate_item_test, test_data) {
  create_rule_test(PredicateItem, predicate_item);
}
TEST(model_test, test_data) { create_rule_test(Model, model); }

TEST(par_decl_item, manual) {
  std::string input = "array [1..2] of int: coeffs = [1, -1];";

  ParDeclItem actual;
  auto iter = input.begin();
  EXPECT_TRUE(x3::phrase_parse(iter, input.end(), par_decl_item,
                               x3::standard::space, actual))
      << ("\"" + input + "\"");
  EXPECT_TRUE(iter == input.end()) << ("\"" + input + "\"");
  expect_eq(actual,
            ParDeclItem{BasicParTypeArray{IndexSet{2}, BasicParType::INT},
                        std::string{"coeffs"},
                        ParExpr{ParArrayLiteral{int64_t{1}, int64_t{-1}}}},
            input);
}

TEST(basic_var_decl, manual) {
  std::string input = "var int: a;";

  BasicVarDecl actual;
  auto iter = input.begin();
  EXPECT_TRUE(x3::phrase_parse(iter, input.end(), basic_var_decl,
                               x3::standard::space, actual))
      << ("\"" + input + "\"");
  EXPECT_TRUE(iter == input.end()) << ("\"" + input + "\"");
  expect_eq(actual,
            BasicVarDecl{BasicVarIntTypeUnbounded{}, std::string{"a"},
                         Annotations{}, std::optional<BasicExpr>{}},
            input);
}

TEST(model, manual) {
  std::string input = "var int: a;\nsolve satisfy;";

  Model actual;
  auto iter = input.begin();
  EXPECT_TRUE(
      x3::phrase_parse(iter, input.end(), model, x3::standard::space, actual))
      << ("\"" + input + "\"");
  EXPECT_TRUE(iter == input.end()) << ("\"" + input + "\"");
  expect_eq(actual,
            Model{std::vector<PredicateItem>{}, std::vector<ParDeclItem>{},
                  std::vector<VarDeclItem>{
                      BasicVarDecl{BasicVarIntTypeUnbounded{}, std::string{"a"},
                                   Annotations{}, std::optional<BasicExpr>{}}},
                  std::vector<ConstraintItem>{},
                  SolveItem{SolveSatisfy{Annotations{}}}},
            input);
}

}  // namespace fznparser::testing
