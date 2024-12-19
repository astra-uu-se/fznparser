#pragma once

#include <string>

#include "fznparser/arguments.hpp"
#include "fznparser/model.hpp"
#include "fznparser/parser/grammarAst.hpp"
#include "fznparser/solveType.hpp"

namespace fznparser {

class ModelTransformer {
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
  static void validate(const parser::BasicVarDecl&);
  void validate(const parser::ArrayVarDecl&) const;
  static void validate(const parser::Annotation&);

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
                        const parser::Expr&) const;
  Arg transformArgArray(const std::unordered_map<std::string, Var>&,
                        const parser::ArrayLiteral&) const;
  Constraint transform(const std::unordered_map<std::string, Var>&,
                       const parser::ConstraintItem&);
  SolveType transform(const std::unordered_map<std::string, Var>&,
                      const parser::SolveItem&);

 public:
  ModelTransformer(const ModelTransformer&) = delete;
  ModelTransformer& operator=(const ModelTransformer&) = delete;
  ModelTransformer(ModelTransformer&&) = delete;
  explicit ModelTransformer(parser::Model&& model);

  Model generateModel();
};

}  // namespace fznparser