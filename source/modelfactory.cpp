#include "modelfactory.hpp"

#include <fstream>
#include <iostream>

#include "FlatZincLexer.h"
#include "FlatZincParser.h"
#include "FznVisitor.hpp"

std::unique_ptr<Model> fznparser::ModelFactory::create(const std::filesystem::path& modelFile) {
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
  return std::unique_ptr<Model>(visitor.visitModel(tree).as<Model*>());
}