#include <gtest/gtest.h>

#include <algorithm>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <filesystem>
#include <fstream>
#include <regex>
#include <stack>
#include <string>
#include <unordered_set>
#include <vector>

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

std::string ltrim(std::string s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                                  [](unsigned char ch) { return ch != '/'; }));
  return s;
}

void logModelName(const std::string& modelPath, bool skipping, size_t index,
                  size_t total) {
  size_t padding = std::to_string(total).size();
  std::cout << (skipping ? "\033[0;33m[ SKIPPING" : "\033[0;32m[  PARSING")
            << " ] (" << std::setw(padding) << std::to_string(index + 1) << '/'
            << std::to_string(total) << ")\033[0;0m "
            << (modelPath.rfind(std::string{FZN_CHALLENGE_DIR}, 0) == 0
                    ? ltrim(modelPath.substr(
                          std::string{FZN_CHALLENGE_DIR}.size()))
                    : modelPath)
            << std::endl;
}

const inline std::unordered_set<std::string> modelsThatCrash{};

const inline std::unordered_set<std::string> modelsThatFail{};

void skipLine(std::istreambuf_iterator<char>& i_file,
              const std::istreambuf_iterator<char>& eof) {
  while (i_file != eof && *i_file != '\n') {
    ++i_file;
  }
}

void skipWs(std::istreambuf_iterator<char>& i_file,
            const std::istreambuf_iterator<char>& eof) {
  while (i_file != eof && *i_file != '\n' && std::isspace(*i_file)) {
    ++i_file;
  }
}

std::string nextWord(std::istreambuf_iterator<char>& i_file,
                     const std::istreambuf_iterator<char>& eof) {
  skipWs(i_file, eof);
  if (i_file == eof || *i_file == '\n') {
    return "";
  }
  std::string w;
  while (i_file != eof && *i_file != '\n' && !std::isspace(*i_file)) {
    w += *i_file;
    ++i_file;
    if (*i_file == ':') {
      break;
    }
  }
  return w;
}

std::string lastWord(std::istreambuf_iterator<char>& i_file,
                     const std::istreambuf_iterator<char>& eof) {
  std::string w;
  while (i_file != eof && *i_file != '\n') {
    if (std::isspace(*i_file)) {
      w.clear();
    } else {
      w += *i_file;
      if (*i_file == ';') {
        return w;
      }
    }
    ++i_file;
  }
  return "";
}

FznData get_fzn_data(const std::string& path) {
  const std::regex rx_var(R"(^\s*array\s+.*of\s+var\s+)");

  FznData data{0, 0, false};

  std::ifstream input_file(path);
  if (!input_file.is_open()) {
    std::cout << "Could not open file: " << path << std::endl;
    return data;
  }

  std::istreambuf_iterator<char> i_file(input_file);

  std::istreambuf_iterator<char> eof;

  std::string line;
  while (i_file != eof) {
    // skip whitespace;
    skipWs(i_file, eof);
    if (i_file != eof && *i_file == '\n') {
      line.clear();
      ++i_file;
      continue;
    }
    while (i_file != eof && *i_file != '\n') {
      std::string word = nextWord(i_file, eof);
      if (line.empty()) {
        if (word == "var") {
          ++data.numVars;
          skipLine(i_file, eof);
          break;
        } else if (word == "constraint") {
          ++data.numConstraints;
          skipLine(i_file, eof);
          break;
        } else if (word == "solve") {
          data.isSatisfactionProblem = (lastWord(i_file, eof) == "satisfy;");
          skipLine(i_file, eof);
          break;
        } else if (word != "array") {
          skipLine(i_file, eof);
          break;
        }
        line = word;
      } else {
        line += ' ' + word;
        if (std::regex_search(line, rx_var)) {
          ++data.numVars;
          line.clear();
          skipLine(i_file, eof);
          break;
        }
        if (word.back() == ':') {
          line.clear();
          skipLine(i_file, eof);
          break;
        }
      }
    }
  }

  input_file.close();
  return data;
}

