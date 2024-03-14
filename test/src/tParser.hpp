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

#include "fznparser/parser.hpp"
#include "fznparser/parser/grammar.hpp"
#include "fznparser/parser/grammarDef.hpp"
#include "fznparser/transformer/modelTransformer.hpp"

namespace fznparser::testing {

using namespace fznparser;

namespace x3 = boost::spirit::x3;

TEST(parser, car_sequencing) {
  std::string filename = std::string(FZN_DIR) + "/car_sequencing.fzn";
  fznparser::Model resModel = parseFznFile(filename);
  EXPECT_EQ(resModel.numVars(), 61);
  EXPECT_EQ(resModel.numConstraints(), 88);
  EXPECT_TRUE(resModel.hasVar("Sequence"));
  std::shared_ptr<IntVarArray> sequence = std::get<std::shared_ptr<IntVarArray>>(resModel.var("Sequence"));
  std::vector<int64_t> outputIndexSetSizes = sequence->outputIndexSetSizes();
  EXPECT_EQ(outputIndexSetSizes.size(), 1);
  EXPECT_EQ(outputIndexSetSizes.front(), 10);
}

TEST(parser, magic_square) {
  std::string filename = std::string(FZN_DIR) + "/magic_square.fzn";
  fznparser::Model resModel = parseFznFile(filename);
  EXPECT_EQ(resModel.numVars(), 11);
  EXPECT_EQ(resModel.numConstraints(), 9);
  EXPECT_TRUE(resModel.hasVar("Magic"));
  std::shared_ptr<IntVarArray> magic = std::get<std::shared_ptr<IntVarArray>>(resModel.var("Magic"));
  std::vector<int64_t> outputIndexSetSizes = magic->outputIndexSetSizes();
  EXPECT_EQ(outputIndexSetSizes.size(), 2);
  EXPECT_EQ(outputIndexSetSizes.at(0), 3);
  EXPECT_EQ(outputIndexSetSizes.at(1), 3);
}

TEST(parser, n_queens) {
  std::string filename = std::string(FZN_DIR) + "/n_queens.fzn";
  fznparser::Model resModel = parseFznFile(filename);
  EXPECT_EQ(resModel.numVars(), 99);
  EXPECT_EQ(resModel.numConstraints(), 67);
  std::shared_ptr<IntVarArray> q = std::get<std::shared_ptr<IntVarArray>>(resModel.var("q"));
  std::vector<int64_t> outputIndexSetSizes = q->outputIndexSetSizes();
  EXPECT_EQ(outputIndexSetSizes.size(), 1);
  EXPECT_EQ(outputIndexSetSizes.at(0), 32);
}

TEST(parser, tsp_alldiff) {
  std::string filename = std::string(FZN_DIR) + "/tsp_alldiff.fzn";
  fznparser::Model resModel = parseFznFile(filename);
  EXPECT_EQ(resModel.numVars(), 37);
  EXPECT_EQ(resModel.numConstraints(), 19);
  std::shared_ptr<IntVarArray> sequence = std::get<std::shared_ptr<IntVarArray>>(resModel.var("Sequence"));
  std::vector<int64_t> outputIndexSetSizes = sequence->outputIndexSetSizes();
  EXPECT_EQ(outputIndexSetSizes.size(), 1);
  EXPECT_EQ(outputIndexSetSizes.at(0), 17);
}

}  // namespace fznparser::testing