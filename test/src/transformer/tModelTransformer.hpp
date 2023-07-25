#pragma once

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

#include "fznparser/parser/grammar.hpp"
#include "fznparser/parser/grammarDef.hpp"
#include "fznparser/transformer/modelTransformer.hpp"

namespace fznparser::testing {

using namespace fznparser;

namespace x3 = boost::spirit::x3;

void test_transformer(std::string filename) {
  parser::Model resModel;
  std::string path = std::string(STUB_DIR) + "/" + filename;
  std::ifstream input_file(path, std::ios_base::in);
  if (!input_file.is_open()) {
    FAIL() << "Could not open file: " << path;
  }
  std::string input{""};
  input_file.unsetf(std::ios::skipws);  // No white space skipping!
  std::copy(std::istream_iterator<char>(input_file),
            std::istream_iterator<char>(), std::back_inserter(input));
  input_file.close();

  auto iter = input.begin();
  EXPECT_TRUE(x3::phrase_parse(iter, input.end(), parser::model,
                               x3::standard::space, resModel))
      << filename;
  EXPECT_TRUE(iter == input.end());
  ModelTransformer transformer(std::move(resModel));
}

void test_generator(std::string filename, fznparser::Model expectedModel) {
  parser::Model resModel;
  std::string path = std::string(STUB_DIR) + "/" + filename;
  std::ifstream input_file(path, std::ios_base::in);
  if (!input_file.is_open()) {
    FAIL() << "Could not open file: " << path;
  }
  std::string input{""};
  input_file.unsetf(std::ios::skipws);  // No white space skipping!
  std::copy(std::istream_iterator<char>(input_file),
            std::istream_iterator<char>(), std::back_inserter(input));
  input_file.close();

  auto iter = input.begin();
  EXPECT_TRUE(x3::phrase_parse(iter, input.end(), parser::model,
                               x3::standard::space, resModel))
      << filename;
  EXPECT_TRUE(iter == input.end());
  ModelTransformer transformer(std::move(resModel));
  fznparser::Model actualModel = transformer.generateModel();
  EXPECT_TRUE(transformer.generateModel() == expectedModel) << filename;
}

TEST(transformer, annotations) { test_transformer("annotations.fzn"); }
TEST(transformer, constraints) { test_transformer("constraints.fzn"); }
TEST(transformer, maximize_objective) {
  test_transformer("maximize_objective.fzn");
}
TEST(transformer, minimize_objective) {
  test_transformer("minimize_objective.fzn");
}
TEST(transformer, parameters) { test_transformer("parameters.fzn"); }
TEST(transformer, satisfy_empty) { test_transformer("satisfy_empty.fzn"); }
TEST(transformer, variable_arrays) { test_transformer("variable_arrays.fzn"); }
TEST(transformer, variables) { test_transformer("variables.fzn"); }

TEST(generator, annotations) {
  IntVar a("a");
  a.addAnnotation("output_var");
  IntVar b("b");
  IntVar c("c");
  IntVar d("d");
  std::unordered_map<std::string_view, Variable> variables;
  variables.emplace("a", Variable{std::move(a)});
  variables.emplace("b", Variable{std::move(b)});
  variables.emplace("c", Variable{std::move(c)});
  variables.emplace("d", Variable{std::move(d)});

  IntVarArray arr("arr");
  arr.append(std::get<IntVar>(variables.at("b")));
  arr.append(std::get<IntVar>(variables.at("c")));
  arr.append(std::get<IntVar>(variables.at("c")));
  arr.append(std::get<IntVar>(variables.at("d")));
  arr.addAnnotation("output_array", {IntSet(1, 2), IntSet(1, 2)});
  variables.emplace("arr", Variable{std::move(arr)});

  Constraint int_plus(
      "int_plus",
      std::vector<Arg>{IntArg{std::get<IntVar>(variables.at("a"))},
                       IntArg{std::get<IntVar>(variables.at("b"))},
                       IntArg{std::get<IntVar>(variables.at("c"))}});

  int_plus.addAnnotation("defines_var",
                         AnnotationExpression{fznparser::Annotation("c")});

  std::vector<Constraint> constraints;
  constraints.emplace_back(std::move(int_plus));

  test_generator(
      "annotations.fzn",
      std::move(fznparser::Model(std::move(variables), std::move(constraints),
                                 std::move(SolveType()))));
}

TEST(generator, constraints) {
  IntVar v1("v1");
  IntVar v2("v2");
  std::unordered_map<std::string_view, Variable> variables;
  variables.emplace("v1", Variable{std::move(v1)});
  variables.emplace("v2", Variable{std::move(v2)});

  std::vector<Constraint> constraints;

  IntVarArray coefs("");
  coefs.append(1);
  coefs.append(-1);

  IntVarArray arr("");
  arr.append(std::get<IntVar>(variables.at("v1")));
  arr.append(std::get<IntVar>(variables.at("v2")));

  constraints.emplace_back(Constraint(
      "int_lin_eq",
      std::vector<Arg>{std::move(coefs), std::move(arr), IntArg{2}}));

  constraints.emplace_back(Constraint(
      "set_in",
      std::vector<Arg>{IntArg{std::reference_wrapper<const IntVar>(
                           std::get<IntVar>(variables.at("v1")))},
                       IntSetArg{IntSet(std::vector<int64_t>{1, 4})}}));

  test_generator(
      "constraints.fzn",
      std::move(fznparser::Model(std::move(variables), std::move(constraints),
                                 std::move(SolveType()))));
}

TEST(generator, maximize_objective) {
  IntVar a("a");
  std::unordered_map<std::string_view, Variable> variables;
  variables.emplace("a", Variable{std::move(a)});

  SolveType solveType(ProblemType::MAXIMIZE, variables.at("a"));

  test_generator("maximize_objective.fzn",
                 std::move(fznparser::Model(std::move(variables), {},
                                            std::move(solveType))));
}

TEST(generator, minimize_objective) {
  IntVar a("a");
  std::unordered_map<std::string_view, Variable> variables;
  variables.emplace("a", Variable{std::move(a)});

  SolveType solveType(ProblemType::MINIMIZE, variables.at("a"));

  test_generator("minimize_objective.fzn",
                 std::move(fznparser::Model(std::move(variables), {},
                                            std::move(solveType))));
}

TEST(generator, parameters) {
  test_generator("parameters.fzn", fznparser::Model(std::vector<Variable>{}, {},
                                                    std::move(SolveType())));
}

TEST(generator, satisfy_empty) {
  test_generator(
      "satisfy_empty.fzn",
      fznparser::Model(std::vector<Variable>{}, {}, std::move(SolveType())));
}

TEST(generator, variable_arrays) {
  std::unordered_map<std::string_view, Variable> variables;
  variables.emplace("v1", Variable{IntVar("v1")});
  variables.emplace("v2", Variable{IntVar("v2")});
  variables.emplace("v3", Variable{BoolVar("v3")});

  IntVarArray array1("array1");
  array1.append(std::get<IntVar>(variables.at("v1")));
  array1.append(2);
  array1.append(std::get<IntVar>(variables.at("v2")));
  array1.append(5);
  variables.emplace("array1", Variable{std::move(array1)});

  BoolVarArray array2("array2");
  array2.append(std::get<BoolVar>(variables.at("v3")));
  array2.append(true);
  array2.append(false);
  variables.emplace("array2", Variable{std::move(array2)});

  test_generator("variable_arrays.fzn",
                 std::move(fznparser::Model(std::move(variables), {},
                                            std::move(SolveType()))));
}

TEST(generator, variables) {
  std::unordered_map<std::string_view, Variable> variables;
  variables.emplace("v1", Variable{BoolVar("v1")});
  variables.emplace("v2", Variable{IntVar(0, 5, "v2")});
  variables.emplace("v3",
                    Variable{IntVar(std::vector<int64_t>{3, 5, 10}, "v3")});
  variables.emplace("v4", Variable{IntVar(5, "v4")});
  variables.emplace("v5", Variable{IntVar(3, "v5")});
  variables.emplace("v6", Variable{IntVar("v6")});

  test_generator("variables.fzn",
                 std::move(fznparser::Model(std::move(variables), {},
                                            std::move(SolveType()))));
}

}  // namespace fznparser::testing