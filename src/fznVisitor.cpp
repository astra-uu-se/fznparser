#include "FznVisitor.hpp"

#include <cstdint>
#include <exception>
#include <limits>
#include <memory>
#include <string>
#include <vector>

antlrcpp::Any FznVisitor::visitModel(FlatZincParser::ModelContext *ctx) {
  std::vector<fznparser::Parameter> parameters;
  for (auto item : ctx->parDeclItem()) {
    fznparser::Parameter param = item->accept(this).as<fznparser::Parameter>();

    parameters.push_back(param);

    std::visit([this](auto p) { _literalMap.emplace(p->name(), p); }, param);
  }

  std::vector<std::shared_ptr<fznparser::Variable>> variables;
  for (auto item : ctx->varDeclItem()) {
    std::shared_ptr<fznparser::Variable> variable
        = item->accept(this).as<std::shared_ptr<fznparser::Variable>>();

    variables.push_back(variable);
    _literalMap.emplace(variable->name(), variable);
  }

  std::vector<std::shared_ptr<fznparser::Constraint>> constraints;
  for (auto constraintItem : ctx->constraintItem()) {
    constraints.push_back(
        constraintItem->accept(this).as<std::shared_ptr<fznparser::Constraint>>());
  }

  fznparser::Objective objective;
  std::optional<std::shared_ptr<fznparser::Variable>> objectiveValue = {};

  if (!ctx->solveItem()->optimization()) {
    objective = fznparser::Objective::SATISFY;
  } else if (ctx->solveItem()->optimization()->getText() == "minimize") {
    objective = fznparser::Objective::MINIMIZE;
  } else if (ctx->solveItem()->optimization()->getText() == "maximize") {
    objective = fznparser::Objective::MAXIMIZE;
  } else {
    // Should never happen
    throw std::logic_error("Unknown objective: " + ctx->getText());
  }

  if (objective != fznparser::Objective::SATISFY) {
    auto val = _literalMap.at(ctx->solveItem()->basicExpr()->getText());
    objectiveValue = std::dynamic_pointer_cast<fznparser::Variable>(val);
  }

  return new fznparser::Model(parameters, variables, constraints, objective, objectiveValue);
}

antlrcpp::Any FznVisitor::visitParDeclItem(
    [[maybe_unused]] FlatZincParser::ParDeclItemContext *ctx) {
  fznparser::Parameter param;
  std::string name = ctx->Identifier()->getText();

  if (ctx->parExpr()->basicLiteralExpr()) {
    param = std::make_shared<fznparser::SingleParameter>(
        name, std::stol(ctx->parExpr()->basicLiteralExpr()->getText()));
  } else {
    std::vector<int64_t> values
        = ctx->parExpr()->parArrayLiteral()->accept(this).as<std::vector<int64_t>>();

    param = std::make_shared<fznparser::ParameterArray>(name, values);
  }

  return param;
}

antlrcpp::Any FznVisitor::visitVarDeclItem(FlatZincParser::VarDeclItemContext *ctx) {
  std::string name = ctx->Identifier()->getText();
  ctx->annotations()->accept(this);

  if (ctx->basicVarType()) {
    fznparser::Domain *domain = ctx->basicVarType()->accept(this).as<fznparser::Domain *>();

    return std::static_pointer_cast<fznparser::Variable>(
        std::make_shared<fznparser::SearchVariable>(name, _annotations,
                                                    std::unique_ptr<fznparser::Domain>(domain)));
  }

  assert(ctx->arrayVarType());
  auto contents
      = ctx->arrayLiteral()->accept(this).as<std::vector<std::shared_ptr<fznparser::Literal>>>();

  // We know at this point contents is a vector of search variables, but we cannot make the compiler
  // believe that without using dynamic_cast on each of the elements.

  std::vector<std::shared_ptr<fznparser::SearchVariable>> variables;
  variables.reserve(contents.size());
  for (const auto &literal : contents)
    variables.emplace_back(std::dynamic_pointer_cast<fznparser::SearchVariable>(literal));

  return std::static_pointer_cast<fznparser::Variable>(
      std::make_shared<fznparser::VariableArray>(name, _annotations, variables));
}

antlrcpp::Any FznVisitor::visitBasicVarType(FlatZincParser::BasicVarTypeContext *ctx) {
  if (ctx->basicParType()) {
    if (ctx->basicParType()->getText() == "bool") {
      return static_cast<fznparser::Domain *>(new fznparser::IntDomain(0, 1));
    } else if (ctx->basicParType()->getText() == "int") {
      return static_cast<fznparser::Domain *>(new fznparser::IntDomain(
          std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max()));
    }
  }

  if (ctx->intRange()) {
    int64_t lb = std::stol(ctx->intRange()->intLiteral()[0]->getText());
    int64_t ub = std::stol(ctx->intRange()->intLiteral()[1]->getText());

    return static_cast<fznparser::Domain *>(new fznparser::IntDomain(lb, ub));
  }

  throw std::logic_error("Domain not supported");
}

