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

  auto objective = ctx->solveItem()->accept(this).as<Objective>();

  return FZNModel(std::move(parameters), std::move(variables), std::move(constraints), objective);
}

antlrcpp::Any FznVisitor::visitSolveItem(FlatZincParser::SolveItemContext* ctx) {
  if (!ctx->optimization()) {
    return Objective(Satisfy{});
  }

  auto optimization = ctx->optimization()->getText();

  assert(ctx->basicExpr());
  assert(ctx->basicExpr()->Identifier());
  auto variable = createIdentifier(ctx->basicExpr()->Identifier());

  if (optimization == "minimize") {
    return Objective(Minimise{variable});
  } else if (optimization == "maximize") {
    return Objective(Maximise{variable});
  } else {
    throw std::runtime_error(std::string("Unknown optimisation procedure: ").append(optimization));
  }
}

antlrcpp::Any FznVisitor::visitParDeclItem(FlatZincParser::ParDeclItemContext* ctx) {
  auto type = determineParameterType(ctx->parType());
  auto identifier = createIdentifier(ctx->Identifier());
  _parsingParameterType = type;

  switch (type) {
    case ParameterType::INT: {
      auto value = ctx->parExpr()->accept(this).as<Int>();
      _parsingParameterType = {};
      return Parameter(IntParameter{identifier, value});
    }

    case ParameterType::BOOL: {
      auto value = ctx->parExpr()->accept(this).as<bool>();
      _parsingParameterType = {};
      return Parameter(BoolParameter{identifier, value});
    }

    case ParameterType::SET_OF_INT: {
      auto value = ctx->parExpr()->accept(this).as<Set<Int>>();
      _parsingParameterType = {};
      return Parameter(SetOfIntParameter{identifier, value});
    }

    case ParameterType::INT_ARRAY: {
      auto value = ctx->parExpr()->accept(this).as<std::vector<Int>>();
      _parsingParameterType = {};
      return Parameter(ArrayOfIntParameter{identifier, value});
    }

    case ParameterType::BOOL_ARRAY: {
      auto value = ctx->parExpr()->accept(this).as<std::vector<bool>>();
      _parsingParameterType = {};
      return Parameter(ArrayOfBoolParameter{identifier, value});
    }

    case ParameterType::SET_OF_INT_ARRAY: {
      auto value = ctx->parExpr()->accept(this).as<std::vector<Set<Int>>>();
      _parsingParameterType = {};
      return Parameter(ArrayOfSetOfIntParameter{identifier, value});
    }

    default:
      throw std::logic_error("Unhandled parameter type.");
  }
}

antlrcpp::Any FznVisitor::visitParExpr(FlatZincParser::ParExprContext* ctx) {
  if (ctx->basicLiteralExpr()) {
    return ctx->basicLiteralExpr()->accept(this);
  } else if (ctx->parArrayLiteral()) {
    return ctx->parArrayLiteral()->accept(this);
  } else {
    throw std::runtime_error("Unhandled variant in parExpr.");
  }
}

template <typename Type>
static std::optional<BasicExpr<Type>> parseOptionalValue(antlr4::tree::ParseTreeVisitor* visitor,
                                                         FlatZincParser::VarDeclItemContext* ctx) {
  if (ctx->basicExpr()) {
    return ctx->basicExpr()->accept(visitor).as<BasicExpr<Type>>();
  } else {
    return {};
  }
}

