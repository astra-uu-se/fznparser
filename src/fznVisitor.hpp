#pragma once

#include "FlatZincBaseVisitor.h"

using namespace fznparser::core;

class FznVisitor : FlatZincBaseVisitor {
public:
  antlrcpp::Any visitModel(FlatZincParser::ModelContext *ctx) override;
};
