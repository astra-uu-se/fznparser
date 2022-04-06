#pragma once

#include "FlatZincBaseVisitor.h"

using namespace fznparser::core;

class FznVisitor : FlatZincBaseVisitor {
public:
  antlrcpp::Any visitModel(FlatZincParser::ModelContext *ctx) override;

  antlrcpp::Any visitParDeclItem(FlatZincParser::ParDeclItemContext *ctx) override;
  antlrcpp::Any visitParArrayLiteral(FlatZincParser::ParArrayLiteralContext *ctx) override;
  antlrcpp::Any visitParExpr(FlatZincParser::ParExprContext *ctx) override;

  antlrcpp::Any visitVarDeclItem(FlatZincParser::VarDeclItemContext *ctx) override;
  antlrcpp::Any visitBasicVarType(FlatZincParser::BasicVarTypeContext *ctx) override;
  antlrcpp::Any visitBasicExpr(FlatZincParser::BasicExprContext *ctx) override;

  antlrcpp::Any visitArrayLiteral(FlatZincParser::ArrayLiteralContext *ctx) override;

  antlrcpp::Any visitConstraintItem(FlatZincParser::ConstraintItemContext *ctx) override;
  antlrcpp::Any visitExpr(FlatZincParser::ExprContext *ctx) override;

  antlrcpp::Any visitAnnotations(FlatZincParser::AnnotationsContext *ctx) override;
  antlrcpp::Any visitAnnotation(FlatZincParser::AnnotationContext *ctx) override;

  antlrcpp::Any visitBasicLiteralExpr(FlatZincParser::BasicLiteralExprContext *ctx) override;
  antlrcpp::Any visitBoolLiteral(FlatZincParser::BoolLiteralContext *ctx) override;
  antlrcpp::Any visitIntLiteral(FlatZincParser::IntLiteralContext *ctx) override;
  antlrcpp::Any visitSetLiteral(FlatZincParser::SetLiteralContext *ctx) override;
  antlrcpp::Any visitSet(FlatZincParser::SetContext *ctx) override;
  antlrcpp::Any visitIntRange(FlatZincParser::IntRangeContext *ctx) override;
};
