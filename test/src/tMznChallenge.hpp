#pragma once

#include <gtest/gtest.h>

#include <algorithm>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_set>
#include <vector>
#include <stack>

#include "fznparser/parser/grammar.hpp"
#include "fznparser/parser/grammarDef.hpp"
#include "fznparser/transformer/modelTransformer.hpp"

namespace fznparser::testing {

using namespace fznparser;

namespace x3 = boost::spirit::x3;

struct FznData {
  size_t numVars;
  size_t numConstraints;
  bool isSatisfactionProblem;
};

const inline std::unordered_set<std::string> modelsThatCrash{
  std::string(FZN_CHALLENGE_DIR) + "/2020/code-generator/unison_hexagon_gcc.reload1.gen_reload.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/connect/connect__0018.mzn.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/connect/connect__0041_02.mzn.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/connect/connect__0061.mzn.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/connect/connect__0083_02.mzn.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/connect/connect__0086_02.mzn.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2023/yumi-static/yumi-static_p_9_GGGG_GGGGG_yumi_grid_setup_3_4.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2023/yumi-static/yumi-static_p_9_GGGG_GGGGG_yumi_grid_setup_3_4.fzn"
};

const inline std::unordered_set<std::string> modelsThatFail{
  std::string(FZN_CHALLENGE_DIR) + "/2012/project-planning/ProjectPlannertest_12_8.mzn.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2012/project-planning/ProjectPlannertest_13_6.mzn.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2012/project-planning/ProjectPlannertest_13_8.mzn.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2012/project-planning/ProjectPlannertest_14_8.mzn.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2012/project-planning/ProjectPlannertest_16_7.mzn.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2012/project-planning/ProjectPlannertest_17_9.mzn.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2015/project-planning/ProjectPlannertest_12_7.mzn.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2015/project-planning/ProjectPlannertest_14_7.mzn.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2015/project-planning/ProjectPlannertest_15_6.mzn.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2015/project-planning/ProjectPlannertest_16_6.mzn.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2015/project-planning/ProjectPlannertest_16_8.mzn.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/ATSP/atsp_instance10_0p25.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/ATSP/atsp_instance16_0p35.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/ATSP/atsp_instance19_0p35.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/ATSP/atsp_instance5_0p15.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/ATSP/atsp_instance9_0p25.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/monomatch/monomatch_data_n_3_percentage_0.97.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/monomatch/monomatch_data_n_4_percentage_0.75.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/monomatch/monomatch_data_n_4_percentage_0.97.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/monomatch/monomatch_data_n_5_percentage_0.5.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/monomatch/monomatch_data_n_6_percentage_0.5.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/physician-scheduling/physician-scheduling_instance03_0.34.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/physician-scheduling/physician-scheduling_instance10-duplicated_0.23.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/physician-scheduling/physician-scheduling_instance13-duplicated_0.56.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/physician-scheduling/physician-scheduling_instance17_oneweek_real.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2021/physician-scheduling/physician-scheduling_instance18_real.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2023/valve-network/valve-network_horizon12.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2023/valve-network/valve-network_horizon19.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2023/valve-network/valve-network_horizon20.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2023/valve-network/valve-network_horizon28.fzn",
  std::string(FZN_CHALLENGE_DIR) + "/2023/valve-network/valve-network_horizon6.fzn"
};

FznData get_fzn_data(const std::string& path) {
  const std::regex rx_var("^\\s*(var|array\\s+.*of\\s+var)\\s+");
  const std::regex rx_constraint("^\\s*constraint\\s+");
  const std::regex rx_csp("^\\s*solve\\s+.*satisfy;");
  
  FznData data{0, 0, false};
  
  std::ifstream input_file(path);
  if (!input_file.is_open()) {
    std::cout << "Could not open file: " << path << std::endl;
    return data;
  }
  
  std::istreambuf_iterator<char> i_file(input_file);

  std::istreambuf_iterator<char> eof;

  std::string line = "";
  while (i_file != eof) {
    if (*i_file == '\n') {
      if (!line.empty()) {
        if (std::regex_search(line, rx_var)) {
          ++data.numVars;
        } else if (std::regex_search(line, rx_constraint)) {
          ++data.numConstraints;
        } else if (std::regex_search(line, rx_csp)) {
          data.isSatisfactionProblem = true;
        }
        line.clear();
      }
    } else {
      if (*i_file == ':') {
        auto i_next = i_file;
        ++i_next;
        if (i_next != eof && *i_next != ':') {
          while (*i_file != '\n') {
            ++i_file;
          }
          continue;
        }
      }
      line += *i_file;
    }
    ++i_file;
  }

  input_file.close();
  return data;
}

void test_fzn_model(const std::string& path) {
  parser::Model resModel{std::vector<parser::PredicateItem>{},
    std::vector<parser::ParDeclItem>{},
    std::vector<parser::VarDeclItem>{},
    std::vector<parser::ConstraintItem>{},
    parser::SolveItem{parser::SolveSatisfy{std::vector<parser::Annotation>{}}}};
  std::ifstream input_file(path);
  EXPECT_TRUE(input_file.is_open()) << "Could not open file: " << path;
  boost::spirit::istream_iterator file_iterator(input_file >> std::noskipws), eof;
  EXPECT_TRUE(x3::phrase_parse(file_iterator, eof, parser::model,
                               x3::standard::space, resModel)) << path;
  EXPECT_TRUE(file_iterator == eof) << path;
  input_file.close();
  try {
    ModelTransformer transformer(std::move(resModel));
    fznparser::Model model = transformer.generateModel();
    const auto fznData = get_fzn_data(path);

    EXPECT_EQ(model.vars().size(), fznData.numVars) << path;
    EXPECT_EQ(model.constraints().size(), fznData.numConstraints) << path;
    EXPECT_EQ(model.isSatisfactionProblem(), fznData.isSatisfactionProblem) << path;
  } catch (const FznException &e) {
    FAIL() << "could not transofrm model: " << path << "\nFznException: " << e.what();
  }
}

TEST(mzn_challange, DISABLED_erronous) {
  std::vector<std::string> fznModels;
  for (const auto& fznModel : modelsThatCrash) {
    fznModels.push_back(fznModel);
  }
  std::sort(fznModels.begin(), fznModels.end());

  size_t i = 0;
  for (const auto& fznModel : fznModels) {
    std::cout << "\033[0;32m" << "[ PARSING  ] " << "\033[0;0m" << fznModel << " (" << std::to_string(++i) << " of " << std::to_string(fznModels.size()) << ')' << std::endl;
    test_fzn_model(fznModel);
  }
}

TEST(mzn_challange, DISABLED_failing) {
  std::vector<std::string> fznModels;
  for (const auto& fznModel : modelsThatFail) {
    fznModels.push_back(fznModel);
  }
  std::sort(fznModels.begin(), fznModels.end());  

  size_t i = 0;
  for (const auto& fznModel : fznModels) {
    std::cout << "\033[0;32m" << "[ PARSING  ] " << "\033[0;0m" << fznModel << " (" << std::to_string(++i) << " of " << std::to_string(fznModels.size()) << ')' << std::endl;
    test_fzn_model(fznModel);
  }
}

TEST(mzn_challenge, DISABLED_parse) { 
  std::vector<std::string> directories{std::string(FZN_CHALLENGE_DIR)};
  std::stack<std::string> stack;
  stack.push(std::string(FZN_CHALLENGE_DIR));
  std::vector<std::string> fznModels;
  std::unordered_set<std::string> visited{stack.top()};

  while (!stack.empty()) {
    const std::string& root = stack.top();
    for (const auto& entry : std::filesystem::directory_iterator(root)) {
      if (entry.is_directory()) {
        if (visited.contains(entry.path().string())) {
          continue;
        }
        stack.push(entry.path().string());
        visited.insert(entry.path().string());
      } else if (entry.path().extension() == ".fzn") {
        fznModels.push_back(entry.path().string());
      }
    }
    stack.pop();
  }
  std::sort(fznModels.begin(), fznModels.end());
  for (size_t i = 937; i < fznModels.size(); ++i) {
    if (modelsThatCrash.contains(fznModels.at(i)) || (false && modelsThatFail.contains(fznModels.at(i)))) {
      std::cout << "\033[0;33m" << "[ SKIPPING ] " << "\033[0;0m" << fznModels.at(i) << " (" << std::to_string(i + 1) << " of " << std::to_string(fznModels.size()) << ')' << std::endl;
    } else {
      std::cout << "\033[0;32m" << "[  PARSING ] " << "\033[0;0m" << fznModels.at(i) << " (" << std::to_string(i + 1) << " of " << std::to_string(fznModels.size()) << ')' << std::endl;
      test_fzn_model(fznModels.at(i));
    }
  }
}

}  // namespace fznparser::testing