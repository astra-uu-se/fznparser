#pragma once

#include <vector>

#include "fznparser/parser/grammar.hpp"

namespace fznparser::parser {

void validate(const ArrayVarType& arrayVarType, const ArrayLiteral& basicExprs);

void validate(const ParType& parType, const ParExpr& parExpr);

void validate(const AnnExpr& annExpr);

}  // namespace fznparser::parser