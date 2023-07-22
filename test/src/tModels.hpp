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

void test_model(std::string filename) {
  parser::Model resModel;
  std::string path = std::string(FZN_DIR) + "/" + filename;
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
  transformer.generateModel();
}

TEST(model, car_sequencing) { test_model("car_sequencing.fzn"); }
TEST(model, magic_square) { test_model("magic_square.fzn"); }
TEST(model, n_queens) { test_model("n_queens.fzn"); }
TEST(model, tsp_alldiff) { test_model("tsp_alldiff.fzn"); }

}  // namespace fznparser::testing