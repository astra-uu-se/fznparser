#include "FznVisitor.hpp"

#include <cstdint>
#include <exception>
#include <limits>
#include <memory>
#include <string>
#include <vector>

antlrcpp::Any FznVisitor::visitModel(FlatZincParser::ModelContext *ctx) {
  for (auto item : ctx->parDeclItem()) {
    item->accept(this);
  }

  for (auto item : ctx->varDeclItem()) {
    std::shared_ptr<fznparser::Variable> variable
        = item->accept(this).as<std::shared_ptr<fznparser::Variable>>();

    _variableMap.emplace(variable->name(), variable);
  }

  std::vector<std::shared_ptr<fznparser::Variable>> variables;
  variables.reserve(_variableMap.size());
  for (const auto &entry : _variableMap) variables.push_back(entry.second);

  std::vector<std::shared_ptr<fznparser::Constraint>> constraints;
  for (auto constraintItem : ctx->constraintItem()) {
    constraints.push_back(
        constraintItem->accept(this).as<std::shared_ptr<fznparser::Constraint>>());
  }

  // if (auto b = ctx->solveItem()->basicExpr()) {
  //   if (b->Identifier()) {
  //     model->addObjective(b->Identifier()->getText());
  //   } else if (b->basicLiteralExpr() != nullptr) {
  //     std::cerr << "Objective is literal expression\n";
  //   }
  // }

  return new fznparser::Model(variables, constraints);
}

antlrcpp::Any FznVisitor::visitParDeclItem(
    [[maybe_unused]] FlatZincParser::ParDeclItemContext *ctx) {
  // std::string name = ctx->Identifier()->getText();

  // if (!ctx->parType()->indexSet()) {
  //   std::string value = ctx->parExpr()->getText();  // Value is discarded
  //   // return static_cast<std::shared_ptr<Variable>>(
  //   //     std::make_shared<Parameter>(name, value));
  //   model->addVariable(std::make_shared<Parameter>(name, value));
  // } else {  // if (ctx->arrayVarType()) {
  //   std::vector<Expression> elements
  //       = visitParArrayLiteral(ctx->parExpr()->parArrayLiteral()).as<std::vector<Expression>>();
  //   std::vector<Annotation> annotations;
  //   // return static_cast<std::shared_ptr<Variable>>(
  //   //     std::make_shared<ArrayVariable>(name, annotations, elements));
  //   model->addVariable(std::make_shared<ArrayVariable>(name, annotations, elements));
  // }
  return 0;
}

antlrcpp::Any FznVisitor::visitVarDeclItem(FlatZincParser::VarDeclItemContext *ctx) {
  std::string name = ctx->Identifier()->getText();
  std::vector<std::shared_ptr<fznparser::Annotation>> annotations
      = ctx->annotations()->accept(this).as<std::vector<std::shared_ptr<fznparser::Annotation>>>();

  if (ctx->basicVarType()) {
    fznparser::Domain *domain = ctx->basicVarType()->accept(this).as<fznparser::Domain *>();

    return std::static_pointer_cast<fznparser::Variable>(
        std::make_shared<fznparser::SearchVariable>(name, annotations,
                                                    std::unique_ptr<fznparser::Domain>(domain)));
  }

  assert(ctx->arrayVarType());
  std::vector<std::shared_ptr<fznparser::Variable>> contents
      = ctx->arrayLiteral()->accept(this).as<std::vector<std::shared_ptr<fznparser::Variable>>>();

  return std::static_pointer_cast<fznparser::Variable>(
      std::make_shared<fznparser::ArrayVariable>(name, annotations, contents));
}

antlrcpp::Any FznVisitor::visitBasicVarType(FlatZincParser::BasicVarTypeContext *ctx) {
  if (ctx->basicParType()) {
    if (ctx->basicParType()->getText() == "bool") {
      return static_cast<fznparser::Domain *>(new fznparser::IntDomain(0, 1));
    } else if (ctx->basicParType()->getText() == "int") {
      return static_cast<fznparser::Domain *>(new fznparser::IntDomain(
          std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::min()));
    }
  }

  if (ctx->intRange()) {
    int64_t lb = std::stol(ctx->intRange()->intLiteral()[0]->getText());
    int64_t ub = std::stol(ctx->intRange()->intLiteral()[1]->getText());

    return static_cast<fznparser::Domain *>(new fznparser::IntDomain(lb, ub));
  }

  // if (ctx->set()) {
  //   std::set<Int> s;
  //   for (auto i : ctx->set()->intLiteral()) {
  //     s.insert(stol(i->getText()));
  //   }
  //   return static_cast<std::shared_ptr<Domain>>(std::make_shared<IntSetDomain>(s));
  // }

  throw std::logic_error("Domain not supported");
}