antlrcpp::Any FznVisitor::visitAnnotations(FlatZincParser::AnnotationsContext *ctx) {
  _annotations = fznparser::MutableAnnotationCollection();
  for (auto a : ctx->annotation()) {
    a->accept(this);
  }
  return _annotations;
}

static std::pair<int64_t, int64_t> parseIntRange(FlatZincParser::SetLiteralContext* setLiteral) {
  assert(setLiteral);
  assert(setLiteral->intLiteral().size() == 2);

  int64_t rangeStart = 0, rangeEnd = 0;
  std::stringstream startStream(setLiteral->intLiteral()[0]->getText());
  startStream >> rangeStart;
  std::stringstream endStream(setLiteral->intLiteral()[1]->getText());
  endStream >> rangeEnd;

  assert(rangeEnd >= rangeStart);

  return std::make_pair(rangeStart, rangeEnd);
}

antlrcpp::Any FznVisitor::visitAnnotation([[maybe_unused]] FlatZincParser::AnnotationContext *ctx) {
  std::string name = ctx->Identifier()->getText();
  if (name == "defines_var") {
    assert(ctx->annExpr().size() == 1);
    auto variableName = ctx->annExpr()[0]->basicAnnExpr()[0]->annotation()->Identifier()->getText();
    std::weak_ptr<fznparser::Variable> variable
        = std::dynamic_pointer_cast<fznparser::Variable>(_literalMap.at(variableName));

    _annotations.add<fznparser::DefinesVarAnnotation>(variable);
  } else if (name == "output_array") {
    assert(ctx->annExpr().size() == 1);
    auto ranges = ctx->annExpr()[0]->basicAnnExpr();

    std::vector<int64_t> dimensions;
    dimensions.reserve(ranges.size());

    for (const auto& expr : ranges) {
      assert(expr->basicLiteralExpr());

      auto setLiteral = expr->basicLiteralExpr()->setLiteral();
      const auto [rangeStart, rangeEnd] = parseIntRange(setLiteral);
      assert(rangeStart == 1);

      dimensions.push_back(rangeEnd);
    }

    _annotations.add<fznparser::OutputArrayAnnotation>(std::move(dimensions));
  } else if (name == "output_var") {
    _annotations.add<fznparser::OutputAnnotation>();
  } else if (name == "is_defined_var") {
    _annotations.add<fznparser::DefinedVarAnnotation>();
  } else if (name == "var_is_introduced") {
    _annotations.add<fznparser::IntroducedVarAnnotation>();
  } else {
    throw std::runtime_error("Unsupported annotation: " + name);
  }

  return 0;
}

antlrcpp::Any FznVisitor::visitConstraintItem(FlatZincParser::ConstraintItemContext *ctx) {
  std::string name = ctx->Identifier()->getText();
  ctx->annotations()->accept(this);

  std::vector<fznparser::ConstraintArgument> arguments;
  arguments.reserve(ctx->expr().size());

  for (auto expr : ctx->expr()) {
    if (expr->arrayLiteral()) {
      auto argument = expr->arrayLiteral()
                          ->accept(this)
                          .as<std::vector<std::shared_ptr<fznparser::Literal>>>();

      arguments.emplace_back(argument);
    } else if (expr->basicExpr()->Identifier()) {
      std::string variableName = expr->basicExpr()->Identifier()->getText();
      arguments.emplace_back(_literalMap.at(variableName));
    } else if (expr->basicExpr()->basicLiteralExpr()) {
      int64_t value = expr->basicExpr()->basicLiteralExpr()->accept(this).as<int64_t>();
      arguments.emplace_back(std::make_shared<fznparser::ValueLiteral>(value));
    }
  }

  return std::make_shared<fznparser::Constraint>(name, arguments, _annotations);
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
  std::vector<std::shared_ptr<fznparser::Literal>> literals;
  literals.reserve(ctx->basicExpr().size());

  for (auto expr : ctx->basicExpr()) {
    if (expr->basicLiteralExpr()) {
      int64_t value = expr->basicLiteralExpr()->accept(this).as<int64_t>();
      literals.emplace_back(std::make_shared<fznparser::ValueLiteral>(value));
    } else if (expr->Identifier()) {
      literals.push_back(_literalMap.at(expr->Identifier()->getText()));
    }
  }

  return literals;
}

antlrcpp::Any FznVisitor::visitParArrayLiteral(
    [[maybe_unused]] FlatZincParser::ParArrayLiteralContext *ctx) {
  std::vector<int64_t> values;
  values.reserve(ctx->basicLiteralExpr().size());

  for (auto c : ctx->basicLiteralExpr()) {
    values.push_back(c->accept(this).as<int64_t>());
  }

  return values;
}

antlrcpp::Any FznVisitor::visitBasicLiteralExpr(FlatZincParser::BasicLiteralExprContext *ctx) {
  if (ctx->intLiteral()) {
    return std::stol(ctx->intLiteral()->getText());
  } else {
    throw std::runtime_error("Unimplemented literal type.");
  }
}
