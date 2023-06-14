#include "test_data.hpp"

namespace fznparser::testing {

using namespace fznparser::parser;
using std::pair;
using std::vector;

vector<std::string> padding() {
  return vector<std::string>{"\t ", "\t", "  ", " ", ""};
}

bool isalnum_us_minus(const char c) {
  return std::isalnum(c) || c == '_' || c == '-';
}

std::string flatten(const vector<std::string> &str_vec, const std::string &p) {
  if (str_vec.empty()) {
    return "";
  }
  std::string str = str_vec.front();
  for (size_t i = 1; i < str_vec.size(); ++i) {
    if (str.empty() || str_vec.at(i).empty()) {
      continue;
    }
    if (!p.empty() || !isalnum_us_minus(str.back()) ||
        !isalnum_us_minus(str_vec.at(i).front())) {
      str += p + str_vec.at(i);
    } else {
      str += " " + str_vec.at(i);
    }
  }
  return str;
}

bool contains(const std::string &str, const std::string &needle) {
  return str.find(needle) != std::string::npos;
}

vector<std::string> extend(vector<std::string> &str_vec,
                           const vector<std::string> &other) {
  str_vec.reserve(str_vec.size() + other.size());
  str_vec.insert(str_vec.end(), other.begin(), other.end());
  return str_vec;
}

template <typename T>
vector<std::string> combine_str_vec(
    const vector<pair<vector<std::string>, T>> &data,
    const vector<std::string> &prefix, const vector<std::string> &separator,
    const vector<std::string> &suffix) {
  vector<std::string> str_vec(prefix);
  for (size_t i = 0; i < data.size(); ++i) {
    if (i != 0) {
      extend(str_vec, separator);
    }
    extend(str_vec, data.at(i).first);
  }
  extend(str_vec, suffix);
  return str_vec;
}

template <typename T>
vector<std::string> combine_str_vec(
    const vector<pair<vector<std::string>, T>> &data, const std::string &prefix,
    const std::string &separator, const std::string &suffix) {
  return combine_str_vec<T>(data,
                            prefix.length() == 0 ? vector<std::string>{}
                                                 : vector<std::string>{prefix},
                            separator.length() == 0
                                ? vector<std::string>{}
                                : vector<std::string>{separator},
                            suffix.length() == 0 ? vector<std::string>{}
                                                 : vector<std::string>{suffix});
}

template <typename T>
vector<std::string> front_str_vec(
    const vector<pair<vector<std::string>, T>> &data,
    const vector<std::string> &prefix, const vector<std::string> &suffix) {
  vector<std::string> str_vec(prefix);
  extend(str_vec, data.front().first);
  extend(str_vec, suffix);
  return str_vec;
}

template <typename T>
vector<std::string> front_str_vec(
    const vector<pair<vector<std::string>, T>> &data, const std::string &prefix,
    const std::string &suffix) {
  return front_str_vec<T>(data,
                          prefix.length() == 0 ? vector<std::string>{}
                                               : vector<std::string>{prefix},
                          suffix.length() == 0 ? vector<std::string>{}
                                               : vector<std::string>{suffix});
}

template <typename T>
vector<T> combine_data(const vector<pair<vector<std::string>, T>> &data) {
  vector<T> data_vec{};
  for (const auto &[_, d] : data) {
    data_vec.emplace_back(d);
  }
  return data_vec;
}

template <typename T>
T front_data(const vector<pair<vector<std::string>, T>> &data) {
  return data.front().second;
}

template <typename T>
vector<pair<vector<std::string>, T>> int_bounded(
    const vector<std::string> &prefix, const vector<std::string> &separator,
    const vector<std::string> &suffix) {
  vector<pair<vector<std::string>, T>> good_inputs{};
  const auto &ild = int_literal_data_pos();
  vector<pair<vector<std::string>, int64_t>> elems{ild.front(), ild.back()};

  for (const auto &[lb_str_vec, lb] : elems) {
    for (const auto &[ub_str_vec, ub] : elems) {
      vector<std::string> str_vec(prefix);
      extend(str_vec, lb_str_vec);
      extend(str_vec, separator);
      extend(str_vec, ub_str_vec);
      extend(str_vec, suffix);
      good_inputs.emplace_back(
          pair<vector<std::string>, T>{str_vec, T{lb, ub}});
    }
  }
  return good_inputs;
}

template <typename T>
vector<pair<vector<std::string>, T>> float_bounded(
    const vector<std::string> &prefix, const vector<std::string> &separator,
    const vector<std::string> &suffix) {
  vector<pair<vector<std::string>, T>> good_inputs{};
  const auto &fld = float_literal_data_pos();
  vector<pair<vector<std::string>, double>> elems{fld.front(), fld.back()};

  for (const auto &[lb_str_vec, lb] : elems) {
    for (const auto &[ub_str_vec, ub] : elems) {
      vector<std::string> str_vec(prefix);
      extend(str_vec, lb_str_vec);
      extend(str_vec, separator);
      extend(str_vec, ub_str_vec);
      extend(str_vec, suffix);
      good_inputs.emplace_back(
          pair<vector<std::string>, T>{str_vec, T{lb, ub}});
    }
  }
  return good_inputs;
}

template <typename T>
vector<pair<vector<std::string>, T>> int_set(
    const vector<std::string> &prefix, const vector<std::string> &separator,
    const vector<std::string> &suffix, bool includeEmpty = true) {
  const auto &ild = int_literal_data_pos();
  vector<pair<vector<std::string>, T>> good_inputs{};
  if (includeEmpty) {
    vector<std::string> empty(prefix);
    extend(empty, suffix);
    good_inputs.emplace_back(pair<vector<std::string>, T>{empty, T{}});
  }
  good_inputs.emplace_back(pair<vector<std::string>, T>{
      front_str_vec<int64_t>(ild, prefix, suffix),
      T(std::vector<int64_t>{ild.front().second})});
  good_inputs.emplace_back(pair<vector<std::string>, T>{
      combine_str_vec<int64_t>(ild, prefix, separator, suffix),
      T(combine_data<int64_t>(ild))});
  return good_inputs;
}

template <typename T>
vector<pair<vector<std::string>, T>> float_set(
    const vector<std::string> &prefix, const vector<std::string> &separator,
    const vector<std::string> &suffix, bool includeEmpty = true) {
  const auto &fld = float_literal_data_pos();
  vector<pair<vector<std::string>, T>> good_inputs{};
  if (includeEmpty) {
    vector<std::string> empty(prefix);
    extend(empty, suffix);
    good_inputs.emplace_back(pair<vector<std::string>, T>{empty, T{}});
  }
  good_inputs.emplace_back(
      pair<vector<std::string>, T>{front_str_vec<double>(fld, prefix, suffix),
                                   T(std::vector<double>{fld.front().second})});
  good_inputs.emplace_back(pair<vector<std::string>, T>{
      combine_str_vec<double>(fld, prefix, separator, suffix),
      T(combine_data<double>(fld))});
  return good_inputs;
}

vector<pair<vector<std::string>, int64_t>> int_literal_data_pos() {
  return vector<pair<vector<std::string>, int64_t>>{
      {{"0"}, 0},         {{"1234"}, 1234},
      {{"-1234"}, -1234}, {{"0xDEADBEEF"}, 3735928559},
      {{"0o67"}, 55},     {{"-0xDEADBEEF"}, -3735928559},
      {{"-0o67"}, -55}};
}

vector<pair<vector<std::string>, double>> float_literal_data_pos() {
  return vector<pair<vector<std::string>, double>>{
      {{"123.456"}, 123.456}, {{"-123.456"}, -123.456}, {{"0.0"}, 0.0}};
}

vector<pair<vector<std::string>, std::string>> string_literal_data_pos() {
  return vector<pair<vector<std::string>, std::string>>{
      {{"\"abcd\""}, "abcd"},
      {{"\"e f g h\""}, "e f g h"},
      {{"\"\\\"ijk\\\"\""}, "\"ijk\""},
      {{"\"l\tm\tn\to\""}, "l\tm\tn\to"}};
}

vector<pair<vector<std::string>, std::string>> identifier_data_pos() {
  return vector<pair<vector<std::string>, std::string>>{
      {{"x"}, "x"}, {{"a0sd_"}, "a0sd_"}, {{"c_b_l_s_0"}, "c_b_l_s_0"}};
}

vector<pair<vector<std::string>, BasicParType>> basic_par_type_data_pos() {
  return vector<pair<vector<std::string>, BasicParType>>{
      {{"bool"}, BasicParType::BOOL},
      {{"int"}, BasicParType::INT},
      {{"float"}, BasicParType::FLOAT},
      // rule requires only one space
      {{"set of int"}, BasicParType::SET_OF_INT}};
}

vector<pair<vector<std::string>, bool>> bool_literal_data_pos() {
  return vector<pair<vector<std::string>, bool>>{{{"false"}, false},
                                                 {{"true"}, true}};
}

vector<pair<vector<std::string>, FloatSetLiteralSet>>
float_set_literal_set_data_pos() {
  return float_set<FloatSetLiteralSet>(vector<std::string>{"{"},
                                       vector<std::string>{","},
                                       vector<std::string>{"}"}, false);
};

vector<pair<vector<std::string>, FloatSetLiteralBounded>>
float_set_literal_bounded_data_pos() {
  return float_bounded<FloatSetLiteralBounded>(
      vector<std::string>{}, vector<std::string>{".."}, vector<std::string>{});
};

vector<pair<vector<std::string>, SetLiteralEmpty>>
set_literal_empty_data_pos() {
  return vector<pair<vector<std::string>, SetLiteralEmpty>>{{{"{", "}"}, {}}};
};

vector<pair<vector<std::string>, IntSetLiteralSet>>
int_set_literal_set_data_pos() {
  return int_set<IntSetLiteralSet>(vector<std::string>{"{"},
                                   vector<std::string>{","},
                                   vector<std::string>{"}"}, false);
};

vector<pair<vector<std::string>, IntSetLiteralBounded>>
int_set_literal_bounded_data_pos() {
  return int_bounded<IntSetLiteralBounded>(
      vector<std::string>{}, vector<std::string>{".."}, vector<std::string>{});
};

vector<pair<vector<std::string>, IndexSet>> index_set_data_pos() {
  vector<pair<vector<std::string>, IndexSet>> good_inputs{};
  for (const auto &[il_str_vec, il] : int_literal_data_pos()) {
    vector<std::string> str_vec{"1", ".."};
    extend(str_vec, il_str_vec);
    good_inputs.emplace_back(
        pair<vector<std::string>, IndexSet>{str_vec, IndexSet{il}});
  }
  return good_inputs;
}

vector<pair<vector<std::string>, BasicParTypeArray>>
basic_par_type_array_data_pos() {
  vector<pair<vector<std::string>, BasicParTypeArray>> good_inputs{};
  for (const auto &[is_str_vec, is] : index_set_data_pos()) {
    for (const auto &[bpt_str_vec, bpt] : basic_par_type_data_pos()) {
      vector<std::string> str_vec{"array", "["};
      extend(str_vec, is_str_vec);
      str_vec.emplace_back("]");
      str_vec.emplace_back("of");
      extend(str_vec, bpt_str_vec);
      good_inputs.emplace_back(pair<vector<std::string>, BasicParTypeArray>{
          str_vec, BasicParTypeArray{is, bpt}});
    }
  }

  return good_inputs;
}

vector<pair<vector<std::string>, ParType>> par_type_data_pos() {
  vector<pair<vector<std::string>, ParType>> good_inputs{};
  extend_should_succeed(good_inputs, basic_par_type_data_pos(), ParType);
  extend_should_succeed(good_inputs, basic_par_type_array_data_pos(), ParType);
  return good_inputs;
}

vector<pair<vector<std::string>, BasicVarBoolType>>
basic_var_bool_type_data_pos() {
  return vector<pair<vector<std::string>, BasicVarBoolType>>{
      {{"var", "bool"}, BasicVarBoolType{}}};
}

vector<pair<vector<std::string>, BasicVarIntTypeUnbounded>>
basic_var_int_type_unbounded_data_pos() {
  return vector<pair<vector<std::string>, BasicVarIntTypeUnbounded>>{
      {{"var", "int"}, BasicVarIntTypeUnbounded{}}};
}

vector<pair<vector<std::string>, BasicVarIntTypeBounded>>
basic_var_int_type_bounded_data_pos() {
  return int_bounded<BasicVarIntTypeBounded>(vector<std::string>{"var"},
                                             vector<std::string>{".."},
                                             vector<std::string>{});
}

vector<pair<vector<std::string>, BasicVarIntTypeSet>>
basic_var_int_type_set_data_pos() {
  return int_set<BasicVarIntTypeSet>(vector<std::string>{"var", "{"},
                                     vector<std::string>{","},
                                     vector<std::string>{"}"});
};

vector<pair<vector<std::string>, BasicVarFloatTypeUnbounded>>
basic_var_float_type_unbounded_data_pos() {
  return vector<pair<vector<std::string>, BasicVarFloatTypeUnbounded>>{
      {{"var", "float"}, {}}};
}

vector<pair<vector<std::string>, BasicVarFloatTypeBounded>>
basic_var_float_type_bounded_data_pos() {
  return float_bounded<BasicVarFloatTypeBounded>(vector<std::string>{"var"},
                                                 vector<std::string>{".."},
                                                 vector<std::string>{});
}

vector<pair<vector<std::string>, BasicVarSetTypeBounded>>
basic_var_set_type_bounded_data_pos() {
  return int_bounded<BasicVarSetTypeBounded>(
      vector<std::string>{"var", "set", "of", "int"}, vector<std::string>{".."},
      vector<std::string>{});
}

vector<pair<vector<std::string>, BasicVarSetTypeSet>>
basic_var_set_type_set_data_pos() {
  return int_set<BasicVarSetTypeSet>(
      vector<std::string>{"var", "set", "of", "{"}, vector<std::string>{","},
      vector<std::string>{"}"});
};

vector<pair<vector<std::string>, BasicVarType>> basic_var_type_data_pos() {
  vector<pair<vector<std::string>, BasicVarType>> good_inputs{};
  extend_should_succeed(good_inputs, basic_var_bool_type_data_pos(),
                        BasicVarType);
  extend_should_succeed(good_inputs, basic_var_int_type_unbounded_data_pos(),
                        BasicVarType);
  extend_should_succeed(good_inputs, basic_var_int_type_bounded_data_pos(),
                        BasicVarType);
  extend_should_succeed(good_inputs, basic_var_int_type_set_data_pos(),
                        BasicVarType);
  extend_should_succeed(good_inputs, basic_var_float_type_unbounded_data_pos(),
                        BasicVarType);
  extend_should_succeed(good_inputs, basic_var_float_type_bounded_data_pos(),
                        BasicVarType);
  extend_should_succeed(good_inputs, basic_var_set_type_bounded_data_pos(),
                        BasicVarType);
  extend_should_succeed(good_inputs, basic_var_set_type_set_data_pos(),
                        BasicVarType);
  return good_inputs;
}

vector<pair<vector<std::string>, ArrayVarType>> array_var_type_data_pos() {
  vector<pair<vector<std::string>, ArrayVarType>> good_inputs{};
  for (const auto &[index_str_vec, index_set] : index_set_data_pos()) {
    for (const auto &[var_str_vec, basic_var_type] :
         basic_var_type_data_pos()) {
      vector<std::string> str_vec{"array", "["};
      extend(str_vec, index_str_vec);
      str_vec.emplace_back("]");
      str_vec.emplace_back("of");
      extend(str_vec, var_str_vec);
      good_inputs.emplace_back(pair<vector<std::string>, ArrayVarType>{
          str_vec, ArrayVarType{index_set, basic_var_type}});
    }
  }
  return good_inputs;
}

vector<pair<vector<std::string>, IndexSetUnbounded>>
index_set_unbounded_data_pos() {
  return vector<pair<vector<std::string>, IndexSetUnbounded>>{{{"int"}, {}}};
};

vector<pair<vector<std::string>, PredIndexSet>> pred_index_set_data_pos() {
  vector<pair<vector<std::string>, PredIndexSet>> good_inputs{};
  extend_should_succeed(good_inputs, index_set_data_pos(), PredIndexSet);
  extend_should_succeed(good_inputs, index_set_unbounded_data_pos(),
                        PredIndexSet);
  return good_inputs;
}

vector<pair<vector<std::string>, BasicPredParamTypeIntBounded>>
basic_pred_param_type_int_bounded_data_pos() {
  return int_bounded<BasicPredParamTypeIntBounded>(
      vector<std::string>{}, vector<std::string>{".."}, vector<std::string>{});
}

vector<pair<vector<std::string>, BasicPredParamTypeFloatBounded>>
basic_pred_param_type_float_bounded_data_pos() {
  return float_bounded<BasicPredParamTypeFloatBounded>(
      vector<std::string>{}, vector<std::string>{".."}, vector<std::string>{});
}

vector<pair<vector<std::string>, BasicPredParamTypeIntSet>>
basic_pred_param_type_int_set_data_pos() {
  return int_set<BasicPredParamTypeIntSet>(vector<std::string>{"{"},
                                           vector<std::string>{","},
                                           vector<std::string>{"}"});
}

vector<pair<vector<std::string>, BasicPredParamTypeSetBounded>>
basic_pred_param_type_set_bounded_data_pos() {
  return int_bounded<BasicPredParamTypeSetBounded>(
      vector<std::string>{"set", "of"}, vector<std::string>{".."},
      vector<std::string>{});
}

vector<pair<vector<std::string>, BasicPredParamTypeSetSet>>
basic_pred_param_type_set_set_data_pos() {
  return int_set<BasicPredParamTypeSetSet>(
      vector<std::string>{"set", "of", "{"}, vector<std::string>{","},
      vector<std::string>{"}"});
}

vector<pair<vector<std::string>, BasicPredParamType>>
basic_pred_param_type_data_pos() {
  vector<pair<vector<std::string>, BasicPredParamType>> good_inputs{};
  extend_should_succeed(good_inputs, basic_par_type_data_pos(),
                        BasicPredParamType);
  extend_should_succeed(good_inputs, basic_var_bool_type_data_pos(),
                        BasicPredParamType);
  extend_should_succeed(good_inputs, basic_var_int_type_unbounded_data_pos(),
                        BasicPredParamType);
  extend_should_succeed(good_inputs, basic_var_int_type_bounded_data_pos(),
                        BasicPredParamType);
  extend_should_succeed(good_inputs, basic_var_int_type_set_data_pos(),
                        BasicPredParamType);
  extend_should_succeed(good_inputs, basic_var_float_type_bounded_data_pos(),
                        BasicPredParamType);
  extend_should_succeed(good_inputs, basic_var_float_type_unbounded_data_pos(),
                        BasicPredParamType);
  extend_should_succeed(good_inputs, basic_var_set_type_bounded_data_pos(),
                        BasicPredParamType);
  extend_should_succeed(good_inputs, basic_var_set_type_set_data_pos(),
                        BasicPredParamType);
  extend_should_succeed(good_inputs,
                        basic_pred_param_type_int_bounded_data_pos(),
                        BasicPredParamType);
  extend_should_succeed(good_inputs,
                        basic_pred_param_type_float_bounded_data_pos(),
                        BasicPredParamType);
  extend_should_succeed(good_inputs, basic_pred_param_type_int_set_data_pos(),
                        BasicPredParamType);
  extend_should_succeed(good_inputs,
                        basic_pred_param_type_set_bounded_data_pos(),
                        BasicPredParamType);
  extend_should_succeed(good_inputs, basic_pred_param_type_set_set_data_pos(),
                        BasicPredParamType);
  return good_inputs;
}

vector<pair<vector<std::string>, PredParamArrayType>>
pred_param_array_data_pos() {
  vector<pair<vector<std::string>, PredParamArrayType>> good_inputs{};
  for (const auto &[pis_str_vec, pis] : pred_index_set_data_pos()) {
    for (const auto &[bppt_str_vec, bppt] : basic_pred_param_type_data_pos()) {
      vector<std::string> str_vec{"array", "["};
      extend(str_vec, pis_str_vec);
      str_vec.emplace_back("]");
      str_vec.emplace_back("of");
      extend(str_vec, bppt_str_vec);
      good_inputs.emplace_back(pair<vector<std::string>, PredParamArrayType>{
          str_vec, PredParamArrayType{pis, bppt}});
    }
  }
  return good_inputs;
}

vector<pair<vector<std::string>, PredParamType>> pred_param_type_data_pos() {
  vector<pair<vector<std::string>, PredParamType>> good_inputs{};
  extend_should_succeed(good_inputs, basic_par_type_data_pos(), PredParamType);
  extend_should_succeed(good_inputs, basic_var_bool_type_data_pos(),
                        PredParamType);
  extend_should_succeed(good_inputs, basic_var_int_type_unbounded_data_pos(),
                        PredParamType);
  extend_should_succeed(good_inputs, basic_var_int_type_bounded_data_pos(),
                        PredParamType);
  extend_should_succeed(good_inputs, basic_var_int_type_set_data_pos(),
                        PredParamType);
  extend_should_succeed(good_inputs, basic_var_float_type_bounded_data_pos(),
                        PredParamType);
  extend_should_succeed(good_inputs, basic_var_float_type_unbounded_data_pos(),
                        PredParamType);
  extend_should_succeed(good_inputs, basic_var_set_type_bounded_data_pos(),
                        PredParamType);
  extend_should_succeed(good_inputs, basic_var_set_type_set_data_pos(),
                        PredParamType);
  extend_should_succeed(
      good_inputs, basic_pred_param_type_int_bounded_data_pos(), PredParamType);
  extend_should_succeed(good_inputs,
                        basic_pred_param_type_float_bounded_data_pos(),
                        PredParamType);
  extend_should_succeed(good_inputs, basic_pred_param_type_int_set_data_pos(),
                        PredParamType);
  extend_should_succeed(
      good_inputs, basic_pred_param_type_set_bounded_data_pos(), PredParamType);
  extend_should_succeed(good_inputs, basic_pred_param_type_set_set_data_pos(),
                        PredParamType);
  extend_should_succeed(good_inputs, pred_param_array_data_pos(),
                        PredParamType);
  return good_inputs;
}

vector<pair<vector<std::string>, PredParam>> pred_param_data_pos() {
  vector<pair<vector<std::string>, PredParam>> good_inputs{};
  const auto id = identifier_data_pos();
  const auto &[i_str_vec, i] = id.front();
  for (const auto &[ppt_str_vec, ppt] : pred_param_type_data_pos()) {
    vector<std::string> str_vec(ppt_str_vec);
    str_vec.emplace_back(":");
    extend(str_vec, i_str_vec);
    good_inputs.emplace_back(
        pair<vector<std::string>, PredParam>{str_vec, PredParam{ppt, i}});
  }
  return good_inputs;
}

vector<pair<vector<std::string>, BasicLiteralExpr>>
basic_literal_expr_data_pos() {
  vector<pair<vector<std::string>, BasicLiteralExpr>> good_inputs{};
  extend_should_succeed(good_inputs, bool_literal_data_pos(), BasicLiteralExpr);
  extend_should_succeed(good_inputs, int_literal_data_pos(), BasicLiteralExpr);
  extend_should_succeed(good_inputs, float_literal_data_pos(),
                        BasicLiteralExpr);
  extend_should_succeed(good_inputs, int_set_literal_bounded_data_pos(),
                        BasicLiteralExpr);
  extend_should_succeed(good_inputs, int_set_literal_set_data_pos(),
                        BasicLiteralExpr);
  extend_should_succeed(good_inputs, float_set_literal_bounded_data_pos(),
                        BasicLiteralExpr);
  extend_should_succeed(good_inputs, float_set_literal_set_data_pos(),
                        BasicLiteralExpr);
  return good_inputs;
}

vector<pair<vector<std::string>, BasicExpr>> basic_expr_data_pos() {
  vector<pair<vector<std::string>, BasicExpr>> good_inputs{};
  extend_should_succeed(good_inputs, bool_literal_data_pos(), BasicExpr);
  extend_should_succeed(good_inputs, int_literal_data_pos(), BasicExpr);
  extend_should_succeed(good_inputs, float_literal_data_pos(), BasicExpr);
  extend_should_succeed(good_inputs, int_set_literal_bounded_data_pos(),
                        BasicExpr);
  extend_should_succeed(good_inputs, int_set_literal_set_data_pos(), BasicExpr);
  extend_should_succeed(good_inputs, float_set_literal_bounded_data_pos(),
                        BasicExpr);
  extend_should_succeed(good_inputs, float_set_literal_set_data_pos(),
                        BasicExpr);
  extend_should_succeed(good_inputs, var_par_identifier_data_pos(), BasicExpr);
  return good_inputs;
}

vector<pair<vector<std::string>, ArrayLiteral>> array_literal_data_pos() {
  const auto bed = basic_expr_data_pos();
  return vector<pair<vector<std::string>, ArrayLiteral>>{
      {vector<std::string>{"[", "]"}, ArrayLiteral{}},
      {front_str_vec<BasicExpr>(bed, "[", "]"),
       ArrayLiteral(std::vector<BasicExpr>{bed.front().second})},
      {combine_str_vec<BasicExpr>(bed, "[", ",", "]"),
       ArrayLiteral(combine_data<BasicExpr>(bed))}};
}

vector<pair<vector<std::string>, Expr>> expr_data_pos() {
  vector<pair<vector<std::string>, Expr>> good_inputs{};
  extend_should_succeed(good_inputs, bool_literal_data_pos(), Expr);
  extend_should_succeed(good_inputs, int_literal_data_pos(), Expr);
  extend_should_succeed(good_inputs, float_literal_data_pos(), Expr);
  extend_should_succeed(good_inputs, int_set_literal_bounded_data_pos(), Expr);
  extend_should_succeed(good_inputs, int_set_literal_set_data_pos(), Expr);
  extend_should_succeed(good_inputs, float_set_literal_bounded_data_pos(),
                        Expr);
  extend_should_succeed(good_inputs, float_set_literal_set_data_pos(), Expr);
  extend_should_succeed(good_inputs, var_par_identifier_data_pos(), Expr);
  extend_should_succeed(good_inputs, array_literal_data_pos(), Expr);
  return good_inputs;
}

vector<pair<vector<std::string>, ParArrayLiteral>>
par_array_literal_data_pos() {
  const auto bled = basic_literal_expr_data_pos();
  return vector<pair<vector<std::string>, ParArrayLiteral>>{
      {vector<std::string>{"[", "]"}, ParArrayLiteral{}},
      {front_str_vec<BasicLiteralExpr>(bled, "[", "]"),
       ParArrayLiteral(std::vector<BasicLiteralExpr>{bled.front().second})},
      {combine_str_vec<BasicLiteralExpr>(bled, "[", ",", "]"),
       ParArrayLiteral(combine_data<BasicLiteralExpr>(bled))}};
}

vector<pair<vector<std::string>, ParExpr>> par_expr_data_pos() {
  vector<pair<vector<std::string>, ParExpr>> good_inputs{};
  extend_should_succeed(good_inputs, bool_literal_data_pos(), ParExpr);
  extend_should_succeed(good_inputs, int_literal_data_pos(), ParExpr);
  extend_should_succeed(good_inputs, float_literal_data_pos(), ParExpr);
  extend_should_succeed(good_inputs, int_set_literal_bounded_data_pos(),
                        ParExpr);
  extend_should_succeed(good_inputs, int_set_literal_set_data_pos(), ParExpr);
  extend_should_succeed(good_inputs, float_set_literal_bounded_data_pos(),
                        ParExpr);
  extend_should_succeed(good_inputs, float_set_literal_set_data_pos(), ParExpr);
  extend_should_succeed(good_inputs, par_array_literal_data_pos(), ParExpr);
  return good_inputs;
}

vector<pair<vector<std::string>, ParDeclItem>> par_decl_item_data_pos() {
  vector<pair<vector<std::string>, ParDeclItem>> good_inputs{};
  for (const auto &[pt_str_vec, pt] : par_type_data_pos()) {
    for (const auto &[vpi_str_vec, vpi] : var_par_identifier_data_pos()) {
      for (const auto &[pe_str_vec, pe] : par_expr_data_pos()) {
        vector<std::string> str_vec(pt_str_vec);
        str_vec.emplace_back(":");
        extend(str_vec, vpi_str_vec);
        str_vec.emplace_back("=");
        extend(str_vec, pe_str_vec);
        str_vec.emplace_back(";");
        good_inputs.emplace_back(pair<vector<std::string>, ParDeclItem>{
            str_vec, ParDeclItem{pt, vpi, pe}});
      }
    }
  }
  return good_inputs;
}

vector<pair<vector<std::string>, PredParamArrayType>>
pred_param_array_type_data_pos() {
  vector<pair<vector<std::string>, PredParamArrayType>> good_inputs{};
  for (const auto &[is_str_vec, is] : index_set_data_pos()) {
    for (const auto &[bppt_str_vec, bppt] : basic_pred_param_type_data_pos()) {
      vector<std::string> str_vec{"array", "["};
      extend(str_vec, is_str_vec);
      str_vec.emplace_back("]");
      str_vec.emplace_back("of");
      extend(str_vec, bppt_str_vec);
      good_inputs.emplace_back(pair<vector<std::string>, PredParamArrayType>{
          str_vec, PredParamArrayType{is, bppt}});
    }
  }
  return good_inputs;
}

vector<pair<vector<std::string>, PredicateItem>> predicate_item_data_pos() {
  vector<pair<vector<std::string>, PredicateItem>> good_inputs{};
  const auto id = identifier_data_pos();
  const auto pptd = pred_param_type_data_pos();
  for (const auto &[identifier_str_vec, identifier] : id) {
    const size_t len = 1 + (id.size() > pptd.size() ? pptd.size() : id.size());
    for (size_t i = 0; i < len; ++i) {
      vector<std::string> str_vec{"predicate"};
      extend(str_vec, identifier_str_vec);
      str_vec.emplace_back("(");
      vector<PredParam> params;
      for (size_t j = 0; j < i; ++j) {
        if (j != 0) {
          str_vec.emplace_back(",");
        }
        extend(str_vec, pptd.at(j).first);
        str_vec.emplace_back(":");
        extend(str_vec, id.at(j).first);
        params.emplace_back(PredParam{pptd.at(j).second, id.at(j).second});
      }
      str_vec.emplace_back(")");
      str_vec.emplace_back(";");
      good_inputs.emplace_back(pair<vector<std::string>, PredicateItem>{
          str_vec, PredicateItem{identifier, params}});
    }
  }

  return good_inputs;
}

vector<pair<vector<std::string>, BasicAnnExpr>> basic_ann_expr_data_pos(
    int64_t recursion) {
  vector<pair<vector<std::string>, BasicAnnExpr>> good_inputs{};
  if (recursion < 0) {
    return good_inputs;
  }
  extend_should_succeed(good_inputs, bool_literal_data_pos(), BasicAnnExpr);
  extend_should_succeed(good_inputs, int_literal_data_pos(), BasicAnnExpr);
  extend_should_succeed(good_inputs, float_literal_data_pos(), BasicAnnExpr);
  extend_should_succeed(good_inputs, int_set_literal_bounded_data_pos(),
                        BasicAnnExpr);
  extend_should_succeed(good_inputs, int_set_literal_set_data_pos(),
                        BasicAnnExpr);
  extend_should_succeed(good_inputs, float_set_literal_bounded_data_pos(),
                        BasicAnnExpr);
  extend_should_succeed(good_inputs, float_set_literal_set_data_pos(),
                        BasicAnnExpr);
  extend_should_succeed(good_inputs, string_literal_data_pos(), BasicAnnExpr);
  extend_should_succeed(good_inputs, annotation_data_pos(recursion - 1),
                        BasicAnnExpr);
  return good_inputs;
}

vector<pair<vector<std::string>, AnnExpr>> ann_expr_data_pos(
    int64_t recursion) {
  vector<pair<vector<std::string>, AnnExpr>> good_inputs{};
  if (recursion < 0) {
    return good_inputs;
  }
  const auto baed = basic_ann_expr_data_pos(recursion - 1);
  if (baed.empty()) {
    return good_inputs;
  }
  for (const size_t i : vector<size_t>{0, 1, baed.size()}) {
    vector<std::string> str_vec{"["};
    vector<BasicAnnExpr> vals{};
    for (size_t j = 0; j < i; ++j) {
      if (j > 0) {
        str_vec.emplace_back(",");
      }
      extend(str_vec, baed.at(j).first);
      vals.emplace_back(baed.at(j).second);
    }
    str_vec.emplace_back("]");
    good_inputs.emplace_back(
        pair<vector<std::string>, AnnExpr>{str_vec, AnnExpr{vals}});
  }
  for (const size_t i : vector<size_t>{0, 1, baed.size() - 1}) {
    good_inputs.emplace_back(pair<vector<std::string>, AnnExpr>{
        baed.at(i).first, AnnExpr{baed.at(i).second}});
  }
  return good_inputs;
}

vector<pair<vector<std::string>, Annotation>> annotation_data_pos(
    int64_t recursion) {
  vector<pair<vector<std::string>, Annotation>> good_inputs{};
  if (recursion < 0) {
    return good_inputs;
  }
  const auto id = identifier_data_pos();
  for (const size_t i : vector<size_t>{1, id.size() - 1}) {
    good_inputs.emplace_back(pair<vector<std::string>, Annotation>{
        id.at(i).first, Annotation{id.at(i).second, vector<AnnExpr>{}}});
  }
  const auto ae = ann_expr_data_pos(recursion - 1);
  if (ae.empty()) {
    return good_inputs;
  }
  for (size_t i : vector<size_t>{1, ae.size()}) {
    vector<std::string> str_vec(id.front().first);
    str_vec.emplace_back("(");
    vector<AnnExpr> expressions{};
    for (size_t j = 0; j < i; ++j) {
      if (j != 0) {
        str_vec.emplace_back(",");
      }
      extend(str_vec, ae.at(j).first);
      expressions.emplace_back(ae.at(j).second);
    }
    str_vec.emplace_back(")");

    good_inputs.emplace_back(pair<vector<std::string>, Annotation>{
        str_vec, Annotation{id.front().second, expressions}});
  }

  return good_inputs;
}

vector<pair<vector<std::string>, Annotations>> annotations_data_pos() {
  vector<pair<vector<std::string>, Annotations>> good_inputs{};
  good_inputs.emplace_back(pair<vector<std::string>, Annotations>{
      vector<std::string>{""}, Annotations{}});

  const auto ad = annotation_data_pos(1);

  for (size_t i : vector<size_t>{0, 1, ad.size()}) {
    vector<std::string> str_vec{};
    Annotations annotations{};
    for (size_t j = 0; j < i; ++j) {
      str_vec.emplace_back("::");
      extend(str_vec, ad.at(j).first);
      annotations.emplace_back(ad.at(j).second);
    }

    good_inputs.emplace_back(
        pair<vector<std::string>, Annotations>{str_vec, annotations});
  }

  return good_inputs;
}

vector<pair<vector<std::string>, std::string>> var_par_identifier_data_pos() {
  vector<pair<vector<std::string>, std::string>> good_inputs =
      identifier_data_pos();
  good_inputs.emplace_back(
      pair<vector<std::string>, std::string>{{"_cbls"}, "_cbls"});
  good_inputs.emplace_back(
      pair<vector<std::string>, std::string>{{"_0_cbls"}, "_0_cbls"});
  return good_inputs;
}

vector<pair<vector<std::string>, BasicVarDecl>> basic_var_decl_data_pos() {
  vector<pair<vector<std::string>, BasicVarDecl>> good_inputs{};
  const auto bvtd = basic_var_type_data_pos();
  const auto vpid = var_par_identifier_data_pos();
  const auto ad = annotations_data_pos();
  const auto bed = basic_expr_data_pos();

  for (const auto &[bvt_str_vec, bvt] : bvtd) {
    for (const auto &[vpi_str_vec, vpi] :
         vector<pair<vector<std::string>, std::string>>{vpid.front(),
                                                        vpid.back()}) {
      for (const auto &[a_str_vec, a] :
           vector<pair<vector<std::string>, Annotations>>{ad.front(),
                                                          ad.back()}) {
        vector<std::string> str_vec{bvt_str_vec};
        str_vec.emplace_back(":");
        extend(str_vec, vpi_str_vec);
        extend(str_vec, a_str_vec);
        str_vec.emplace_back(";");

        good_inputs.emplace_back(pair<vector<std::string>, BasicVarDecl>{
            str_vec, BasicVarDecl{bvt, vpi, a, {}}});

        for (const auto &[be_str_vec, be] :
             vector<pair<vector<std::string>, BasicExpr>>{bed.front(),
                                                          bed.back()}) {
          vector<std::string> str_vec_extended(str_vec);
          str_vec_extended.back() = "=";  // replace ';'
          extend(str_vec_extended, be_str_vec);
          str_vec_extended.emplace_back(";");
          good_inputs.emplace_back(pair<vector<std::string>, BasicVarDecl>{
              str_vec_extended, BasicVarDecl{bvt, vpi, a, be}});
        }
      }
    }
  }

  return good_inputs;
}

vector<pair<vector<std::string>, ArrayVarDecl>> array_var_decl_data_pos() {
  vector<pair<vector<std::string>, ArrayVarDecl>> good_inputs{};
  const auto avtd = array_var_type_data_pos();
  const auto vpid = var_par_identifier_data_pos();
  const auto ad = annotations_data_pos();
  const auto ald = array_literal_data_pos();

  for (const auto &[avt_str_vec, avt] : avtd) {
    for (const auto &[vpi_str_vec, vpi] :
         vector<pair<vector<std::string>, std::string>>{vpid.front(),
                                                        vpid.back()}) {
      for (const auto &[a_str_vec, a] :
           vector<pair<vector<std::string>, Annotations>>{ad.front(),
                                                          ad.back()}) {
        for (const auto &[be_str_vec, al] :
             vector<pair<vector<std::string>, ArrayLiteral>>{ald.front(),
                                                             ald.back()}) {
          vector<std::string> str_vec{avt_str_vec};
          str_vec.emplace_back(":");
          extend(str_vec, vpi_str_vec);
          extend(str_vec, a_str_vec);
          str_vec.emplace_back("=");
          extend(str_vec, be_str_vec);
          str_vec.emplace_back(";");
          good_inputs.emplace_back(pair<vector<std::string>, ArrayVarDecl>{
              str_vec, ArrayVarDecl{avt, vpi, a, al}});
        }
      }
    }
  }

  return good_inputs;
}

vector<pair<vector<std::string>, VarDeclItem>> var_decl_item_data_pos() {
  vector<pair<vector<std::string>, VarDeclItem>> good_inputs{};
  for (const auto &[str_vec, bvd] : basic_var_decl_data_pos()) {
    good_inputs.emplace_back(
        pair<vector<std::string>, VarDeclItem>{str_vec, bvd});
  }
  for (const auto &[str_vec, bad] : array_var_decl_data_pos()) {
    good_inputs.emplace_back(
        pair<vector<std::string>, VarDeclItem>{str_vec, bad});
  }
  return good_inputs;
}

vector<pair<vector<std::string>, ConstraintItem>> constraint_item_data_pos() {
  vector<pair<vector<std::string>, ConstraintItem>> good_inputs{};

  const auto ed = expr_data_pos();

  for (const auto &[id_str_vec, id] : identifier_data_pos()) {
    for (size_t i : vector<size_t>{0, 1, ed.size()}) {
      for (const auto &[a_str_vec, a] : annotations_data_pos()) {
        vector<std::string> str_vec{"constraint"};
        extend(str_vec, id_str_vec);

        vector<Expr> expressions{};

        str_vec.emplace_back("(");
        for (size_t j = 0; j < i; ++j) {
          if (j != 0) {
            str_vec.emplace_back(",");
          }
          extend(str_vec, ed.at(j).first);
          expressions.emplace_back(ed.at(j).second);
        }
        str_vec.emplace_back(")");

        extend(str_vec, a_str_vec);
        str_vec.emplace_back(";");

        good_inputs.emplace_back(pair<vector<std::string>, ConstraintItem>{
            str_vec, ConstraintItem{id, expressions, a}});
      }
    }
  }

  return good_inputs;
}

vector<pair<vector<std::string>, SolveSatisfy>> solve_satisfy_data_pos() {
  vector<pair<vector<std::string>, SolveSatisfy>> good_inputs{};

  const auto ad = annotations_data_pos();

  for (size_t i : vector<size_t>{0, 1, ad.size() - 1}) {
    vector<std::string> str_vec{"solve"};
    extend(str_vec, ad.at(i).first);
    str_vec.emplace_back("satisfy");
    str_vec.emplace_back(";");
    good_inputs.emplace_back(pair<vector<std::string>, SolveSatisfy>{
        str_vec, SolveSatisfy{ad.at(i).second}});
  }
  return good_inputs;
}

vector<pair<vector<std::string>, SolveOptimize>> solve_optimize_data_pos() {
  vector<pair<vector<std::string>, SolveOptimize>> good_inputs{};

  const auto ad = annotations_data_pos();

  for (const auto &[st_str, st] : vector<pair<std::string, OptimizationType>>{
           {"minimize", OptimizationType::MINIMIZE},
           {"maximize", OptimizationType::MAXIMIZE}}) {
    for (size_t i : vector<size_t>{0, 1, ad.size() - 1}) {
      for (const auto &[be_str_vec, be] : basic_expr_data_pos()) {
        vector<std::string> str_vec{"solve"};
        extend(str_vec, ad.at(i).first);
        str_vec.emplace_back(st_str);
        extend(str_vec, be_str_vec);
        str_vec.emplace_back(";");
        good_inputs.emplace_back(pair<vector<std::string>, SolveOptimize>{
            str_vec, SolveOptimize{ad.at(i).second, st, be}});
      }
    }
  }
  return good_inputs;
}

vector<pair<vector<std::string>, SolveItem>> solve_item_data_pos() {
  const auto ssd = solve_satisfy_data_pos();
  const auto sod = solve_optimize_data_pos();

  return vector<pair<vector<std::string>, SolveItem>>{
      pair<vector<std::string>, SolveItem>{ssd.front().first,
                                           SolveItem{ssd.front().second}},
      pair<vector<std::string>, SolveItem>{ssd.back().first,
                                           SolveItem{ssd.back().second}},
      pair<vector<std::string>, SolveItem>{sod.front().first,
                                           SolveItem{sod.front().second}},
      pair<vector<std::string>, SolveItem>{sod.back().first,
                                           SolveItem{sod.back().second}}};
}

template <typename T>
pair<vector<std::string>, std::vector<T>> flatten(
    const vector<pair<vector<std::string>, T>> &data_pos) {
  vector<std::string> str_vec{};
  vector<T> item_vec{};
  for (const pair<vector<std::string>, T> &p : data_pos) {
    extend(str_vec, p.first);
    item_vec.emplace_back(p.second);
  }
  return pair<vector<std::string>, std::vector<T>>{str_vec, item_vec};
}

vector<pair<vector<std::string>, Model>> model_data_pos() {
  vector<pair<vector<std::string>, Model>> good_inputs{};

  const auto pid_init = predicate_item_data_pos();
  const auto pdid_init = par_decl_item_data_pos();
  const auto vdid_init = var_decl_item_data_pos();
  const auto cid_init = constraint_item_data_pos();
  const auto sid = solve_item_data_pos();

  vector<pair<vector<std::string>, vector<PredicateItem>>> pid{
      {},
      {pid_init.front().first, {pid_init.front().second}},
      flatten<PredicateItem>(pid_init)};

  vector<pair<vector<std::string>, vector<ParDeclItem>>> pdid{
      {},
      {pdid_init.front().first, {pdid_init.front().second}},
      flatten<ParDeclItem>(pdid_init)};

  vector<pair<vector<std::string>, vector<VarDeclItem>>> vdid{
      {},
      {vdid_init.front().first, {vdid_init.front().second}},
      flatten<VarDeclItem>(vdid_init)};

  vector<pair<vector<std::string>, vector<ConstraintItem>>> cid{
      {},
      {cid_init.front().first, {cid_init.front().second}},
      flatten<ConstraintItem>(cid_init)};

  for (const auto &[pi_str_vec, pi] : pid) {
    for (const auto &[pdi_str_vec, pdi] : pdid) {
      for (const auto &[ci_str_vec, ci] : cid) {
        for (const auto &[si_str_vec, si] : sid) {
          vector<std::string> str_vec(pi_str_vec);
          extend(str_vec, pdi_str_vec);
          extend(str_vec, ci_str_vec);
          extend(str_vec, si_str_vec);
          good_inputs.emplace_back(pair<vector<std::string>, Model>{
              str_vec, Model{pi, pdi, vector<VarDeclItem>{}, ci, si}});
        }
      }
    }
  }
  return good_inputs;
}

vector<std::string> int_literal_data_neg() { return vector<std::string>{}; }
vector<std::string> float_literal_data_neg() { return vector<std::string>{}; }
vector<std::string> identifier_data_neg() { return vector<std::string>{}; }
vector<std::string> basic_par_type_data_neg() { return vector<std::string>{}; }
vector<std::string> bool_literal_data_neg() { return vector<std::string>{}; }
vector<std::string> set_literal_empty_data_neg() {
  return vector<std::string>{};
}
vector<std::string> float_set_literal_set_data_neg() {
  return vector<std::string>{};
}
vector<std::string> float_set_literal_bounded_data_neg() {
  return vector<std::string>{};
}
vector<std::string> int_set_literal_set_data_neg() {
  return vector<std::string>{};
}
vector<std::string> int_set_literal_bounded_data_neg() {
  return vector<std::string>{};
}
vector<std::string> string_literal_data_neg() { return vector<std::string>{}; }
vector<std::string> index_set_data_neg() { return vector<std::string>{}; }
vector<std::string> basic_par_type_array_data_neg() {
  return vector<std::string>{};
}
vector<std::string> par_type_data_neg() { return vector<std::string>{}; }
vector<std::string> basic_var_bool_type_data_neg() {
  return vector<std::string>{};
}
vector<std::string> basic_var_int_type_unbounded_data_neg() {
  return vector<std::string>{};
}
vector<std::string> basic_var_int_type_bounded_data_neg() {
  return vector<std::string>{};
}
vector<std::string> basic_var_int_type_set_data_neg() {
  return vector<std::string>{};
}
vector<std::string> basic_var_float_type_unbounded_data_neg() {
  return vector<std::string>{};
}
vector<std::string> basic_var_float_type_bounded_data_neg() {
  return vector<std::string>{};
}
vector<std::string> basic_var_set_type_bounded_data_neg() {
  return vector<std::string>{};
}
vector<std::string> basic_var_set_type_set_data_neg() {
  return vector<std::string>{};
}
vector<std::string> basic_var_type_data_neg() { return vector<std::string>{}; }
vector<std::string> array_var_type_data_neg() { return vector<std::string>{}; }
vector<std::string> index_set_unbounded_data_neg() {
  return vector<std::string>{};
}
vector<std::string> pred_index_set_data_neg() { return vector<std::string>{}; }
vector<std::string> basic_pred_param_type_int_bounded_data_neg() {
  return vector<std::string>{};
}
vector<std::string> basic_pred_param_type_float_bounded_data_neg() {
  return vector<std::string>{};
}
vector<std::string> basic_pred_param_type_int_set_data_neg() {
  return vector<std::string>{};
}
vector<std::string> basic_pred_param_type_set_bounded_data_neg() {
  return vector<std::string>{};
}
vector<std::string> basic_pred_param_type_set_set_data_neg() {
  return vector<std::string>{};
}
vector<std::string> basic_pred_param_type_data_neg() {
  return vector<std::string>{};
}
vector<std::string> pred_param_array_data_neg() {
  return vector<std::string>{};
}
vector<std::string> pred_param_type_data_neg() { return vector<std::string>{}; }
vector<std::string> pred_param_data_neg() { return vector<std::string>{}; }
vector<std::string> basic_literal_expr_data_neg() {
  return vector<std::string>{};
}
vector<std::string> basic_expr_data_neg() { return vector<std::string>{}; }
vector<std::string> array_literal_data_neg() { return vector<std::string>{}; }
vector<std::string> expr_data_neg() { return vector<std::string>{}; }
vector<std::string> par_array_literal_data_neg() {
  return vector<std::string>{};
}
vector<std::string> par_expr_data_neg() { return vector<std::string>{}; }
vector<std::string> par_decl_item_data_neg() { return vector<std::string>{}; }
vector<std::string> pred_param_array_type_data_neg() {
  return vector<std::string>{};
}
vector<std::string> basic_ann_expr_data_neg() { return vector<std::string>{}; }
vector<std::string> ann_expr_data_neg() { return vector<std::string>{}; }
vector<std::string> annotation_data_neg() { return vector<std::string>{}; }
vector<std::string> annotations_data_neg() { return vector<std::string>{}; }
vector<std::string> basic_var_decl_data_neg() { return vector<std::string>{}; }
vector<std::string> array_var_decl_data_neg() { return vector<std::string>{}; }
vector<std::string> var_decl_item_data_neg() { return vector<std::string>{}; }
vector<std::string> constraint_item_data_neg() { return vector<std::string>{}; }
vector<std::string> solve_satisfy_data_neg() { return vector<std::string>{}; }
vector<std::string> solve_optimize_data_neg() { return vector<std::string>{}; }
vector<std::string> solve_item_data_neg() { return vector<std::string>{}; }
vector<std::string> predicate_item_data_neg() { return vector<std::string>{}; }
vector<std::string> model_data_neg() { return vector<std::string>{}; }
vector<std::string> var_par_identifier_data_neg() {
  return vector<std::string>{};
}
}  // namespace fznparser::testing