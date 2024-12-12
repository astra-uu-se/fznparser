#include <gtest/gtest.h>

#include <algorithm>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "./expectEq.hpp"
#include "fznparser/parser/grammar.hpp"
#include "fznparser/parser/grammarDef.hpp"

namespace fznparser::testing {

using namespace fznparser::parser;

namespace x3 = boost::spirit::x3;

void test_stub(const std::string& filename, const parser::Model& actual) {
  parser::Model expected;
  std::string path = std::string(STUB_DIR) + "/" + filename;

  std::ifstream input_file(path);
  EXPECT_TRUE(input_file.is_open()) << "Could not open file: " << path;
  boost::spirit::istream_iterator file_iterator(input_file >> std::noskipws),
      eof;
  EXPECT_TRUE(x3::phrase_parse(file_iterator, eof, parser::model,
                               parser::skipper, expected))
      << path;
  if (file_iterator != eof) {
    size_t buffer_size = 200;
    std::string buffer;
    buffer.reserve(buffer_size);
    for (size_t i = 0; i < buffer_size; ++i) {
      if (file_iterator == eof || *file_iterator == '\n') {
        break;
      }
      buffer += *file_iterator;
      ++file_iterator;
    }
    input_file.close();
    std::cout << '"' << buffer << '"' << std::endl;
    FAIL() << path;
  }
  expect_eq(expected, actual, filename);
}

TEST(stubs, annotations) {
  test_stub(
      "annotations.fzn",
      parser::Model{
          std::vector<PredicateItem>{}, std::vector<ParDeclItem>{},
          std::vector<VarDeclItem>{
              BasicVarDecl{BasicVarIntTypeUnbounded{}, std::string{"a"},
                           Annotations{Annotation{std::string{"output_var"},
                                                  std::vector<AnnExpr>{}}},
                           std::optional<BasicExpr>{std::nullopt}},
              BasicVarDecl{BasicVarIntTypeUnbounded{}, std::string{"b"},
                           Annotations{},
                           std::optional<BasicExpr>{std::nullopt}},
              BasicVarDecl{BasicVarIntTypeUnbounded{}, std::string{"c"},
                           Annotations{},
                           std::optional<BasicExpr>{std::nullopt}},
              BasicVarDecl{BasicVarIntTypeUnbounded{}, std::string{"d"},
                           Annotations{},
                           std::optional<BasicExpr>{std::nullopt}},
              ArrayVarDecl{
                  ArrayVarType{IndexSet{4}, BasicVarIntTypeUnbounded{}},
                  std::string{"arr"},
                  Annotations{
                      Annotation{std::string{"output_array"},
                                 std::vector<AnnExpr>{std::vector<BasicAnnExpr>{
                                     IntSetLiteralBounded{1, 2},
                                     IntSetLiteralBounded{1, 2}}}}},
                  ArrayLiteral{std::string{"b"}, std::string{"c"},
                               std::string{"c"}, std::string{"d"}}}},
          std::vector<ConstraintItem>{ConstraintItem{
              std::string{"int_plus"},
              std::vector<Expr>{std::string{"a"}, std::string{"b"},
                                std::string{"c"}},
              Annotations{Annotation{
                  std::string{"defines_var"},
                  std::vector<AnnExpr>{std::vector<BasicAnnExpr>{
                      Annotation{std::string{"c"}, std::vector<AnnExpr>{}}}}}}

          }},
          SolveSatisfy{}});
}

TEST(stubs, constraints) {
  test_stub(
      "constraints.fzn",
      parser::Model{
          std::vector<PredicateItem>{},
          std::vector<ParDeclItem>{ParDeclItem{
              BasicParTypeArray{{2}, BasicParType::INT}, std::string{"coeffs"},
              ParExpr{ParArrayLiteral{int64_t{1}, int64_t{-1}}}}},
          std::vector<VarDeclItem>{
              BasicVarDecl{BasicVarIntTypeUnbounded{}, std::string{"v1"},
                           Annotations{},
                           std::optional<BasicExpr>{std::nullopt}},
              BasicVarDecl{BasicVarIntTypeUnbounded{}, std::string{"v2"},
                           Annotations{},
                           std::optional<BasicExpr>{std::nullopt}},
              BasicVarDecl{BasicVarIntTypeBounded{0, 139},
                           std::string{"X_INTRODUCED_2233_"},
                           Annotations{Annotation{"var_is_introduced", {}},
                                       Annotation{"is_defined_var", {}}},
                           std::optional<BasicExpr>{std::nullopt}},
              BasicVarDecl{BasicVarIntTypeBounded{0, 139},
                           std::string{"X_INTRODUCED_1888_"},
                           Annotations{Annotation{"var_is_introduced", {}},
                                       Annotation{"is_defined_var", {}}},
                           std::optional<BasicExpr>{std::nullopt}},
              BasicVarDecl{BasicVarIntTypeBounded{0, 139},
                           std::string{"X_INTRODUCED_1974_"},
                           Annotations{Annotation{"var_is_introduced", {}},
                                       Annotation{"is_defined_var", {}}},
                           std::optional<BasicExpr>{std::nullopt}},
              BasicVarDecl{BasicVarIntTypeBounded{0, 139},
                           std::string{"X_INTRODUCED_2060_"},
                           Annotations{Annotation{"var_is_introduced", {}},
                                       Annotation{"is_defined_var", {}}},
                           std::optional<BasicExpr>{std::nullopt}},
              BasicVarDecl{
                  BasicVarIntTypeBounded{0, 139},
                  std::string{"X_INTRODUCED_2146_"},
                  Annotations{Annotation{"var_is_introduced", {}},
                              Annotation{"is_defined_var", {}}},
                  std::optional<BasicExpr>{std::nullopt},
              },
              BasicVarDecl{BasicVarIntTypeBounded{0, 139},
                           std::string{"X_INTRODUCED_2232_"},
                           Annotations{Annotation{"var_is_introduced", {}},
                                       Annotation{"is_defined_var", {}}},
                           std::optional<BasicExpr>{std::nullopt}},
              ArrayVarDecl{
                  ArrayVarType{IndexSet{5}, BasicVarIntTypeUnbounded{}},
                  std::string{"information"}, Annotations{},
                  ArrayLiteral{std::string{"X_INTRODUCED_1888_"},
                               std::string{"X_INTRODUCED_1974_"},
                               std::string{"X_INTRODUCED_2060_"},
                               std::string{"X_INTRODUCED_2146_"},
                               std::string{"X_INTRODUCED_2232_"}}}},
          std::vector<ConstraintItem>{
              ConstraintItem{std::string{"int_lin_eq"},
                             std::vector<Expr>{std::string{"coeffs"},
                                               ArrayLiteral{std::string{"v1"},
                                                            std::string{"v2"}},
                                               int64_t(2)},
                             Annotations{}},
              ConstraintItem{std::string{"set_in"},
                             std::vector<Expr>{
                                 std::string{"v1"},
                                 IntSetLiteralSet{std::vector<int64_t>{1, 4}}},
                             Annotations{}},
              ConstraintItem{
                  std::string{"array_int_minimum"},
                  std::vector<Expr>{std::string{"X_INTRODUCED_2233_"},
                                    std::string{"information"}},
                  Annotations{}}},
          SolveSatisfy{}});
}

TEST(stubs, maximize_objective) {
  test_stub(
      "maximize_objective.fzn",
      parser::Model{std::vector<PredicateItem>{}, std::vector<ParDeclItem>{},
                    std::vector<VarDeclItem>{BasicVarDecl{
                        BasicVarIntTypeUnbounded{}, std::string{"a"},
                        Annotations{}, std::optional<BasicExpr>{std::nullopt}}},
                    std::vector<ConstraintItem>{},
                    SolveOptimize{Annotations{}, OptimizationType::MAXIMIZE,
                                  BasicExpr{std::string{"a"}}}});
}

TEST(stubs, minimize_objective) {
  test_stub(
      "minimize_objective.fzn",
      parser::Model{std::vector<PredicateItem>{}, std::vector<ParDeclItem>{},
                    std::vector<VarDeclItem>{BasicVarDecl{
                        BasicVarIntTypeUnbounded{}, std::string{"a"},
                        Annotations{}, std::optional<BasicExpr>{std::nullopt}}},
                    std::vector<ConstraintItem>{},
                    SolveOptimize{Annotations{}, OptimizationType::MINIMIZE,
                                  BasicExpr{std::string{"a"}}}});
}

TEST(stubs, parameters) {
  test_stub(
      "parameters.fzn",
      parser::Model{
          std::vector<PredicateItem>{},
          std::vector<ParDeclItem>{
              ParDeclItem{BasicParType::INT, std::string{"n"},
                          ParExpr{int64_t{4}}},
              ParDeclItem{BasicParType::BOOL, std::string{"bF"},
                          ParExpr{bool{false}}},
              ParDeclItem{BasicParType::BOOL, std::string{"bT"},
                          ParExpr{bool{true}}},
              ParDeclItem{BasicParTypeArray{{4}, BasicParType::INT},
                          std::string{"nums"},
                          ParExpr{ParArrayLiteral{int64_t{1}, int64_t{2},
                                                  int64_t{3}, int64_t{4}}}},
              ParDeclItem{BasicParTypeArray{{3}, BasicParType::BOOL},
                          std::string{"bools"},
                          ParExpr{ParArrayLiteral{bool{false}, bool{false},
                                                  bool{true}}}},
              ParDeclItem{
                  BasicParType::SET_OF_INT, std::string{"explicitSet"},
                  ParExpr{IntSetLiteralSet{std::vector<int64_t>{1, 4, 5}}}},
              ParDeclItem{BasicParType::SET_OF_INT, std::string{"intervalSet"},
                          ParExpr{IntSetLiteralBounded{1, 10}}},
              ParDeclItem{BasicParTypeArray{{2}, BasicParType::SET_OF_INT},
                          std::string{"sets"},
                          ParExpr{ParArrayLiteral{
                              IntSetLiteralSet{std::vector<int64_t>{4, 50, 55}},
                              IntSetLiteralBounded{1, 10}}}}},
          std::vector<VarDeclItem>{}, std::vector<ConstraintItem>{},
          SolveSatisfy{}});
}

TEST(stubs, predicates) {
  test_stub(
      "predicates.fzn",
      parser::Model{
          std::vector<PredicateItem>{
              PredicateItem{"pred", std::vector<PredParam>{}},
              PredicateItem{"pred_1",
                            std::vector<PredParam>{PredParam{
                                BasicVarIntTypeUnbounded{}, std::string{"a"}}}},
              PredicateItem{
                  "int_eq_imp",
                  std::vector<PredParam>{
                      PredParam{BasicVarIntTypeUnbounded{}, std::string{"a"}},
                      PredParam{BasicVarIntTypeUnbounded{}, std::string{"b"}},
                      PredParam{BasicVarBoolType{}, std::string{"r"}}}}},
          std::vector<ParDeclItem>{}, std::vector<VarDeclItem>{},
          std::vector<ConstraintItem>{}, SolveSatisfy{}});
}

TEST(stubs, satisfy_empty) {
  test_stub(
      "satisfy_empty.fzn",
      parser::Model{std::vector<PredicateItem>{}, std::vector<ParDeclItem>{},
                    std::vector<VarDeclItem>{}, std::vector<ConstraintItem>{},
                    SolveSatisfy{}});
}

TEST(stubs, variable_arrays) {
  test_stub(
      "variable_arrays.fzn",
      parser::Model{
          std::vector<PredicateItem>{}, std::vector<ParDeclItem>{},
          std::vector<VarDeclItem>{
              BasicVarDecl{BasicVarIntTypeUnbounded{}, std::string{"v1"},
                           Annotations{},
                           std::optional<BasicExpr>{std::nullopt}},
              BasicVarDecl{BasicVarIntTypeUnbounded{}, std::string{"v2"},
                           Annotations{},
                           std::optional<BasicExpr>{std::nullopt}},
              BasicVarDecl{BasicVarBoolType{}, std::string{"v3"}, Annotations{},
                           std::optional<BasicExpr>{std::nullopt}},
              ArrayVarDecl{
                  ArrayVarType{IndexSet{4}, BasicVarIntTypeUnbounded{}},
                  std::string{"array1"}, Annotations{},
                  ArrayLiteral{std::string{"v1"}, int64_t{2}, std::string{"v2"},
                               int64_t{5}}},
              ArrayVarDecl{
                  ArrayVarType{IndexSet{3}, BasicVarBoolType{}},
                  std::string{"array2"}, Annotations{},
                  ArrayLiteral{std::string{"v3"}, bool{true}, bool{false}}}},
          std::vector<ConstraintItem>{}, SolveSatisfy{}});
}

TEST(stubs, variables) {
  test_stub(
      "variables.fzn",
      parser::Model{
          std::vector<PredicateItem>{},
          std::vector<ParDeclItem>{ParDeclItem{
              BasicParType::INT, std::string{"n"}, ParExpr{int64_t{3}}}},
          std::vector<VarDeclItem>{
              BasicVarDecl{BasicVarBoolType{}, std::string{"v1"}, Annotations{},
                           std::optional<BasicExpr>{std::nullopt}},
              BasicVarDecl{BasicVarIntTypeBounded{0, 5}, std::string{"v2"},
                           Annotations{},
                           std::optional<BasicExpr>{std::nullopt}},
              BasicVarDecl{BasicVarIntTypeSet{std::vector<int64_t>{3, 5, 10}},
                           std::string{"v3"}, Annotations{},
                           std::optional<BasicExpr>{std::nullopt}},
              BasicVarDecl{BasicVarIntTypeBounded{1, 5}, std::string{"v4"},
                           Annotations{}, std::optional<BasicExpr>{int64_t{5}}},
              BasicVarDecl{BasicVarIntTypeBounded{1, 5}, std::string{"v5"},
                           Annotations{},
                           std::optional<BasicExpr>{std::string{"n"}}},
              BasicVarDecl{BasicVarIntTypeUnbounded{}, std::string{"v6"},
                           Annotations{},
                           std::optional<BasicExpr>{std::nullopt}}},
          std::vector<ConstraintItem>{}, SolveSatisfy{}});
}

TEST(stubs, comments) {
  test_stub(
      "comments.fzn",
      parser::Model{
          std::vector<PredicateItem>{},
          std::vector<ParDeclItem>{ParDeclItem{
              BasicParType::INT, std::string{"n"}, ParExpr{int64_t{3}}}},
          std::vector<VarDeclItem>{
              BasicVarDecl{BasicVarBoolType{}, std::string{"v1"}, Annotations{},
                           std::optional<BasicExpr>{std::nullopt}},
              BasicVarDecl{BasicVarIntTypeBounded{0, 5}, std::string{"v2"},
                           Annotations{},
                           std::optional<BasicExpr>{std::nullopt}},
              BasicVarDecl{BasicVarIntTypeSet{std::vector<int64_t>{3, 5, 10}},
                           std::string{"v3"}, Annotations{},
                           std::optional<BasicExpr>{std::nullopt}},
              BasicVarDecl{BasicVarIntTypeBounded{1, 5}, std::string{"v4"},
                           Annotations{}, std::optional<BasicExpr>{int64_t{5}}},
              BasicVarDecl{BasicVarIntTypeBounded{1, 5}, std::string{"v5"},
                           Annotations{},
                           std::optional<BasicExpr>{std::string{"n"}}},
              BasicVarDecl{BasicVarIntTypeUnbounded{}, std::string{"v6"},
                           Annotations{},
                           std::optional<BasicExpr>{std::nullopt}}},
          std::vector<ConstraintItem>{}, SolveSatisfy{}});
}

}  // namespace fznparser::testing