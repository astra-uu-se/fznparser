#include "fznparser/modelFactory.hpp"

#include <map>
#include <string>

#include "FlatZincLexer.h"
#include "FlatZincParser.h"
#include "fznVisitor.hpp"

fznparser::FZNModel fznparser::ModelFactory::create(const std::filesystem::path& modelFile) {
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
  return std::move(std::any_cast<fznparser::FZNModel>(visitor.visitModel(tree)));
}
