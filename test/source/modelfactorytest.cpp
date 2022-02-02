#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <vector>

#include "fznparser/modelfactory.hpp"

using namespace fznparser;

class ModelFactoryTest : public ::testing::Test {
private:
  inline static const std::string& file = STUB_DIR "/test-model.fzn";

protected:
  std::unique_ptr<Model> model;

  void SetUp() { model = ModelFactory::create(file); }
};

TEST_F(ModelFactoryTest, parsing_of_variables) {
  const auto& variables = model->variables();

  EXPECT_EQ(variables.size(), 3);
  EXPECT_FALSE(variables[0]->isArray());
  EXPECT_FALSE(variables[1]->isArray());

  std::shared_ptr<SearchVariable> var1 = std::dynamic_pointer_cast<SearchVariable>(variables[0]);
  std::shared_ptr<SearchVariable> var2 = std::dynamic_pointer_cast<SearchVariable>(variables[1]);

  EXPECT_EQ(var1->domain()->type(), DomainType::INT);
  EXPECT_EQ(var2->domain()->type(), DomainType::INT);

  EXPECT_EQ(dynamic_cast<IntDomain*>(var1->domain())->lowerBound(), 1);
  EXPECT_EQ(dynamic_cast<IntDomain*>(var2->domain())->lowerBound(), 1);
  EXPECT_EQ(dynamic_cast<IntDomain*>(var1->domain())->upperBound(), 3);
  EXPECT_EQ(dynamic_cast<IntDomain*>(var2->domain())->upperBound(), 3);
}

TEST_F(ModelFactoryTest, parsing_of_array_variables) {
  const auto& variables = model->variables();
  EXPECT_TRUE(variables[2]->isArray());

  std::shared_ptr<ArrayVariable> arrayVar = std::dynamic_pointer_cast<ArrayVariable>(variables[2]);
  EXPECT_EQ(arrayVar->size(), 2);
  EXPECT_EQ(arrayVar->contents()[0], variables[0]);
  EXPECT_EQ(arrayVar->contents()[1], variables[1]);

  EXPECT_EQ(arrayVar->annotations().size(), 1);
  EXPECT_EQ(arrayVar->annotations()[0]->type(), AnnotationType::OUTPUT);
}

TEST_F(ModelFactoryTest, parsing_of_constraints) {
  const std::vector<std::shared_ptr<Constraint>>& constraints = model->constraints();

  EXPECT_EQ(constraints.size(), 1);
  EXPECT_FALSE(constraints[0]->isFunctional());
  EXPECT_EQ(constraints[0]->name(), "int_lt");

  const std::vector<std::shared_ptr<Variable>>& variables
      = std::dynamic_pointer_cast<NonFunctionalConstraint>(constraints[0])->variables();

  EXPECT_EQ(variables.size(), 2);
}