antlrcpp::Any FznVisitor::visitAnnotations(FlatZincParser::AnnotationsContext *ctx) {
  std::vector<std::shared_ptr<fznparser::Annotation>> annotations;
  for (auto a : ctx->annotation()) {
    annotations.push_back(a->accept(this).as<std::shared_ptr<fznparser::Annotation>>());
  }
  return annotations;
}

antlrcpp::Any FznVisitor::visitAnnotation([[maybe_unused]] FlatZincParser::AnnotationContext *ctx) {
  return std::static_pointer_cast<fznparser::Annotation>(
      std::make_shared<fznparser::MarkerAnnotation>(fznparser::AnnotationType::OUTPUT));
}

antlrcpp::Any FznVisitor::visitConstraintItem(FlatZincParser::ConstraintItemContext *ctx) {
  std::string name = ctx->Identifier()->getText();
  std::vector<std::shared_ptr<fznparser::Annotation>> annotations
      = ctx->annotations()->accept(this).as<std::vector<std::shared_ptr<fznparser::Annotation>>>();

  std::vector<std::shared_ptr<fznparser::Variable>> arguments;
  arguments.reserve(ctx->expr().size());

  for (auto expr : ctx->expr()) {
    if (expr->arrayLiteral()) {
      throw std::logic_error("Unimplemented: in-line array literals in constraint items.");

      // TODO: group array literal into one variable. This would be a variable that does not exist
      // in the flatzinc itself. Maybe the constructors for fznparser::Constraint need to accept
      // vectors of variables as a single input.

      //      std::vector<std::shared_ptr<fznparser::Variable>> vars
      //          = expr->arrayLiteral()
      //                ->accept(this)
      //                .as<std::vector<std::shared_ptr<fznparser::Variable>>>();
    } else if (expr->basicExpr()) {
      std::string variableName = expr->basicExpr()->Identifier()->getText();
      arguments.push_back(_variableMap.at(variableName));
    }
  }

  return fznparser::Constraint::create(name, annotations, arguments);
}

antlrcpp::Any FznVisitor::visitExpr([[maybe_unused]] FlatZincParser::ExprContext *ctx) {
  // if (auto b = ctx->basicExpr()) {
  //   if (b->Identifier()) {
  //     return Expression(b->Identifier()->getText(), true);
  //   } else if (auto ble = b->basicLiteralExpr()) {
  //     return Expression(ble->getText(), false);
  //   }
  // } else if (auto c = ctx->arrayLiteral()) {
  //   return Expression(c->getText(), visitArrayLiteral(c).as<std::vector<Expression>>(), false);
  // }
  // std::cout << "Parsed something wrong" << std::endl;
  // return Expression();

  return 0;
}

antlrcpp::Any FznVisitor::visitArrayLiteral(
    [[maybe_unused]] FlatZincParser::ArrayLiteralContext *ctx) {
  // Implemented under the assumption that all variables in an array are previously declared, and
  // are in the _variableMap.

  std::vector<std::shared_ptr<fznparser::Variable>> variables;
  variables.reserve(ctx->basicExpr().size());

  for (auto expr : ctx->basicExpr()) {
    variables.push_back(_variableMap.at(expr->Identifier()->getText()));
  }

  return variables;
}

antlrcpp::Any FznVisitor::visitParArrayLiteral(
    [[maybe_unused]] FlatZincParser::ParArrayLiteralContext *ctx) {
  // std::vector<Expression> ab;
  // for (auto c : ctx->basicLiteralExpr()) {
  //   ab.push_back(Expression(c->getText(), false));
  // }
  // return ab;
  return 0;
}
