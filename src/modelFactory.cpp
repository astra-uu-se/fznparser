#include "fznparser/modelFactory.hpp"

#include <map>
#include <string>

#include "FlatZincLexer.h"
#include "FlatZincParser.h"
#include "fznVisitor.hpp"

std::unique_ptr<fznparser::Model> fznparser::ModelFactory::create(
    const std::filesystem::path& modelFile) {
  std::ifstream stream;
  stream.open(modelFile);

  if (!stream.good()) {
    throw std::invalid_argument("Failed to read file '" + modelFile.string() + "'.");
  }

  antlr4::ANTLRInputStream input(stream);

  fznparser::core::FlatZincLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);

  fznparser::core::FlatZincParser parser(&tokens);
  FlatZincParser::ModelContext* tree = parser.model();

  FznVisitor visitor;
  fznparser::Model* model = visitor.visitModel(tree).as<fznparser::Model*>();

  return std::unique_ptr<fznparser::Model>(model);
}
