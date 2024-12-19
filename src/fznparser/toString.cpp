#include "fznparser/toString.hpp"

#include <sstream>

#include "fznparser/annotation.hpp"
#include "fznparser/arguments.hpp"
#include "fznparser/model.hpp"
#include "fznparser/writer.hpp"
#include "variant"

namespace fznparser {

using std::get;
using std::holds_alternative;

std::string toString(const Annotation& annotation) {
  return (std::ostringstream() << annotation).str();
}

std::string toString(const AnnotationExpression& annotationExpression) {
  return (std::ostringstream() << annotationExpression).str();
}

std::string toString(const BoolArg& arg) {
  return (std::ostringstream() << arg).str();
}

std::string toString(const IntArg& arg) {
  return (std::ostringstream() << arg).str();
}

std::string toString(const FloatArg& arg) {
  return (std::ostringstream() << arg).str();
}

std::string toString(const IntSetArg& arg) {
  return (std::ostringstream() << arg).str();
}

std::string toString(const Arg& arg) {
  return (std::ostringstream() << arg).str();
}

std::string toString(const Constraint& constraint) {
  return (std::ostringstream() << constraint).str();
}

std::string toString(const Model& model) {
  return (std::ostringstream() << model).str();
}

std::string toString(const SolveType& solveType) {
  return (std::ostringstream() << solveType).str();
}

std::string toString(const IntSet& set) {
  return (std::ostringstream() << set).str();
}

std::string toString(const FloatSet& set) {
  return (std::ostringstream() << set).str();
}

std::string toString(const FloatSetArray& arr) {
  return (std::ostringstream() << arr).str();
}

std::string toString(const BoolVar& var) {
  return (std::ostringstream() << var).str();
}

std::string toString(const IntVar& var) {
  return (std::ostringstream() << var).str();
}

std::string toString(const FloatVar& var) {
  return (std::ostringstream() << var).str();
}

std::string toString(const SetVar& var) {
  return (std::ostringstream() << var).str();
}

std::string toString(const BoolVarArray& var) {
  return (std::ostringstream() << var).str();
}

std::string toString(const IntVarArray& var) {
  return (std::ostringstream() << var).str();
}

std::string toString(const FloatVarArray& var) {
  return (std::ostringstream() << var).str();
}

std::string toString(const SetVarArray& var) {
  return (std::ostringstream() << var).str();
}

std::string toString(const Var& var) {
  return (std::ostringstream() << var).str();
}

std::string toString(const VarReference& var) {
  return (std::ostringstream() << var).str();
}

}  // namespace fznparser