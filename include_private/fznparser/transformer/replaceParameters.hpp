#pragma once

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "fznparser/except.hpp"
#include "fznparser/parser/grammarAst.hpp"

namespace fznparser::transformer {

using boost::get;

using namespace fznparser::parser;

void replaceParameter(
    const std::unordered_set<std::string>& identifiers,
    const std::unordered_map<std::string, ParExpr>& parameters,
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

}  // namespace fznparser::transformer