#pragma once

#include "FlatZincBaseVisitor.h"

using namespace fznparser::core;

class FznVisitor : FlatZincBaseVisitor {
public:
  antlrcpp::Any visitModel(FlatZincParser::ModelContext *ctx) override;
  antlrcpp::Any visitParDeclItem(FlatZincParser::ParDeclItemContext *ctx) override;
  antlrcpp::Any visitParExpr(FlatZincParser::ParExprContext *ctx) override;
  antlrcpp::Any visitBasicLiteralExpr(FlatZincParser::BasicLiteralExprContext *ctx) override;
  antlrcpp::Any visitBoolLiteral(FlatZincParser::BoolLiteralContext *ctx) override;
  antlrcpp::Any visitIntLiteral(FlatZincParser::IntLiteralContext *ctx) override;
  antlrcpp::Any visitSetLiteral(FlatZincParser::SetLiteralContext *ctx) override;
  antlrcpp::Any visitExplicitIntSet(FlatZincParser::ExplicitIntSetContext *ctx) override;
  antlrcpp::Any visitIntInterval(FlatZincParser::IntIntervalContext *ctx) override;
  antlrcpp::Any visitParArrayLiteral(FlatZincParser::ParArrayLiteralContext *ctx) override;
};
