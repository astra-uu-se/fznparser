#include "fznVisitor.hpp"

#include <fznparser/model.hpp>

using namespace fznparser;

static Identifier createIdentifier(antlr4::tree::TerminalNode* node) {
  return Identifier(node->getText());
}

antlrcpp::Any FznVisitor::visitModel(FlatZincParser::ModelContext* ctx) {
  std::vector<Parameter> parameters;
  for (auto& parDeclItem : ctx->parDeclItem()) {
    parameters.push_back(parDeclItem->accept(this).as<Parameter>());
  }

  return FZNModel(std::move(parameters), Satisfy{});
}

antlrcpp::Any FznVisitor::visitParDeclItem(FlatZincParser::ParDeclItemContext* ctx) {
  auto identifier = createIdentifier(ctx->Identifier());
  auto value = ctx->parExpr()->accept(this).as<Parameter::ParamValue>();

  return Parameter{identifier, value};
}

antlrcpp::Any FznVisitor::visitParExpr(FlatZincParser::ParExprContext* ctx) {
  if (ctx->basicLiteralExpr()) {
    return Parameter::ParamValue(ctx->basicLiteralExpr()->accept(this).as<Value>());
  } else if (ctx->parArrayLiteral()) {
    return Parameter::ParamValue(ctx->parArrayLiteral()->accept(this).as<std::vector<Value>>());
  } else {
    throw std::runtime_error("Unhandled variant in parExpr.");
  }
}

antlrcpp::Any FznVisitor::visitBasicLiteralExpr(FlatZincParser::BasicLiteralExprContext* ctx) {
  if (ctx->boolLiteral()) {
    return ctx->boolLiteral()->accept(this);
  } else if (ctx->intLiteral()) {
    return ctx->intLiteral()->accept(this);
  } else if (ctx->setLiteral()) {
    return ctx->setLiteral()->accept(this);
  } else {
    throw std::runtime_error("Unhandled variant in basicLiteralExpr.");
  }
}

antlrcpp::Any FznVisitor::visitBoolLiteral(FlatZincParser::BoolLiteralContext* ctx) {
  std::string text = ctx->getText();

  if (text == "true") {
    return Value(true);
  } else if (text == "false") {
    return Value(false);
  } else {
    throw std::runtime_error("Unhandled bool literal.");
  }
}

antlrcpp::Any FznVisitor::visitIntLiteral(FlatZincParser::IntLiteralContext* ctx) {
  return Value(std::stol(ctx->getText()));
}

antlrcpp::Any FznVisitor::visitSetLiteral(FlatZincParser::SetLiteralContext* ctx) {
  if (ctx->explicitIntSet()) {
    return Value(ctx->explicitIntSet()->accept(this).as<Set>());
  } else if (ctx->intInterval()) {
    return Value(ctx->intInterval()->accept(this).as<Set>());
  } else {
    throw std::runtime_error("Unhandled variant in setLiteral.");
  }
}

antlrcpp::Any FznVisitor::visitExplicitIntSet(FlatZincParser::ExplicitIntSetContext* ctx) {
  std::vector<Int> values;
  values.reserve(ctx->intLiteral().size());

  for (auto& literal : ctx->intLiteral()) {
    values.push_back(std::stol(literal->getText()));
  }

  return Set(values);
}

antlrcpp::Any FznVisitor::visitIntInterval(FlatZincParser::IntIntervalContext* ctx) {
  assert(ctx->intLiteral().size() == 2);

  Int lowerBound = std::stol(ctx->intLiteral().front()->getText());
  Int upperBound = std::stol(ctx->intLiteral().back()->getText());

  return Set(IntervalSet{lowerBound, upperBound});
}

antlrcpp::Any FznVisitor::visitParArrayLiteral(FlatZincParser::ParArrayLiteralContext* ctx) {
  std::vector<Value> values;
  values.reserve(ctx->basicLiteralExpr().size());

  for (auto& basicLiteralExpr : ctx->basicLiteralExpr()) {
    values.push_back(basicLiteralExpr->accept(this).as<Value>());
  }

  return values;
}
