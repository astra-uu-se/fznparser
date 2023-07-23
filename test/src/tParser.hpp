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
  EXPECT_EQ(resModel.numVariables(), 61);
  EXPECT_EQ(resModel.numConstraints(), 88);
  EXPECT_TRUE(resModel.hasVariable("Sequence"));
  IntVarArray sequence = std::get<IntVarArray>(resModel.variable("Sequence"));
  std::vector<IntSet> outputArrayIndexSets = sequence.outputArrayIndexSets();
  EXPECT_EQ(outputArrayIndexSets.size(), 1);
  EXPECT_EQ(outputArrayIndexSets.front().size(), 10);
  EXPECT_EQ(outputArrayIndexSets.front().lowerBound(), 1);
  EXPECT_EQ(outputArrayIndexSets.front().upperBound(), 10);
}

TEST(parser, magic_square) {
  std::string filename = std::string(FZN_DIR) + "/magic_square.fzn";
  fznparser::Model resModel = parseFznFile(filename);
  EXPECT_EQ(resModel.numVariables(), 11);
  EXPECT_EQ(resModel.numConstraints(), 9);
  EXPECT_TRUE(resModel.hasVariable("Magic"));
  IntVarArray magic = std::get<IntVarArray>(resModel.variable("Magic"));
  std::vector<IntSet> outputArrayIndexSets = magic.outputArrayIndexSets();
  EXPECT_EQ(outputArrayIndexSets.size(), 2);
  EXPECT_EQ(outputArrayIndexSets.at(0).size(), 3);
  EXPECT_EQ(outputArrayIndexSets.at(0).lowerBound(), 1);
  EXPECT_EQ(outputArrayIndexSets.at(0).upperBound(), 3);
  EXPECT_EQ(outputArrayIndexSets.at(1).size(), 3);
  EXPECT_EQ(outputArrayIndexSets.at(1).lowerBound(), 1);
  EXPECT_EQ(outputArrayIndexSets.at(1).upperBound(), 3);
}

TEST(parser, n_queens) {
  std::string filename = std::string(FZN_DIR) + "/n_queens.fzn";
  fznparser::Model resModel = parseFznFile(filename);
  EXPECT_EQ(resModel.numVariables(), 99);
  EXPECT_EQ(resModel.numConstraints(), 67);
  IntVarArray q = std::get<IntVarArray>(resModel.variable("q"));
  std::vector<IntSet> outputArrayIndexSets = q.outputArrayIndexSets();
  EXPECT_EQ(outputArrayIndexSets.size(), 1);
  EXPECT_EQ(outputArrayIndexSets.at(0).size(), 32);
  EXPECT_EQ(outputArrayIndexSets.at(0).lowerBound(), 1);
  EXPECT_EQ(outputArrayIndexSets.at(0).upperBound(), 32);
}

TEST(parser, tsp_alldiff) {
  std::string filename = std::string(FZN_DIR) + "/tsp_alldiff.fzn";
  fznparser::Model resModel = parseFznFile(filename);
  EXPECT_EQ(resModel.numVariables(), 37);
  EXPECT_EQ(resModel.numConstraints(), 19);
  IntVarArray sequence = std::get<IntVarArray>(resModel.variable("Sequence"));
  std::vector<IntSet> outputArrayIndexSets = sequence.outputArrayIndexSets();
  EXPECT_EQ(outputArrayIndexSets.size(), 1);
  EXPECT_EQ(outputArrayIndexSets.at(0).size(), 17);
  EXPECT_EQ(outputArrayIndexSets.at(0).lowerBound(), 1);
  EXPECT_EQ(outputArrayIndexSets.at(0).upperBound(), 17);
}

}  // namespace fznparser::testing