void test_fzn_predicate_items(boost::spirit::istream_iterator& file_iterator,
                              const boost::spirit::istream_iterator& eof,
                              std::vector<parser::PredicateItem>& predicates) {
  while (true) {
    parser::PredicateItem predicateItem;
    if (!x3::phrase_parse(file_iterator, eof, parser::predicate_item, x3::space,
                          predicateItem)) {
      break;
    }
    predicates.push_back(predicateItem);
  }
}

void test_fzn_par_decl_items(boost::spirit::istream_iterator& file_iterator,
                             const boost::spirit::istream_iterator& eof,
                             std::vector<parser::ParDeclItem>& parDeclItems) {
  while (true) {
    parser::ParDeclItem parDeclItem;
    if (!x3::phrase_parse(file_iterator, eof, parser::par_decl_item, x3::space,
                          parDeclItem)) {
      break;
    }
    parDeclItems.push_back(parDeclItem);
  }
}

void test_fzn_var_decl_items(boost::spirit::istream_iterator& file_iterator,
                             const boost::spirit::istream_iterator& eof,
                             std::vector<parser::VarDeclItem>& varDeclItems) {
  while (true) {
    parser::VarDeclItem varDeclItem;
    if (!x3::phrase_parse(file_iterator, eof, parser::var_decl_item, x3::space,
                          varDeclItem)) {
      break;
    }
    varDeclItems.push_back(varDeclItem);
  }
}

void test_fzn_constraint_items(
    boost::spirit::istream_iterator& file_iterator,
    const boost::spirit::istream_iterator& eof,
    std::vector<parser::ConstraintItem>& constraintItems) {
  while (true) {
    parser::ConstraintItem constraintItem;
    if (!x3::phrase_parse(file_iterator, eof, parser::constraint_item,
                          x3::space, constraintItem)) {
      break;
    }
    constraintItems.push_back(constraintItem);
  }
}

void test_fzn_solve_item(boost::spirit::istream_iterator& file_iterator,
                         const boost::spirit::istream_iterator& eof,
                         parser::SolveItem& solveItem) {
  EXPECT_TRUE(x3::phrase_parse(file_iterator, eof, parser::solve_item,
                               x3::space, solveItem));
}

void test_fzn_model_breakdown(const std::string& path) {
  std::ifstream input_file(path);
  EXPECT_TRUE(input_file.is_open()) << "Could not open file: " << path;
  boost::spirit::istream_iterator file_iterator(input_file >> std::noskipws),
      eof;

  parser::Model resModel{
      std::vector<parser::PredicateItem>{}, std::vector<parser::ParDeclItem>{},
      std::vector<parser::VarDeclItem>{}, std::vector<parser::ConstraintItem>{},
      parser::SolveItem{
          parser::SolveSatisfy{std::vector<parser::Annotation>{}}}};
  test_fzn_predicate_items(file_iterator, eof, resModel.predicateItems);
  test_fzn_par_decl_items(file_iterator, eof, resModel.parDeclItems);
  test_fzn_var_decl_items(file_iterator, eof, resModel.varDeclItems);
  test_fzn_constraint_items(file_iterator, eof, resModel.constraintItems);
  test_fzn_solve_item(file_iterator, eof, resModel.solveItem);

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
    std::cout << buffer << std::endl;
    FAIL() << path;
  }
  input_file.close();
  try {
    ModelTransformer transformer(std::move(resModel));
    fznparser::Model model = transformer.generateModel();
    const auto fznData = get_fzn_data(path);

    EXPECT_EQ(model.vars().size(), fznData.numVars) << path;
    EXPECT_EQ(model.constraints().size(), fznData.numConstraints) << path;
    EXPECT_EQ(model.isSatisfactionProblem(), fznData.isSatisfactionProblem)
        << path;
  } catch (const FznException& e) {
    EXPECT_TRUE(false) << "could not transform model: " << path
                       << "\nFznException: " << e.what();
  }
}

