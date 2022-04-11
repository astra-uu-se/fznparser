#pragma once

#include <optional>

#include "FlatZincBaseVisitor.h"

using namespace fznparser::core;

class FznVisitor : FlatZincBaseVisitor {
private:
  enum class ParameterType { INT, BOOL, INT_ARRAY, BOOL_ARRAY, SET_OF_INT, SET_OF_INT_ARRAY };

  // Unknown type is used when parsing array literals in constraint arguments, when we do not know
  // the type of the variable.
  enum class VariableType { INT, BOOL };

  std::optional<ParameterType> _parsingParameterType;
  std::optional<VariableType> _parsingVariableType;

public:
  antlrcpp::Any visitModel(FlatZincParser::ModelContext *ctx) override;
  antlrcpp::Any visitSolveItem(FlatZincParser::SolveItemContext *ctx) override;

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

private:
  static VariableType determineVariableType(FlatZincParser::BasicVarTypeContext *ctx);
  static ParameterType determineParameterType(FlatZincParser::ParTypeContext *ctx);
};
