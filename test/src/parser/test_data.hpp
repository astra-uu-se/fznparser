#pragma once

#include <gtest/gtest.h>

#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "fznparser/modelFactory.hpp"
#include "fznparser/parser/grammar.hpp"
#include "fznparser/parser/grammar_def.hpp"

namespace fznparser::testing {

using namespace fznparser::parser;
using std::pair;
using std::vector;

vector<std::string> padding();

bool isalnum_us_minus(const char c);

std::string flatten(const vector<std::string> &str_v, const std::string &p);

bool contains(const std::string &str, const std::string needle);

vector<std::string> extend(vector<std::string> &str_vec,
                           const vector<std::string> &other);
vector<pair<vector<std::string>, int64_t>> int_literal_data_pos();
vector<pair<vector<std::string>, double>> float_literal_data_pos();
vector<pair<vector<std::string>, std::string>> identifier_data_pos();
vector<pair<vector<std::string>, BasicParType>> basic_par_type_data_pos();
vector<pair<vector<std::string>, bool>> bool_literal_data_pos();
vector<pair<vector<std::string>, FloatSetLiteralSet>>
float_set_literal_set_data_pos();
vector<pair<vector<std::string>, FloatSetLiteralBounded>>
float_set_literal_bounded_data_pos();
vector<pair<vector<std::string>, FloatSetLiteralBounded>>
float_set_literal_bounded_data_pos();
vector<pair<vector<std::string>, IntSetLiteralSet>>
int_set_literal_set_data_pos();
vector<pair<vector<std::string>, IntSetLiteralBounded>>
int_set_literal_bounded_data_pos();
vector<pair<vector<std::string>, std::string>> string_literal_data_pos();
vector<pair<vector<std::string>, IndexSet>> index_set_data_pos();
vector<pair<vector<std::string>, BasicParTypeArray>>
basic_par_type_array_data_pos();
vector<pair<vector<std::string>, ParType>> par_type_data_pos();
vector<pair<vector<std::string>, BasicVarBoolType>>
basic_var_bool_type_data_pos();
vector<pair<vector<std::string>, BasicVarIntTypeUnbounded>>
basic_var_int_type_unbounded_data_pos();
vector<pair<vector<std::string>, BasicVarIntTypeBounded>>
basic_var_int_type_bounded_data_pos();
vector<pair<vector<std::string>, BasicVarIntTypeSet>>
basic_var_int_type_set_data_pos();
vector<pair<vector<std::string>, BasicVarFloatTypeUnbounded>>
basic_var_float_type_unbounded_data_pos();
vector<pair<vector<std::string>, BasicVarFloatTypeBounded>>
basic_var_float_type_bounded_data_pos();
vector<pair<vector<std::string>, BasicVarSetTypeBounded>>
basic_var_set_type_bounded_data_pos();
vector<pair<vector<std::string>, BasicVarSetTypeSet>>
basic_var_set_type_set_data_pos();
vector<pair<vector<std::string>, BasicVarType>> basic_var_type_data_pos();
vector<pair<vector<std::string>, ArrayVarType>> array_var_type_data_pos();
vector<pair<vector<std::string>, IndexSetUnbounded>>
index_set_unbounded_data_pos();
vector<pair<vector<std::string>, PredIndexSet>> pred_index_set_data_pos();
vector<pair<vector<std::string>, BasicPredParamTypeIntBounded>>
basic_pred_param_type_int_bounded_data_pos();
vector<pair<vector<std::string>, BasicPredParamTypeFloatBounded>>
basic_pred_param_type_float_bounded_data_pos();
vector<pair<vector<std::string>, BasicPredParamTypeIntSet>>
basic_pred_param_type_int_set_data_pos();
vector<pair<vector<std::string>, BasicPredParamTypeSetBounded>>
basic_pred_param_type_set_bounded_data_pos();
vector<pair<vector<std::string>, BasicPredParamTypeSetSet>>
basic_pred_param_type_set_set_data_pos();
vector<pair<vector<std::string>, BasicPredParamType>>
basic_pred_param_type_data_pos();
vector<pair<vector<std::string>, PredParamType>> pred_param_type_data_pos();
vector<pair<vector<std::string>, PredParam>> pred_param_data_pos();
vector<pair<vector<std::string>, BasicLiteralExpr>>
basic_literal_expr_data_pos();
vector<pair<vector<std::string>, BasicExpr>> basic_expr_data_pos();
vector<pair<vector<std::string>, ArrayLiteral>> array_literal_data_pos();
vector<pair<vector<std::string>, Expr>> expr_data_pos();
vector<pair<vector<std::string>, ParArrayLiteral>> par_array_literal_data_pos();
vector<pair<vector<std::string>, ParExpr>> par_expr_data_pos();
vector<pair<vector<std::string>, ParDeclItem>> par_decl_item_data_pos();
vector<pair<vector<std::string>, PredParamArrayType>>
pred_param_array_type_data_pos();
vector<pair<vector<std::string>, BasicAnnExpr>> basic_ann_expr_data_pos(
    int64_t recursion = 1);
vector<pair<vector<std::string>, AnnExpr>> ann_expr_data_pos(
    int64_t recursion = 1);
vector<pair<vector<std::string>, Annotation>> annotation_data_pos(
    int64_t recursion = 1);
vector<pair<vector<std::string>, Annotations>> annotations_data_pos();
vector<pair<vector<std::string>, BasicVarDecl>> basic_var_decl_data_pos();
vector<pair<vector<std::string>, ArrayVarDecl>> array_var_decl_data_pos();
vector<pair<vector<std::string>, VarDeclItem>> var_decl_item_data_pos();
vector<pair<vector<std::string>, ConstraintItem>> constraint_item_data_pos();
vector<pair<vector<std::string>, SolveSatisfy>> solve_satisfy_data_pos();
vector<pair<vector<std::string>, SolveOptimize>> solve_optimize_data_pos();
vector<pair<vector<std::string>, SolveItem>> solve_item_data_pos();
vector<pair<vector<std::string>, PredicateItem>> predicate_item_data_pos();
vector<pair<vector<std::string>, Model>> model_data_pos();
vector<pair<vector<std::string>, std::string>> var_par_identifier_data_pos();

vector<std::string> int_literal_data_neg();
vector<std::string> float_literal_data_neg();
vector<std::string> identifier_data_neg();
vector<std::string> basic_par_type_data_neg();
vector<std::string> bool_literal_data_neg();
vector<std::string> float_set_literal_set_data_neg();
vector<std::string> set_literal_empty_data_neg();
vector<std::string> float_set_literal_bounded_data_neg();
vector<std::string> int_set_literal_set_data_neg();
vector<std::string> int_set_literal_bounded_data_neg();
vector<std::string> string_literal_data_neg();
vector<std::string> index_set_data_neg();
vector<std::string> basic_par_type_array_data_neg();
vector<std::string> par_type_data_neg();
vector<std::string> basic_var_bool_type_data_neg();
vector<std::string> basic_var_int_type_unbounded_data_neg();
vector<std::string> basic_var_int_type_bounded_data_neg();
vector<std::string> basic_var_int_type_set_data_neg();
vector<std::string> basic_var_float_type_unbounded_data_neg();
vector<std::string> basic_var_float_type_bounded_data_neg();
vector<std::string> basic_var_set_type_bounded_data_neg();
vector<std::string> basic_var_set_type_set_data_neg();
vector<std::string> basic_var_type_data_neg();
vector<std::string> array_var_type_data_neg();
vector<std::string> index_set_unbounded_data_neg();
vector<std::string> pred_index_set_data_neg();
vector<std::string> basic_pred_param_type_int_bounded_data_neg();
vector<std::string> basic_pred_param_type_float_bounded_data_neg();
vector<std::string> basic_pred_param_type_int_set_data_neg();
vector<std::string> basic_pred_param_type_set_bounded_data_neg();
vector<std::string> basic_pred_param_type_set_set_data_neg();
vector<std::string> basic_pred_param_type_data_neg();
vector<std::string> pred_param_array_data_neg();
vector<std::string> pred_param_type_data_neg();
vector<std::string> pred_param_data_neg();
vector<std::string> basic_literal_expr_data_neg();
vector<std::string> basic_expr_data_neg();
vector<std::string> array_literal_data_neg();
vector<std::string> expr_data_neg();
vector<std::string> par_array_literal_data_neg();
vector<std::string> par_expr_data_neg();
vector<std::string> par_decl_item_data_neg();
vector<std::string> pred_param_array_type_data_neg();
vector<std::string> basic_ann_expr_data_neg();
vector<std::string> ann_expr_data_neg();
vector<std::string> annotation_data_neg();
vector<std::string> annotations_data_neg();
vector<std::string> basic_var_decl_data_neg();
vector<std::string> array_var_decl_data_neg();
vector<std::string> var_decl_item_data_neg();
vector<std::string> constraint_item_data_neg();
vector<std::string> solve_satisfy_data_neg();
vector<std::string> solve_optimize_data_neg();
vector<std::string> solve_item_data_neg();
vector<std::string> predicate_item_data_neg();
vector<std::string> model_data_neg();
vector<std::string> var_par_identifier_data_neg();

template <class T>
void extend_should_fail(
    vector<std::string> bad_inputs,
    vector<pair<vector<std::string>, T>> &other_rule_inputs) {
  for (const auto &[input, _] : other_rule_inputs) {
    bad_inputs.emplace_back(input);
  }
}

#define extend_should_succeed(good_inputs, sub_rule_inputs, data_type) \
  do {                                                                 \
    size_t i = 0;                                                      \
    for (const auto &[input_str_vec, expected] : sub_rule_inputs) {    \
      good_inputs.emplace_back(pair<vector<std::string>, data_type>(   \
          input_str_vec, data_type{expected}));                        \
      if (++i > 2) {                                                   \
        break;                                                         \
      }                                                                \
    }                                                                  \
  } while (false)

#define populate_with_bounds(good_inputs, data, T, prefix)    \
  do {                                                        \
    for (size_t i = 0; i < data.size(); ++i) {                \
      const size_t j = (i + 1) % data.size();                 \
      vector<std::string> str_vec = prefix;                   \
      extend(str_vec, data.at(i).first);                      \
      str_vec.emplace_back("..");                             \
      extend(str_vec, data.at(j).first);                      \
      good_inputs.emplace_back(pair<vector<std::string>, T>{  \
          str_vec, T{data.at(i).second, data.at(j).second}}); \
    }                                                         \
  } while (false);
#define populate_with_sets(good_inputs, data, NumberType, T, prefix)       \
  do {                                                                     \
    for (size_t i = 0; i < data.size(); ++i) {                             \
      const size_t j = (i + 1) % data.size();                              \
      vector<std::string> str_vec = prefix;                                \
      extend(str_vec, data.at(i).first);                                   \
      str_vec.emplace_back("..");                                          \
      extend(str_vec, data.at(j).first);                                   \
      good_inputs.emplace_back(pair<vector<std::string>, T>{               \
          str_vec,                                                         \
          T{vector<NumberType>{}, data.at(i).second, data.at(j).second}}); \
    }                                                                      \
    for (size_t i : vector<size_t>{0, 1, data.size()}) {                   \
      vector<NumberType> vals{};                                           \
      vector<std::string> str_vec = prefix;                                \
      str_vec.emplace_back("{");                                           \
      for (size_t j = 0; j < i; ++j) {                                     \
        if (j != 0) {                                                      \
          str_vec.emplace_back(",");                                       \
        }                                                                  \
        extend(str_vec, data.at(j).first);                                 \
        vals.emplace_back(data.at(j).second);                              \
      }                                                                    \
      str_vec.emplace_back("}");                                           \
      good_inputs.emplace_back(                                            \
          pair<vector<std::string>, T>{str_vec, T{vals, 0, 0}});           \
    }                                                                      \
  } while (false);

}  // namespace fznparser::testing