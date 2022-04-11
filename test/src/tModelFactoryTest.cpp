#include <gtest/gtest.h>

#include "fznparser/modelFactory.hpp"

using namespace fznparser;

TEST(ModelFactoryTest, empty_satisfy_is_parsed) {
  auto model = ModelFactory::create(STUB_DIR "/satisfy-empty.fzn");

  EXPECT_EQ(model.objective(), Objective(Satisfy{}));
}

TEST(ModelFactoryTest, parameters_are_parsed) {
  auto model = ModelFactory::create(STUB_DIR "/parameters.fzn");

  EXPECT_EQ(model.parameters().size(), 8);

  IntParameter expectedInt{"n", Int(4)};
  EXPECT_EQ(model.parameters()[0], Parameter(expectedInt));

  BoolParameter expectedBoolF{"bF", false};
  EXPECT_EQ(model.parameters()[1], Parameter(expectedBoolF));
  BoolParameter expectedBoolT{"bT", true};
  EXPECT_EQ(model.parameters()[2], Parameter(expectedBoolT));

  ArrayOfIntParameter intArray{"nums", {Int(1), Int(2), Int(3), Int(4)}};
  EXPECT_EQ(model.parameters()[3], Parameter(intArray));
  ArrayOfBoolParameter boolsArray{"bools", {false, false, true}};
  EXPECT_EQ(model.parameters()[4], Parameter(boolsArray));

  SetOfIntParameter explicitSet{"explicitSet", LiteralSet<Int>{Int(1), Int(4), Int(5)}};
  EXPECT_EQ(model.parameters()[5], Parameter(explicitSet));
  SetOfIntParameter intervalSet{"intervalSet", IntRange{1, 10}};
  EXPECT_EQ(model.parameters()[6], Parameter(intervalSet));

  ArrayOfSetOfIntParameter arrayOfSets{
      "sets", {LiteralSet<Int>{Int(4), Int(50), Int(55)}, IntRange{1, 10}}};
  EXPECT_EQ(model.parameters()[7], Parameter(arrayOfSets));
}

TEST(ModelFactoryTest, variables_are_parsed) {
  auto model = ModelFactory::create(STUB_DIR "/variables.fzn");

  EXPECT_EQ(model.variables().size(), 6);

  BoolVariable v1{"v1", BasicDomain<bool>{}, {}, {}};
  EXPECT_EQ(model.variables()[0], Variable(v1));

  IntVariable v2{"v2", IntRange{0, 5}, {}, {}};
  EXPECT_EQ(model.variables()[1], Variable(v2));

  IntVariable v3{"v3", LiteralSet<Int>{3, 5, 10}, {}, {}};
  EXPECT_EQ(model.variables()[2], Variable(v3));

  IntVariable v4{"v4", IntRange{1, 5}, {}, Int(5)};
  EXPECT_EQ(model.variables()[3], Variable(v4));

  IntVariable v5{"v5", IntRange{1, 5}, {}, Identifier("n")};
  EXPECT_EQ(model.variables()[4], Variable(v5));

  IntVariable v6{"v6", BasicDomain<Int>{}, {}, {}};
  EXPECT_EQ(model.variables()[5], Variable(v6));
}

TEST(ModelFactoryTest, variable_arrays_are_parsed) {
  auto model = ModelFactory::create(STUB_DIR "/variable-arrays.fzn");

  EXPECT_EQ(model.variables().size(), 5);

  IntVariableArray array1{"array1", {Identifier("v1"), Int(2), Identifier("v2"), Int(5)}, {}};
  EXPECT_EQ(model.variables()[3], Variable(array1));

  BoolVariableArray array2{"array2", {Identifier("v3"), true, false}, {}};
  EXPECT_EQ(model.variables()[4], Variable(array2));
}

TEST(ModelFactoryTest, constraints_are_parsed) {
  auto model = ModelFactory::create(STUB_DIR "/constraints.fzn");

  EXPECT_EQ(model.constraints().size(), 2);

  Constraint intLinEq{
      "int_lin_eq",
      {Identifier("coeffs"), Constraint::ArrayArgument{Identifier("v1"), Identifier("v2")}, Int(2)},
      {}};
  EXPECT_EQ(model.constraints()[0], intLinEq);

  Constraint setIn{"set_in", {Identifier("v1"), Set<Int>(LiteralSet<Int>{Int(1), Int(4)})}, {}};
  EXPECT_EQ(model.constraints()[1], setIn);
}

TEST(ModelFactoryTest, annotations_are_recognised) {
  auto model = ModelFactory::create(STUB_DIR "/annotations.fzn");

  EXPECT_EQ(model.variables().size(), 4);

  IntVariable a{"a", BasicDomain<Int>{}, {TagAnnotation{"output_var"}}, {}};
  EXPECT_EQ(model.variables()[0], Variable(a));

  IntVariableArray arr{"arr",
                       {Identifier("b"), Identifier("c"), Identifier("c"), Identifier("d")},
                       {OutputArrayAnnotation{{2, 2}}}};
  EXPECT_EQ(model.variables()[3], Variable(arr));

  EXPECT_EQ(model.constraints().size(), 1);

  Constraint plus{"int_plus",
                  {Identifier("a"), Identifier("b"), Identifier("c")},
                  {DefinesVariableAnnotation{Identifier("c")}}};
  EXPECT_EQ(model.constraints()[0], plus);
}

TEST(ModelFactoryTest, minimise_objective) {
  auto model = ModelFactory::create(STUB_DIR "/minimise-objective.fzn");

  EXPECT_EQ(model.objective(), Objective(Minimise{Identifier("a")}));
}

TEST(ModelFactoryTest, maximise_objective) {
  auto model = ModelFactory::create(STUB_DIR "/maximise-objective.fzn");

  EXPECT_EQ(model.objective(), Objective(Maximise{Identifier("a")}));
}
