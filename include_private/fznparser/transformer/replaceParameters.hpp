#pragma once

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "fznparser/parser/grammarAst.hpp"

namespace fznparser::transformer {
using namespace fznparser::parser;
using boost::get;

void replaceParameter(
    const std::unordered_set<std::string>& identifiers,
    const std::unordered_map<std::string, ParExpr>& parameters,
    BasicExpr* basicExpr);

}  // namespace fznparser::transformer