antlrcpp::Any FznVisitor::visitVarDeclItem(FlatZincParser::VarDeclItemContext* ctx) {
  auto identifier = createIdentifier(ctx->Identifier());
  auto annotations = ctx->annotations()->accept(this).as<std::vector<Annotation>>();

  if (ctx->basicVarType()) {
    auto domainType = determineVariableType(ctx->basicVarType());
    _parsingVariableType = domainType;

    switch (domainType) {
      case VariableType::INT: {
        auto domain = ctx->basicVarType()->accept(this).as<Domain<Int>>();
        auto value = parseOptionalValue<Int>(this, ctx);
        _parsingVariableType = {};
        return Variable(IntVariable{identifier, domain, annotations, value});
      }

      case VariableType::BOOL: {
        auto domain = ctx->basicVarType()->accept(this).as<Domain<bool>>();
        auto value = parseOptionalValue<bool>(this, ctx);
        _parsingVariableType = {};
        return Variable(BoolVariable{identifier, domain, annotations, value});
      }

      default:
        throw std::runtime_error("Unknown variable type.");
    }
  } else if (ctx->arrayVarType()) {
    auto domainType = determineVariableType(ctx->arrayVarType()->basicVarType());
    _parsingVariableType = domainType;

    switch (domainType) {
      case VariableType::INT: {
        auto contents = ctx->arrayLiteral()->accept(this).as<Array<Int>>();
        _parsingVariableType = {};
        return Variable(IntVariableArray{
            identifier,
            contents,
            annotations,
        });
      }

      case VariableType::BOOL: {
        auto contents = ctx->arrayLiteral()->accept(this).as<Array<bool>>();
        _parsingVariableType = {};
        return Variable(BoolVariableArray{identifier, contents, annotations});
      }

      case VariableType::UNKNOWN: {
        throw std::logic_error("When parsing variable arrays, the type should always be known.");
      }
    }
  }

  throw std::runtime_error("Unhandled variant in varDeclItem.");
}

static antlrcpp::Any createDomainFromParType(FlatZincParser::BasicParTypeContext* ctx) {
  auto text = ctx->getText();

  if (text == "int") {
    return Domain<Int>(BasicDomain<Int>{});
  } else if (text == "bool") {
    return Domain<bool>(BasicDomain<bool>{});
  } else {
    throw std::runtime_error(std::string("Unknown basic domain: ").append(text));
  }
}

antlrcpp::Any FznVisitor::visitBasicVarType(FlatZincParser::BasicVarTypeContext* ctx) {
  if (ctx->basicParType()) {
    return createDomainFromParType(ctx->basicParType());
  } else if (ctx->set()) {
    return Domain<Int>(ctx->set()->accept(this).as<std::vector<Int>>());
  } else if (ctx->intRange()) {
    return Domain<Int>(ctx->intRange()->accept(this).as<IntRange>());
  } else {
    throw std::runtime_error("Unhandled variant in basicVarType");
  }
}

template <typename Type>
static antlrcpp::Any handleBasicExpr(antlr4::tree::ParseTreeVisitor* visitor,
                                     FlatZincParser::BasicExprContext* ctx) {
  if (ctx->Identifier()) {
    return BasicExpr<Type>(createIdentifier(ctx->Identifier()));
  } else if (ctx->basicLiteralExpr()) {
    return BasicExpr<Type>(ctx->basicLiteralExpr()->accept(visitor).as<Type>());
  } else {
    throw std::runtime_error("Unhandled variant in basicExpr.");
  }
}

antlrcpp::Any FznVisitor::visitBasicExpr(FlatZincParser::BasicExprContext* ctx) {
  if (!_parsingVariableType) {
    // Parsing constraint arguments, which means the type is not known up-front. It also means we
    // shouldn't be parsing an identifier (since we wouldn't know the Type in BasicExpr<Type>).
    assert(!ctx->Identifier());
    assert(ctx->basicLiteralExpr());

    return ctx->basicLiteralExpr()->accept(this);
  }

  switch (*_parsingVariableType) {
    case VariableType::INT:
      return handleBasicExpr<Int>(this, ctx);
    case VariableType::BOOL:
      return handleBasicExpr<bool>(this, ctx);
    case VariableType::UNKNOWN:
      return handleBasicExpr<UnknownVariableType>(this, ctx);
    default:
      throw std::logic_error("Unknown variable type.");
  }
}

template <typename Type>
static antlrcpp::Any handleArrayLiteral(antlr4::tree::ParseTreeVisitor* visitor,
                                        FlatZincParser::ArrayLiteralContext* ctx) {
  Array<Type> expressions;
  expressions.reserve(ctx->basicExpr().size());

  for (auto& basicExpr : ctx->basicExpr()) {
    expressions.push_back(basicExpr->accept(visitor).as<BasicExpr<Type>>());
  }

  return expressions;
}

