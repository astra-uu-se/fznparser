#include "fznVisitor.hpp"

#include <fznparser/model.hpp>

using namespace fznparser;

antlrcpp::Any FznVisitor::visitModel(FlatZincParser::ModelContext *) { return FZNModel(Satisfy{}); }