void test_fzn_model(const std::string& path) {
  parser::Model resModel{
      std::vector<parser::PredicateItem>{}, std::vector<parser::ParDeclItem>{},
      std::vector<parser::VarDeclItem>{}, std::vector<parser::ConstraintItem>{},
      parser::SolveItem{
          parser::SolveSatisfy{std::vector<parser::Annotation>{}}}};
  std::ifstream input_file(path);
  EXPECT_TRUE(input_file.is_open()) << "Could not open file: " << path;
  boost::spirit::istream_iterator file_iterator(input_file >> std::noskipws),
      eof;
  EXPECT_TRUE(x3::phrase_parse(file_iterator, eof, parser::model,
                               x3::standard::space, resModel))
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
    std::cout << buffer << std::endl;
    FAIL() << path;
  }
  input_file.close();
  try {
    ModelTransformer transformer(std::move(resModel));
    fznparser::Model model = transformer.generateModel();
    const auto fznData = get_fzn_data(path);

    EXPECT_EQ(model.vars().size(), fznData.numVars) << path;
    EXPECT_EQ(model.constraints().size(), fznData.numConstraints) << path;
    EXPECT_EQ(model.isSatisfactionProblem(), fznData.isSatisfactionProblem)
        << path;
  } catch (const FznException& e) {
    EXPECT_TRUE(false) << "could not transform model: " << path
                       << "\nFznException: " << e.what();
  }
}

TEST(mzn_challenge, crashing) {
  std::vector<std::string> fznModels;
  fznModels.reserve(modelsThatCrash.size());
  for (const auto& fznModel : modelsThatCrash) {
    fznModels.push_back(fznModel);
  }
  std::sort(fznModels.begin(), fznModels.end());

  size_t i = 0;
  for (const auto& fznModel : fznModels) {
    if (!std::filesystem::exists(fznModel)) {
      EXPECT_TRUE(false) << fznModel;
    } else {
      logModelName(fznModel, false, i, fznModels.size());
      test_fzn_model(fznModel);
    }
    ++i;
  }
}

TEST(mzn_challenge, failing) {
  std::vector<std::string> fznModels;
  fznModels.reserve(modelsThatFail.size());
  for (const auto& fznModel : modelsThatFail) {
    fznModels.push_back(fznModel);
  }
  std::sort(fznModels.begin(), fznModels.end());

  size_t i = 0;
  for (const auto& fznModel : fznModels) {
    if (!std::filesystem::exists(fznModel)) {
      EXPECT_TRUE(false) << fznModel;
    } else {
      logModelName(fznModel, false, i, fznModels.size());
      test_fzn_model_breakdown(fznModel);
    }
    ++i;
  }
}

TEST(mzn_challenge, parse) {
  std::stack<std::string> stack;
  stack.emplace(FZN_CHALLENGE_DIR);
  std::unordered_set<std::string> visited{std::string(FZN_CHALLENGE_DIR)};
  std::vector<std::string> fznModels;

  while (!stack.empty()) {
    const std::string dir = stack.top();
    stack.pop();
    if (!std::filesystem::exists(dir)) {
      continue;
    }
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
      if (entry.is_directory()) {
        if (!visited.contains(entry.path().string())) {
          stack.push(entry.path().string());
          visited.insert(entry.path().string());
        }
      } else if (entry.is_regular_file() &&
                 entry.path().extension() == ".fzn") {
        fznModels.push_back(entry.path().string());
      }
    }
  }

  std::sort(fznModels.begin(), fznModels.end());
  for (size_t i = 1; i < fznModels.size(); ++i) {
    bool skipping = modelsThatCrash.contains(fznModels.at(i)) ||
                    modelsThatFail.contains(fznModels.at(i));
    logModelName(fznModels.at(i), skipping, i, fznModels.size());
    if (!skipping) {
      test_fzn_model(fznModels.at(i));
    }
  }
}

}  // namespace fznparser::testing