#pragma once

#include <optional>
#include <stdexcept>
#include <string>

#include "fznparser/annotation.hpp"
#include "fznparser/arguments.hpp"
#include "fznparser/constraint.hpp"
#include "fznparser/model.hpp"
#include "fznparser/parser/grammarAst.hpp"
#include "fznparser/solveType.hpp"
#include "fznparser/variables.hpp"

namespace fznparser {

class ModelTransformer {
 private:
  parser::Model _model;
  std::unordered_map<std::string, parser::PredicateItem> _predDeclItems;
  std::unordered_map<std::string, parser::ParDeclItem> _parDeclItems;
  std::unordered_map<std::string, parser::VarDeclItem> _varDeclItems;

  void replaceParameters(parser::VarDeclItem&);
  void replaceParameters(parser::BasicVarDecl&);
  void replaceParameters(parser::ArrayVarDecl&);
  void replaceParameters(parser::ArrayLiteral&);
  void replaceParameters(parser::ConstraintItem&);

  void validate(const parser::VarDeclItem&) const;
  void validate(const parser::BasicVarDecl&) const;
  void validate(const parser::ArrayVarDecl&) const;
  void validate(const parser::Annotation&) const;

  const std::type_info& arrayType(
      const std::unordered_map<std::string, Var>& vars,
      const parser::ArrayLiteral&) const;

  static Var transform(const std::unordered_map<std::string, Var>&,
                const parser::VarDeclItem&);
  static Var transformVar(const std::unordered_map<std::string, Var>&,
                          const parser::BasicVarDecl&);
  static Var transformVarArray(const std::unordered_map<std::string, Var>&,
                               const parser::ArrayVarDecl&);
  Arg transformArgument(const std::unordered_map<std::string, Var>&,
                        const parser::Expr&);
  Arg transformArgArray(const std::unordered_map<std::string, Var>&,
                        const parser::ArrayLiteral&);
  Constraint transform(const std::unordered_map<std::string, Var>&,
                       const parser::ConstraintItem&);
  SolveType transform(const std::unordered_map<std::string, Var>&,
                      const parser::SolveItem&);

 public:
  ModelTransformer(const ModelTransformer&) = delete;
  ModelTransformer& operator=(const ModelTransformer&) = delete;
  ModelTransformer(ModelTransformer&&) = delete;
  explicit ModelTransformer(parser::Model&& model);

  fznparser::Model generateModel();
};

}  // namespace fznparser