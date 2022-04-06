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

  std::vector<Variable> variables;
  for (auto& varDeclItem : ctx->varDeclItem()) {
    variables.push_back(varDeclItem->accept(this).as<Variable>());
  }

  std::vector<Constraint> constraints;
  for (auto& constraintItem : ctx->constraintItem()) {
    constraints.push_back(constraintItem->accept(this).as<Constraint>());
  }

  return FZNModel(std::move(parameters), std::move(variables), std::move(constraints), Satisfy{});
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

antlrcpp::Any FznVisitor::visitVarDeclItem(FlatZincParser::VarDeclItemContext* ctx) {
  auto identifier = createIdentifier(ctx->Identifier());
  auto annotations = ctx->annotations()->accept(this).as<std::vector<Annotation>>();

  if (ctx->basicVarType()) {
    auto domain = ctx->basicVarType()->accept(this).as<Domain>();

    auto value = [&]() -> std::optional<BasicExpr> {
      if (ctx->basicExpr()) {
        return ctx->basicExpr()->accept(this).as<BasicExpr>();
      } else {
        return {};
      }
    }();

    return Variable(SearchVariable{identifier, domain, annotations, value});
  } else if (ctx->arrayVarType()) {
    auto contents = ctx->arrayLiteral()->accept(this).as<std::vector<BasicExpr>>();
    return Variable(VariableArray{identifier, contents, annotations});
  } else {
    throw std::runtime_error("Unhandled variant in varDeclItem.");
  }
}

static Domain createDomainFromParType(FlatZincParser::BasicParTypeContext* ctx) {
  auto text = ctx->getText();

  if (text == "int") {
    return Domain(BasicDomain<Int>{});
  } else if (text == "bool") {
    return Domain(BasicDomain<bool>{});
  } else {
    throw std::runtime_error(std::string("Unknown basic domain: ").append(text));
  }
}

antlrcpp::Any FznVisitor::visitBasicVarType(FlatZincParser::BasicVarTypeContext* ctx) {
  if (ctx->basicParType()) {
    return createDomainFromParType(ctx->basicParType());
  } else if (ctx->set()) {
    return Domain(ctx->set()->accept(this).as<std::vector<Int>>());
  } else if (ctx->intRange()) {
    return Domain(ctx->intRange()->accept(this).as<IntRange>());
  } else {
    throw std::runtime_error("Unhandled variant in basicVarType");
  }
}

antlrcpp::Any FznVisitor::visitBasicExpr(FlatZincParser::BasicExprContext* ctx) {
  if (ctx->Identifier()) {
    return BasicExpr(createIdentifier(ctx->Identifier()));
  } else if (ctx->basicLiteralExpr()) {
    return BasicExpr(ctx->basicLiteralExpr()->accept(this).as<Value>());
  } else {
    throw std::runtime_error("Unhandled variant in basicExpr.");
  }
}

antlrcpp::Any FznVisitor::visitArrayLiteral(FlatZincParser::ArrayLiteralContext* ctx) {
  Array expressions;
  expressions.reserve(ctx->basicExpr().size());

  for (auto& basicExpr : ctx->basicExpr()) {
    expressions.push_back(basicExpr->accept(this).as<BasicExpr>());
  }

  return expressions;
}

antlrcpp::Any FznVisitor::visitConstraintItem(FlatZincParser::ConstraintItemContext* ctx) {
  auto identifier = createIdentifier(ctx->Identifier());
  auto annotations = ctx->annotations()->accept(this).as<std::vector<Annotation>>();

  std::vector<Constraint::Argument> arguments;
  arguments.reserve(ctx->expr().size());
  for (auto& expr : ctx->expr()) {
    arguments.push_back(expr->accept(this).as<Constraint::Argument>());
  }

  return Constraint{identifier, arguments, annotations};
}

static Constraint::Argument createArgumentFromBasicExpr(BasicExpr& expr) {
  if (std::holds_alternative<Identifier>(expr)) {
    return std::get<Identifier>(expr);
  }

  if (std::holds_alternative<Value>(expr)) {
    return std::get<Value>(expr);
  }

  throw std::runtime_error("Unhandled basic expr variant when converting to constraint argument.");
}

