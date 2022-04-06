#include <gtest/gtest.h>

#include "fznparser/modelFactory.hpp"

using namespace fznparser;

TEST(ModelFactoryTest, empty_satisfy_is_parsed) {
  auto model = ModelFactory::create(STUB_DIR "/satisfy-empty.fzn");

  EXPECT_TRUE(std::holds_alternative<fznparser::Satisfy>(model.objective()));
}

TEST(ModelFactoryTest, parameters_are_parsed) {
  auto model = ModelFactory::create(STUB_DIR "/parameters.fzn");

  EXPECT_EQ(model.parameters().size(), 7);

  Parameter expectedInt{"n", Int(4)};
  EXPECT_EQ(model.parameters()[0], expectedInt);

  Parameter expectedBoolF{"bF", false};
  EXPECT_EQ(model.parameters()[1], expectedBoolF);
  Parameter expectedBoolT{"bT", true};
  EXPECT_EQ(model.parameters()[2], expectedBoolT);

  Parameter intArray{"nums", std::vector<Value>{Int(1), Int(2), Int(3), Int(4)}};
  EXPECT_EQ(model.parameters()[3], intArray);
  Parameter boolsArray{"bools", std::vector<Value>{false, false, true}};
  EXPECT_EQ(model.parameters()[4], boolsArray);

  Parameter explicitSet{"explicitSet", Set(std::vector<Int>{Int(1), Int(4), Int(5)})};
  EXPECT_EQ(model.parameters()[5], explicitSet);
  Parameter intervalSet{"intervalSet", IntRange{1, 10}};
  EXPECT_EQ(model.parameters()[6], intervalSet);
}

TEST(ModelFactoryTest, variables_are_parsed) {
  auto model = ModelFactory::create(STUB_DIR "/variables.fzn");

  EXPECT_EQ(model.variables().size(), 6);

  SearchVariable v1{"v1", BasicDomain<bool>{}, {}, {}};
  EXPECT_EQ(model.variables()[0], Variable(v1));

  SearchVariable v2{"v2", IntRange{0, 5}, {}, {}};
  EXPECT_EQ(model.variables()[1], Variable(v2));

  SearchVariable v3{"v3", std::vector<Int>{3, 5, 10}, {}, {}};
  EXPECT_EQ(model.variables()[2], Variable(v3));

  SearchVariable v4{"v4", IntRange{1, 5}, {}, Int(5)};
  EXPECT_EQ(model.variables()[3], Variable(v4));

  SearchVariable v5{"v5", IntRange{1, 5}, {}, Identifier("n")};
  EXPECT_EQ(model.variables()[4], Variable(v5));

  SearchVariable v6{"v6", BasicDomain<Int>{}, {}, {}};
  EXPECT_EQ(model.variables()[5], Variable(v6));
}

TEST(ModelFactoryTest, variable_arrays_are_parsed) {
  auto model = ModelFactory::create(STUB_DIR "/variable-arrays.fzn");

  EXPECT_EQ(model.variables().size(), 5);

  VariableArray array1{"array1", {Identifier("v1"), Int(2), Identifier("v2"), Int(5)}, {}};
  EXPECT_EQ(model.variables()[3], Variable(array1));

  VariableArray array2{"array2", {Identifier("v3"), true, false}, {}};
  EXPECT_EQ(model.variables()[4], Variable(array2));
}

TEST(ModelFactoryTest, constraints_are_parsed) {
  auto model = ModelFactory::create(STUB_DIR "/constraints.fzn");

  EXPECT_EQ(model.constraints().size(), 1);

  Constraint intLinEq{
      "int_lin_eq", {Identifier("coeffs"), Array{Identifier("v1"), Identifier("v2")}, Int(2)}, {}};
  EXPECT_EQ(model.constraints()[0], intLinEq);
}
