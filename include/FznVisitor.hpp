#ifndef __FZN_VISITOR_H_INCLUDED__
#define __FZN_VISITOR_H_INCLUDED__

#include "FlatZincBaseVisitor.h"
#include "model.hpp"

using namespace fznparser::core;

class FznVisitor : FlatZincBaseVisitor {
private:
  Model *model;

public:
  antlrcpp::Any visitModel(FlatZincParser::ModelContext *ctx);
  antlrcpp::Any visitVarDeclItem(FlatZincParser::VarDeclItemContext *ctx);
  antlrcpp::Any visitParDeclItem(FlatZincParser::ParDeclItemContext *ctx);
  antlrcpp::Any visitConstraintItem(FlatZincParser::ConstraintItemContext *ctx);
  antlrcpp::Any visitBasicVarType(FlatZincParser::BasicVarTypeContext *ctx);
  antlrcpp::Any visitAnnotations(FlatZincParser::AnnotationsContext *ctx);
  antlrcpp::Any visitAnnotation(FlatZincParser::AnnotationContext *ctx);
  antlrcpp::Any visitExpr(FlatZincParser::ExprContext *ctx);
  antlrcpp::Any visitArrayLiteral(FlatZincParser::ArrayLiteralContext *ctx);
  antlrcpp::Any visitParArrayLiteral(FlatZincParser::ParArrayLiteralContext *ctx);
  // antlrcpp::Any visitArrayVarType(FlatZincParser::ArrayVarTypeContext *ctx);
};

#endif