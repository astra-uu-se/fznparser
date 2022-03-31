#include <gtest/gtest.h>

#include <limits>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "fznparser/modelfactory.hpp"

using namespace fznparser;

TEST(ModelFactoryTest, parsing_of_variables) {
    auto model = ModelFactory::create(STUB_DIR "/test-model-1.fzn");
    const auto &variables = model->variables();

    EXPECT_EQ(variables.size(), 3);
    EXPECT_EQ(variables[0]->type(), LiteralType::SEARCH_VARIABLE);
    EXPECT_EQ(variables[1]->type(), LiteralType::SEARCH_VARIABLE);

    std::shared_ptr<SearchVariable> var1 = std::dynamic_pointer_cast<SearchVariable>(variables[0]);
    std::shared_ptr<SearchVariable> var2 = std::dynamic_pointer_cast<SearchVariable>(variables[1]);

    EXPECT_EQ(var1->domain()->type(), DomainType::INT);
    EXPECT_EQ(var2->domain()->type(), DomainType::INT);

    EXPECT_EQ(dynamic_cast<IntDomain *>(var1->domain())->lowerBound(), 1);
    EXPECT_EQ(dynamic_cast<IntDomain *>(var2->domain())->lowerBound(), 1);
    EXPECT_EQ(dynamic_cast<IntDomain *>(var1->domain())->upperBound(), 3);
    EXPECT_EQ(dynamic_cast<IntDomain *>(var2->domain())->upperBound(), 3);
}

TEST(ModelFactoryTest, parsing_of_array_variables) {
    auto model = ModelFactory::create(STUB_DIR "/test-model-1.fzn");
    const auto &variables = model->variables();
    EXPECT_EQ(variables[2]->type(), LiteralType::VARIABLE_ARRAY);

    std::shared_ptr<VariableArray> arrayVar = std::dynamic_pointer_cast<VariableArray>(variables[2]);
    EXPECT_EQ(arrayVar->size(), 2);
    EXPECT_EQ(arrayVar->contents()[0], variables[0]);
    EXPECT_EQ(arrayVar->contents()[1], variables[1]);

    EXPECT_EQ(arrayVar->annotations().size(), 1);
    EXPECT_TRUE(arrayVar->annotations().has<OutputArrayAnnotation>());
}

TEST(ModelFactoryTest, parsing_of_constraints) {
    auto model = ModelFactory::create(STUB_DIR "/test-model-1.fzn");
    const std::vector<std::shared_ptr<Constraint>> &constraints = model->constraints();

    EXPECT_EQ(constraints.size(), 1);
    EXPECT_EQ(constraints[0]->name(), "int_lt");

    const std::vector<ConstraintArgument> &arguments = constraints[0]->arguments();

    EXPECT_EQ(arguments.size(), 2);
}

TEST(ModelFactoryTest, parsing_of_objective) {
    auto model = ModelFactory::create(STUB_DIR "/test-model-1.fzn");
    EXPECT_EQ(model->objective(), Objective::SATISFY);
    EXPECT_FALSE(model->objective_value());

    model = ModelFactory::create(STUB_DIR "/test-model-2.fzn");
    EXPECT_EQ(model->objective(), Objective::MINIMIZE);
    EXPECT_TRUE(model->objective_value());

    const auto &variables = model->variables();
    auto variablePos = std::find(variables.begin(), variables.end(), *model->objective_value());
    EXPECT_NE(variablePos, std::end(variables));
}

TEST(ModelFactoryTest, parsing_of_parameters) {
    auto model = ModelFactory::create(STUB_DIR "/test-model-3.fzn");

    const auto &parameters = model->parameters();
    EXPECT_EQ(parameters.size(), 3);

    auto param1 = std::get<std::shared_ptr<ParameterArray>>(parameters[0]);
    auto param2 = std::get<std::shared_ptr<ParameterArray>>(parameters[1]);
    auto param3 = std::get<std::shared_ptr<ParameterArray>>(parameters[2]);

    EXPECT_EQ(param1->name(), "Size");
    EXPECT_EQ(param2->name(), "X_INTRODUCED_30_");
    EXPECT_EQ(param3->name(), "X_INTRODUCED_33_");

    EXPECT_EQ(param1->size(), 6);
    EXPECT_EQ(param2->size(), 2);
    EXPECT_EQ(param3->size(), 2);

    for (int64_t i = 1; i <= 6; i++) EXPECT_EQ(param1->contents()[i - 1], i);

    EXPECT_EQ(param2->contents()[0], 1);
    EXPECT_EQ(param2->contents()[1], -1);

    EXPECT_EQ(param3->contents()[0], 2);
    EXPECT_EQ(param3->contents()[1], -1);
}

TEST(ModelFactoryTest, parsing_of_annotations) {
    auto model = ModelFactory::create(STUB_DIR "/test-model-4.fzn");

    auto constraint = model->constraints()[0];

    EXPECT_TRUE(constraint->annotations().has<DefinesVarAnnotation>());

    auto ann = constraint->annotations().get<DefinesVarAnnotation>();
    EXPECT_EQ(model->variables()[2], ann->defines().lock());
}

TEST(ModelFactoryTest, unbounded_int_domain) {
    auto model = ModelFactory::create(STUB_DIR "/test-model-5.fzn");

    auto variable = std::dynamic_pointer_cast<fznparser::SearchVariable>(model->variables()[0]);
    EXPECT_EQ(variable->domain()->lowerBound(), std::numeric_limits<int64_t>::min());
    EXPECT_EQ(variable->domain()->upperBound(), std::numeric_limits<int64_t>::max());
}

TEST(ModelFactoryTest, multi_dimensional_output_array) {
    auto model = ModelFactory::create(STUB_DIR "/multi-dimensional-array.fzn");

    auto variable = std::dynamic_pointer_cast<fznparser::VariableArray>(
            model->variables()[model->variables().size() - 1]);

    EXPECT_TRUE(variable->annotations().has<fznparser::OutputArrayAnnotation>());

    auto ann = variable->annotations().get<fznparser::OutputArrayAnnotation>();
    EXPECT_EQ(ann->dimensions().size(), 2);
    EXPECT_EQ(ann->dimensions()[0], 4);
    EXPECT_EQ(ann->dimensions()[1], 8);
}
