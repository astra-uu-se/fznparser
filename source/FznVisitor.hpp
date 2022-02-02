#pragma once

#include <map>
#include <memory>

#include "FlatZincBaseVisitor.h"
#include "fznparser/model.hpp"

using namespace fznparser::core;

class FznVisitor : FlatZincBaseVisitor {
private:
  std::map<std::string_view, std::shared_ptr<fznparser::Variable>> _variableMap;

public:
  antlrcpp::Any visitModel(FlatZincParser::ModelContext *ctx) override;
  antlrcpp::Any visitVarDeclItem(FlatZincParser::VarDeclItemContext *ctx) override;
  antlrcpp::Any visitParDeclItem(FlatZincParser::ParDeclItemContext *ctx) override;
  antlrcpp::Any visitConstraintItem(FlatZincParser::ConstraintItemContext *ctx) override;
  antlrcpp::Any visitBasicVarType(FlatZincParser::BasicVarTypeContext *ctx) override;
  antlrcpp::Any visitAnnotations(FlatZincParser::AnnotationsContext *ctx) override;
  antlrcpp::Any visitAnnotation(FlatZincParser::AnnotationContext *ctx) override;
  antlrcpp::Any visitExpr(FlatZincParser::ExprContext *ctx) override;
  antlrcpp::Any visitArrayLiteral(FlatZincParser::ArrayLiteralContext *ctx) override;
  antlrcpp::Any visitParArrayLiteral(FlatZincParser::ParArrayLiteralContext *ctx) override;
  // antlrcpp::Any visitArrayVarType(FlatZincParser::ArrayVarTypeContext *ctx);
};
