#pragma once

#include <boost/spirit/home/x3.hpp>
#include <string>

#include "fznparser/parser/grammarAst.hpp"

namespace fznparser::parser {

using x3::symbols, x3::rule;

struct BasicParTypes_ : symbols<BasicParType> {
  BasicParTypes_() {
    add("bool", BasicParType::BOOL)     //
        ("int", BasicParType::INT)      //
        ("float", BasicParType::FLOAT)  //
        ("set of int", BasicParType::SET_OF_INT);
  }
} const basicParTypes_;

struct OptimizationType_ : symbols<OptimizationType> {
  OptimizationType_() {
    add("minimize", OptimizationType::MINIMIZE)  //
        ("maximize", OptimizationType::MAXIMIZE);
  }
} const optimizationType_;

using annotation_type = rule<struct annotation_class, Annotation>;
using ann_expr_type = rule<struct ann_expr_class, AnnExpr>;
using basic_ann_expr_type = rule<struct basic_ann_expr_class, BasicAnnExpr>;
}  // namespace fznparser::parser
