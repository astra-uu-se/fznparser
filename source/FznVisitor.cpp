#include "FznVisitor.hpp"

#include <cstdint>
#include <memory>
#include <vector>

using Int = int64_t;

antlrcpp::Any FznVisitor::visitModel(FlatZincParser::ModelContext *ctx) {
  model = new Model();
  for (auto item : ctx->parDeclItem()) {
    visitParDeclItem(item);
  }
  for (auto item : ctx->varDeclItem()) {
    visitVarDeclItem(item);
  }
  for (auto constraintItem : ctx->constraintItem()) {
    model->addConstraint(visitConstraintItem(constraintItem).as<ConstraintBox>());
  }
  if (auto b = ctx->solveItem()->basicExpr()) {
    if (b->Identifier()) {
      model->addObjective(b->Identifier()->getText());
    } else if (auto ble __attribute__((unused)) = b->basicLiteralExpr()) {
      std::cerr << "Objective is literal expression\n";
    }
  }
  model->init();
  return model;
}

antlrcpp::Any FznVisitor::visitParDeclItem(FlatZincParser::ParDeclItemContext *ctx) {
  std::string name = ctx->Identifier()->getText();

  if (!ctx->parType()->indexSet()) {
    std::string value = ctx->parExpr()->getText();  // Value is discarded
    // return static_cast<std::shared_ptr<Variable>>(
    //     std::make_shared<Parameter>(name, value));
    model->addVariable(std::make_shared<Parameter>(name, value));
  } else {  // if (ctx->arrayVarType()) {
    std::vector<Expression> elements
        = visitParArrayLiteral(ctx->parExpr()->parArrayLiteral()).as<std::vector<Expression>>();
    std::vector<Annotation> annotations;
    // return static_cast<std::shared_ptr<Variable>>(
    //     std::make_shared<ArrayVariable>(name, annotations, elements));
    model->addVariable(std::make_shared<ArrayVariable>(name, annotations, elements));
  }
  return 0;
}
antlrcpp::Any FznVisitor::visitVarDeclItem(FlatZincParser::VarDeclItemContext *ctx) {
  std::string name = ctx->Identifier()->getText();
  std::vector<Annotation> annotations
      = visitAnnotations(ctx->annotations()).as<std::vector<Annotation>>();
  ;

  if (ctx->basicVarType()) {
    std::shared_ptr<Domain> domain
        = visitBasicVarType(ctx->basicVarType()).as<std::shared_ptr<Domain>>();
    // return static_cast<std::shared_ptr<Variable>>(
    //     std::make_shared<SingleVariable>(name, annotations, domain));
    model->addVariable(std::make_shared<Variable>(name, annotations, domain));
  } else {  // if (ctx->arrayVarType()) {
    std::vector<Expression> elements
        = visitArrayLiteral(ctx->arrayLiteral()).as<std::vector<Expression>>();
    // return static_cast<std::shared_ptr<Variable>>(
    //     std::make_shared<ArrayVariable>(name, annotations, elements));
    model->addVariable(std::make_shared<ArrayVariable>(name, annotations, elements));
  }
  return 0;
}
antlrcpp::Any FznVisitor::visitBasicVarType(FlatZincParser::BasicVarTypeContext *ctx) {
  if (ctx->basicParType()) {
    if (ctx->basicParType()->getText() == "bool") {
      return static_cast<std::shared_ptr<Domain>>(std::make_shared<BoolDomain>());
    }
    if (ctx->basicParType()->getText() == "int") {
      return static_cast<std::shared_ptr<Domain>>(std::make_shared<IntDomain>());
    }
  }

  if (ctx->intRange()) {
    Int lb = stol(ctx->intRange()->intLiteral()[0]->getText());
    Int ub = stol(ctx->intRange()->intLiteral()[1]->getText());

    return static_cast<std::shared_ptr<Domain>>(std::make_shared<IntDomain>(lb, ub));
  }

  if (ctx->set()) {
    std::set<Int> s;
    for (auto i : ctx->set()->intLiteral()) {
      s.insert(stol(i->getText()));
    }
    return static_cast<std::shared_ptr<Domain>>(std::make_shared<IntSetDomain>(s));
  }

  throw "Domain not supported";
}
antlrcpp::Any FznVisitor::visitAnnotations(FlatZincParser::AnnotationsContext *ctx) {
  std::vector<Annotation> annotations;
  for (auto a : ctx->annotation()) {
    annotations.push_back(visitAnnotation(a).as<Annotation>());
  }
  return annotations;
}
antlrcpp::Any FznVisitor::visitAnnotation(FlatZincParser::AnnotationContext *ctx) {
  std::string variableName;
  if (ctx->Identifier()->getText() == "defines_var") {
    variableName = ctx->annExpr()[0]->getText();
  } else {
    variableName = "";
  }
  return Annotation(ctx->Identifier()->getText(), variableName);
}

antlrcpp::Any FznVisitor::visitConstraintItem(FlatZincParser::ConstraintItemContext *ctx) {
  std::string name = ctx->Identifier()->getText();
  std::vector<Expression> expressions;
  for (auto exp : ctx->expr()) {
    expressions.push_back(visitExpr(exp).as<Expression>());
  }
  return ConstraintBox(name, expressions,
                       visitAnnotations(ctx->annotations()).as<std::vector<Annotation>>());
}

antlrcpp::Any FznVisitor::visitExpr(FlatZincParser::ExprContext *ctx) {
  if (auto b = ctx->basicExpr()) {
    if (b->Identifier()) {
      return Expression(b->Identifier()->getText(), true);
    } else if (auto ble = b->basicLiteralExpr()) {
      return Expression(ble->getText(), false);
    }
  } else if (auto c = ctx->arrayLiteral()) {
    return Expression(c->getText(), visitArrayLiteral(c).as<std::vector<Expression>>(), false);
  }
  std::cout << "Parsed something wrong" << std::endl;
  return Expression();
}

antlrcpp::Any FznVisitor::visitArrayLiteral(FlatZincParser::ArrayLiteralContext *ctx) {
  std::vector<Expression> ab;
  for (auto c : ctx->basicExpr()) {
    if (c->Identifier()) {
      ab.push_back(Expression(c->Identifier()->getText(), true));
    } else {
      ab.push_back(Expression(c->getText(), false));
    }
  }
  return ab;
}
antlrcpp::Any FznVisitor::visitParArrayLiteral(FlatZincParser::ParArrayLiteralContext *ctx) {
  std::vector<Expression> ab;
  for (auto c : ctx->basicLiteralExpr()) {
    ab.push_back(Expression(c->getText(), false));
  }
  return ab;
}
