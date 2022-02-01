#include "fznparser/modelfactory.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "FlatZincLexer.h"
#include "FlatZincParser.h"
#include "FznVisitor.hpp"
#include "model.hpp"

using InternalModel = Model;
using InternalConstraint = Constraint;
using InternalVariable = Variable;

static std::unique_ptr<fznparser::Model> convertRepresentation(InternalModel* model);

std::unique_ptr<fznparser::Model> fznparser::ModelFactory::create(
    const std::filesystem::path& modelFile) {
  std::ifstream stream;
  stream.open(modelFile);

  if (!stream.good()) {
    throw "Failed to read file '" + modelFile.string() + "'.";
  }

  antlr4::ANTLRInputStream input(stream);

  fznparser::core::FlatZincLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);

  fznparser::core::FlatZincParser parser(&tokens);
  FlatZincParser::ModelContext* tree = parser.model();

  FznVisitor visitor;
  InternalModel* model = visitor.visitModel(tree).as<InternalModel*>();

  return convertRepresentation(model);
}

static std::shared_ptr<fznparser::Variable> convertVariable(InternalVariable* var) {
  std::string name = var->getName();

  // TODO: Add correct annotations.
  std::vector<std::shared_ptr<fznparser::Annotation>> annotations;

  // TODO: Different types of domains.
  auto domain = std::make_unique<fznparser::IntDomain>(var->lowerBound(), var->upperBound());

  return std::make_shared<fznparser::SearchVariable>(name, annotations, std::move(domain));
}

static std::shared_ptr<fznparser::Constraint> convertConstraint(
    const std::map<std::string_view, std::shared_ptr<fznparser::Variable>>& varsByName,
    InternalConstraint* cons) {
  if (!cons->isFunctional()) {
    std::vector<std::shared_ptr<fznparser::Variable>> vars;
    vars.reserve(cons->variables().size());
    for (Variable* var : cons->variables()) {
      std::string_view name = var->getName();
      vars.push_back(varsByName.at(name));
    }

    return std::make_shared<fznparser::NonFunctionalConstraint>(cons->getName(), vars);
  }

  std::vector<InternalVariable*> inputs;
  std::copy_if(cons->variables().begin(), cons->variables().end(), inputs.begin(),
               [cons](auto var) { return !cons->annDefines(var); });
  std::vector<InternalVariable*> outputs;
  std::copy_if(cons->variables().begin(), cons->variables().end(), outputs.begin(),
               [cons](auto var) { return cons->annDefines(var); });

  std::vector<std::shared_ptr<fznparser::Variable>> convertedInputs;
  convertedInputs.reserve(inputs.size());
  for (Variable* var : inputs) {
    std::string_view name = var->getName();
    convertedInputs.push_back(varsByName.at(name));
  }

  std::vector<std::shared_ptr<fznparser::Variable>> convertedOutputs;
  convertedOutputs.reserve(outputs.size());
  for (Variable* var : outputs) {
    std::string_view name = var->getName();
    convertedOutputs.push_back(varsByName.at(name));
  }

  return std::make_shared<fznparser::FunctionalConstraint>(cons->getName(), convertedInputs,
                                                           convertedOutputs);
}

std::unique_ptr<fznparser::Model> convertRepresentation(InternalModel* model) {
  // Convert all variables.
  std::vector<std::shared_ptr<fznparser::Variable>> convertedVariables;
  convertedVariables.reserve(model->varMap().variables().size());
  for (Variable* var : model->varMap().variables())
    convertedVariables.push_back(convertVariable(var));

  // Build a lookup table by variable name.
  std::map<std::string_view, std::shared_ptr<fznparser::Variable>> varsByName;
  std::transform(convertedVariables.begin(), convertedVariables.end(),
                 std::inserter(varsByName, varsByName.end()),
                 [](std::shared_ptr<fznparser::Variable> var) {
                   std::string_view key = var->name();
                   std::shared_ptr<fznparser::Variable> value = var;

                   return std::make_pair(key, value);
                 });

  // Convert all constraints.
  std::vector<std::shared_ptr<fznparser::Constraint>> convertedConstraints;
  convertedConstraints.reserve(model->constraints().size());
  for (auto constraint : model->constraints())
    convertedConstraints.push_back(convertConstraint(varsByName, constraint));

  return std::make_unique<fznparser::Model>(convertedVariables, convertedConstraints);
}
