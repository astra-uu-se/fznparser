#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "modelfactory.hpp"

class ModelFactoryTest : public ::testing::Test {
private:
  inline static const std::string& file = STUB_DIR "/test-model.fzn";

protected:
  std::unique_ptr<Model> model;

  void SetUp() { model = fznparser::ModelFactory::create(file); }
};

TEST_F(ModelFactoryTest, parsing_of_variables) {
  const std::vector<Variable*>& variables = model->varMap().variables();

  EXPECT_EQ(variables.size(), 2);
  EXPECT_EQ(variables[0]->lowerBound(), 1);
  EXPECT_EQ(variables[1]->lowerBound(), 1);
  EXPECT_EQ(variables[0]->upperBound(), 3);
  EXPECT_EQ(variables[1]->upperBound(), 3);
}

TEST_F(ModelFactoryTest, parsing_of_constraints) {
  const std::vector<Constraint*>& constraints = model->constraints();
  EXPECT_EQ(constraints.size(), 1);
}
