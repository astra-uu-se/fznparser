#include "fznparser/transformer/replaceParameters.hpp"

#include "fznparser/except.hpp"

namespace fznparser::transformer {
void replaceParameter(
    const std::unordered_set<std::string>& identifiers,
    const std::unordered_map<std::string, parser::ParExpr>& parameters,
    BasicExpr* basicExpr) {
  if (basicExpr->type() != typeid(std::string)) {
    return;
  }
  const std::string& identifier = get<std::string>(*basicExpr);
  if (!parameters.contains(identifier)) {
    if (!identifiers.contains(identifier)) {
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