antlrcpp::Any FznVisitor::visitExpr(FlatZincParser::ExprContext* ctx) {
  if (ctx->basicExpr()) {
    return createArgumentFromBasicExpr(ctx->basicExpr()->accept(this).as<BasicExpr>());
  } else if (ctx->arrayLiteral()) {
    return Constraint::Argument(ctx->arrayLiteral()->accept(this).as<Array>());
  } else {
    throw std::runtime_error("Unhandled variant in expr.");
  }
}

antlrcpp::Any FznVisitor::visitAnnotations(FlatZincParser::AnnotationsContext* ctx) {
  std::vector<Annotation> annotations;
  annotations.reserve(ctx->annotation().size());

  for (auto& annotation : ctx->annotation()) {
    annotations.push_back(annotation->accept(this).as<Annotation>());
  }

  return annotations;
}

antlrcpp::Any FznVisitor::visitAnnotation(FlatZincParser::AnnotationContext* ctx) {
  auto identifier = createIdentifier(ctx->Identifier());

  if (identifier == "output_array") {
    assert(ctx->annExpr().size() == 1);
    assert(!ctx->annExpr()[0]->basicAnnExpr().empty());

    std::vector<Int> sizes;
    sizes.reserve(ctx->annExpr()[0]->basicAnnExpr().size());
    for (auto& basicAnnExpr : ctx->annExpr()[0]->basicAnnExpr()) {
      assert(basicAnnExpr->basicLiteralExpr());
      assert(basicAnnExpr->basicLiteralExpr()->setLiteral());
      assert(basicAnnExpr->basicLiteralExpr()->setLiteral()->intRange());

      auto range
          = basicAnnExpr->basicLiteralExpr()->setLiteral()->intRange()->accept(this).as<IntRange>();
      assert(range.lowerBound == 1);

      sizes.push_back(range.upperBound);
    }

    return Annotation(OutputArrayAnnotation{sizes});
  } else if (identifier == "defines_var") {
    assert(ctx->annExpr().size() == 1);
    assert(ctx->annExpr()[0]->basicAnnExpr().size() == 1);
    assert(ctx->annExpr()[0]->basicAnnExpr()[0]->annotation());
    assert(ctx->annExpr()[0]->basicAnnExpr()[0]->annotation()->Identifier());

    auto definedVariableIdentifier
        = createIdentifier(ctx->annExpr()[0]->basicAnnExpr()[0]->annotation()->Identifier());
    return Annotation(DefinesVariableAnnotation{definedVariableIdentifier});
  } else {
    return Annotation(TagAnnotation{identifier});
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
  if (ctx->set()) {
    return Value(Set(ctx->set()->accept(this).as<std::vector<Int>>()));
  } else if (ctx->intRange()) {
    return Value(Set(ctx->intRange()->accept(this).as<IntRange>()));
  } else {
    throw std::runtime_error("Unhandled variant in setLiteral.");
  }
}

antlrcpp::Any FznVisitor::visitSet(FlatZincParser::SetContext* ctx) {
  std::vector<Int> values;
  values.reserve(ctx->intLiteral().size());

  for (auto& literal : ctx->intLiteral()) {
    values.push_back(std::stol(literal->getText()));
  }

  return values;
}

antlrcpp::Any FznVisitor::visitIntRange(FlatZincParser::IntRangeContext* ctx) {
  assert(ctx->intLiteral().size() == 2);

  Int lowerBound = std::stol(ctx->intLiteral().front()->getText());
  Int upperBound = std::stol(ctx->intLiteral().back()->getText());

  return IntRange{lowerBound, upperBound};
}

antlrcpp::Any FznVisitor::visitParArrayLiteral(FlatZincParser::ParArrayLiteralContext* ctx) {
  std::vector<Value> values;
  values.reserve(ctx->basicLiteralExpr().size());

  for (auto& basicLiteralExpr : ctx->basicLiteralExpr()) {
    values.push_back(basicLiteralExpr->accept(this).as<Value>());
  }

  return values;
}
