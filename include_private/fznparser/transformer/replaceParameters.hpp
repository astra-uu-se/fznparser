#pragma once

#include <algorithm>
#include <unordered_map>
#include <vector>

#include "fznparser/except.hpp"
#include "fznparser/parser/grammarAst.hpp"

namespace fznparser::transformer {

using boost::get;

using namespace fznparser::parser;

void replaceParameter(
    const std::unordered_set<std::string_view>& identifiers,
    const std::unordered_map<std::string_view, ParExpr>& parameters,
    BasicExpr* basicExpr) {
  if (basicExpr->type() != typeid(std::string)) {
    return;
  }
  const std::string& identifier = get<std::string>(*basicExpr);
  if (parameters.find(identifier) == parameters.end()) {
    if (identifiers.find(identifier) == identifiers.end()) {
      throw FznException("Reference to undefined parameter or variable \"" +
                         identifier + "\"");
    }
  }
  if (parameters.at(identifier).type() == typeid(bool)) {
    *basicExpr = get<bool>(parameters.at(identifier));
  } else if (parameters.at(identifier).type() == typeid(int64_t)) {
    *basicExpr = get<int64_t>(parameters.at(identifier));
  } else if (parameters.at(identifier).type() == typeid(double)) {
    *basicExpr = get<double>(parameters.at(identifier));
  } else if (parameters.at(identifier).type() == typeid(SetLiteralEmpty)) {
    *basicExpr = get<SetLiteralEmpty>(parameters.at(identifier));
  } else if (parameters.at(identifier).type() == typeid(IntSetLiteralBounded)) {
    *basicExpr = get<IntSetLiteralBounded>(parameters.at(identifier));
  } else if (parameters.at(identifier).type() ==
             typeid(FloatSetLiteralBounded)) {
    *basicExpr = get<FloatSetLiteralBounded>(parameters.at(identifier));
  } else if (parameters.at(identifier).type() == typeid(FloatSetLiteralSet)) {
    *basicExpr = get<FloatSetLiteralSet>(parameters.at(identifier));
  } else {
    throw FznException("Parameter \"" + identifier + "\" has invalid type");
  }
}

void replaceParameters(Model& model) {
  std::unordered_set<std::string> identifiers;
  std::unordered_map<std::string, ParExpr> parameters;

  // find all identifiers
  for (const ParDeclItem& par : model.parameters()) {
    if (identifiers.find(par.identifier) != identifiers.end()) {
      throw FznException("A parameter with identifier \"" +
                         std::string(par.identifier) + "\" already declared");
    }
    identifiers.emplace(parDecl.identifier);
    parameters.emplace(parDecl.identifier, parDecl.expr);
  }
  for (const VarDeclItem& var : model.varDeclItems) {
    const std::string& identifier = var.type() == typeid(BasicVarDecl)
                                        ? get<BasicVarDecl>(var).identifier
                                        : get<ArrayVarDecl>(var).identifier;
    if (identifiers.find(identifier) != identifiers.end()) {
      const std::string t = parameters.find(identifier) != parameters.end()
                                ? "parameter"
                                : "variable";
      throw FznException("A " + t + " with identifier \"" + identifier +
                         "\" already declared");
    }
    identifiers.emplace(identifier);
  }

  for (VarDeclItem var : model.varDeclItems) {
    if (var.type() == typeid(BasicVarDecl) &&
        get<BasicVarDecl>(var).expr.has_value()) {
      replaceParameter(identifiers, parameters,
                       &get<BasicVarDecl>(var).expr.value());
    } else if (var.type() == typeid(ArrayVarDecl)) {
      for (BasicExpr* expr : get<ArrayVarDecl>(var).exprs) {
        replaceParameter(identifiers, parameters, expr);
      }
    }
  }
}

}  // namespace fznparser::transformer