antlrcpp::Any FznVisitor::visitArrayLiteral(FlatZincParser::ArrayLiteralContext* ctx) {
  assert(_parsingVariableType.has_value());

  switch (*_parsingVariableType) {
    case VariableType::INT:
      return handleArrayLiteral<Int>(this, ctx);
    case VariableType::BOOL:
      return handleArrayLiteral<bool>(this, ctx);
    case VariableType::UNKNOWN:
      return handleArrayLiteral<UnknownVariableType>(this, ctx);
    default:
      throw std::logic_error("Unknown variable type.");
  }
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

antlrcpp::Any FznVisitor::visitExpr(FlatZincParser::ExprContext* ctx) {
  if (ctx->basicExpr() && ctx->basicExpr()->Identifier()) {
    return Constraint::Argument(createIdentifier(ctx->basicExpr()->Identifier()));
  } else if (ctx->basicExpr()) {
    auto result = ctx->basicExpr()->accept(this);

    if (result.is<Int>()) {
      return Constraint::Argument(result.as<Int>());
    } else if (result.is<bool>()) {
      return Constraint::Argument(result.as<bool>());
    } else if (result.is<Set<Int>>()) {
      return Constraint::Argument(result.as<Set<Int>>());
    }
  } else if (ctx->arrayLiteral()) {
    _parsingVariableType = VariableType::UNKNOWN;
    auto result = ctx->arrayLiteral()->accept(this);
    _parsingVariableType = {};
    return Constraint::Argument(result.as<Array<UnknownVariableType>>());
  }

  throw std::runtime_error("Unhandled variant in expr.");
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
    return true;
  } else if (text == "false") {
    return false;
  } else {
    throw std::runtime_error("Unhandled bool literal.");
  }
}

antlrcpp::Any FznVisitor::visitIntLiteral(FlatZincParser::IntLiteralContext* ctx) {
  return Int(std::stol(ctx->getText()));
}

antlrcpp::Any FznVisitor::visitSetLiteral(FlatZincParser::SetLiteralContext* ctx) {
  if (ctx->set()) {
    return Set<Int>(ctx->set()->accept(this).as<std::vector<Int>>());
  } else if (ctx->intRange()) {
    return Set<Int>(ctx->intRange()->accept(this).as<IntRange>());
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

template <typename Type>
static antlrcpp::Any handleParArrayLiteral(antlr4::tree::ParseTreeVisitor* visitor,
                                           FlatZincParser::ParArrayLiteralContext* ctx) {
  std::vector<Type> values;
  values.reserve(ctx->basicLiteralExpr().size());

  for (auto& basicLiteralExpr : ctx->basicLiteralExpr()) {
    values.push_back(basicLiteralExpr->accept(visitor).as<Type>());
  }

  return values;
}

antlrcpp::Any FznVisitor::visitParArrayLiteral(FlatZincParser::ParArrayLiteralContext* ctx) {
  assert(_parsingParameterType.has_value());
  switch (*_parsingParameterType) {
    case ParameterType::INT_ARRAY:
      return handleParArrayLiteral<Int>(this, ctx);
    case ParameterType::BOOL_ARRAY:
      return handleParArrayLiteral<bool>(this, ctx);
    case ParameterType::SET_OF_INT_ARRAY:
      return handleParArrayLiteral<Set<Int>>(this, ctx);
    default:
      throw std::logic_error(
          "Parameter array literal should be parsing INT_ARRAY, BOOL_ARRAY or SET_OF_INT_ARRAY.");
  }
}

FznVisitor::VariableType FznVisitor::determineVariableType(
    FlatZincParser::BasicVarTypeContext* ctx) {
  if (ctx->intRange() || ctx->set()) {
    return VariableType::INT;
  }

  if (ctx->basicParType()) {
    auto type = ctx->basicParType()->getText();

    if (type == "int") {
      return VariableType::INT;
    } else if (type == "bool") {
      return VariableType::BOOL;
    }
  }

  throw std::runtime_error("Unsupported variable type.");
}

FznVisitor::ParameterType FznVisitor::determineParameterType(FlatZincParser::ParTypeContext* ctx) {
  auto type = ctx->basicParType()->getText();
  if (ctx->indexSet()) {
    if (type == "int") return ParameterType::INT_ARRAY;
    if (type == "bool") return ParameterType::BOOL_ARRAY;
    if (type == "set of int") return ParameterType::SET_OF_INT_ARRAY;

    throw std::runtime_error(std::string("Unhandled parameter type: array of ").append(type));
  } else {
    if (type == "int") return ParameterType::INT;
    if (type == "bool") return ParameterType::BOOL;
    if (type == "set of int") return ParameterType::SET_OF_INT;

    throw std::runtime_error(std::string("Unhandled parameter type: ").append(type));
  }
}
