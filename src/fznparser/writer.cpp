#include "fznparser/writer.hpp"

#include "fznparser/annotation.hpp"
#include "fznparser/arguments.hpp"
#include "fznparser/constraint.hpp"
#include "fznparser/model.hpp"
#include "fznparser/types.hpp"
#include "fznparser/variables.hpp"
#include "variant"

namespace fznparser {

using std::get;
using std::holds_alternative;

std::ostream& writeAnnotations(std::ostream& os,
                               const std::vector<Annotation>& annotations) {
  if (!annotations.empty()) {
    for (const auto& annotation : annotations) {
      os << " :: " << annotation;
    }
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const Annotation& annotation) {
  os << annotation.identifier();
  if (!annotation.expressions().empty()) {
    os << "(";
    for (size_t i = 0; i < annotation.expressions().size(); ++i) {
      if (i != 0) {
        os << ", ";
      }
      os << "[";
      for (size_t j = 0; j < annotation.expressions().at(i).size(); ++j) {
        if (j != 0) {
          os << ", ";
        }
        os << annotation.expressions().at(i).at(j);
      }
      os << "]";
    }
    os << ")";
  }
  return os;
}

std::ostream& operator<<(std::ostream& os,
                         const AnnotationExpression& annotationExpression) {
  if (holds_alternative<bool>(annotationExpression)) {
    return os << (get<bool>(annotationExpression) ? "true" : "false");
  }
  if (holds_alternative<int64_t>(annotationExpression)) {
    return os << std::to_string(get<int64_t>(annotationExpression));
  }
  if (holds_alternative<double>(annotationExpression)) {
    return os << std::to_string(get<double>(annotationExpression));
  }
  if (holds_alternative<IntSet>(annotationExpression)) {
    return os << get<IntSet>(annotationExpression);
  }
  if (holds_alternative<FloatSet>(annotationExpression)) {
    return os << get<FloatSet>(annotationExpression);
  }
  if (holds_alternative<std::string>(annotationExpression)) {
    return os << get<std::string>(annotationExpression);
  }
  if (holds_alternative<Annotation>(annotationExpression)) {
    return os << get<Annotation>(annotationExpression);
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const BoolArg& arg) {
  if (arg.isParameter()) {
    return os << (arg.parameter() ? "true" : "false");
  }
  return os << arg.var()->identifier();
}

std::ostream& operator<<(std::ostream& os, const IntArg& arg) {
  if (arg.isParameter()) {
    return os << std::to_string(arg.toParameter());
  }
  return os << arg.var()->identifier();
}

std::ostream& operator<<(std::ostream& os, const FloatArg& arg) {
  if (arg.isParameter()) {
    return os << std::to_string(arg.toParameter());
  }
  return os << arg.var()->identifier();
}

std::ostream& operator<<(std::ostream& os, const IntSetArg& arg) {
  if (arg.isParameter()) {
    return os << arg.toParameter();
  }
  return os << arg.var()->identifier();
}

std::ostream& operator<<(std::ostream& os, const Arg& arg) {
  if (holds_alternative<BoolArg>(arg)) {
    return os << get<BoolArg>(arg);
  }
  if (holds_alternative<IntArg>(arg)) {
    return os << get<IntArg>(arg);
  }
  if (holds_alternative<FloatArg>(arg)) {
    return os << get<FloatArg>(arg);
  }
  if (holds_alternative<IntSetArg>(arg)) {
    return os << get<IntSetArg>(arg);
  }
  if (holds_alternative<FloatSet>(arg)) {
    return os << get<FloatSet>(arg);
  }
  if (holds_alternative<std::shared_ptr<BoolVarArray>>(arg)) {
    return os << *get<std::shared_ptr<BoolVarArray>>(arg);
  }
  if (holds_alternative<std::shared_ptr<IntVarArray>>(arg)) {
    return os << *get<std::shared_ptr<IntVarArray>>(arg);
  }
  if (holds_alternative<std::shared_ptr<FloatVarArray>>(arg)) {
    return os << *get<std::shared_ptr<FloatVarArray>>(arg);
  }
  if (holds_alternative<std::shared_ptr<SetVarArray>>(arg)) {
    return os << *get<std::shared_ptr<SetVarArray>>(arg);
  }
  if (holds_alternative<std::shared_ptr<FloatSetArray>>(arg)) {
    return os << get<std::shared_ptr<FloatSetArray>>(arg);
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const Constraint& constraint) {
  os << constraint.identifier() << '(';
  for (size_t i = 0; i < constraint.arguments().size(); ++i) {
    if (i != 0) {
      os << ", ";
    }
    os << constraint.arguments()[i];
  }
  os << ')';

  return os;
}

std::ostream& operator<<(std::ostream& os, const Model& model) {
  for (const auto& [identifier, var] : model.vars()) {
    os << var << ";\n";
  }
  for (const Constraint& con : model.constraints()) {
    os << con << ";\n";
  }
  return os << model.solveType() << ";\n";
}

std::ostream& operator<<(std::ostream& os, const SolveType& solveType) {
  os << "solve ";
  if (solveType.problemType() == ProblemType::SATISFY) {
    os << "satisfy";
  } else {
    os << (solveType.problemType() == ProblemType::MINIMIZE ? "minimize"
                                                            : "maximize");
    if (solveType.hasObjective()) {
      os << std::string(solveType.objective().identifier());
    }
  }
  return writeAnnotations(os, solveType.annotations());
}

std::ostream& operator<<(std::ostream& os, const IntSet& set) {
  if (set.isInterval()) {
    return os << std::to_string(set.lowerBound()) << ".."
              << std::to_string(set.upperBound());
  }
  const auto& elements = set.elements();
  os << '{';
  for (size_t i = 0; i < elements.size(); ++i) {
    if (i != 0) {
      os << ", ";
    }
    os << std::to_string(elements[i]);
  }
  return os << '}';
}

std::ostream& operator<<(std::ostream& os, const FloatSet& set) {
  if (set.isInterval()) {
    return os << std::to_string(set.lowerBound()) << ".."
              << std::to_string(set.upperBound());
  }
  const auto& elements = set.elements();
  os << '{';
  for (size_t i = 0; i < elements.size(); ++i) {
    if (i != 0) {
      os << ", ";
    }
    os << std::to_string(elements[i]);
  }
  return os << '}';
}

std::ostream& operator<<(std::ostream& os, const FloatSetArray& arr) {
  os << '[';
  for (size_t i = 0; i < arr.size(); ++i) {
    if (i != 0) {
      os << ", ";
    }
    os << arr.at(i);
  }
  return os << ']';
}

std::ostream& operator<<(std::ostream& os, const BoolVar& var) {
  os << "var bool: " << var.identifier();
  if (var.isFixed()) {
    os << (var.contains(true) ? " = true" : " = false");
  }
  return writeAnnotations(os, var.annotations());
}

std::ostream& operator<<(std::ostream& os, const IntVar& var) {
  os << "var " << var.domain() << ": " << var.identifier();
  return writeAnnotations(os, var.annotations());
}

std::ostream& operator<<(std::ostream& os, const FloatVar& var) {
  os << "var " << var.domain() << ": " << var.identifier();
  return writeAnnotations(os, var.annotations());
}

std::ostream& operator<<(std::ostream& os, const SetVar& var) {
  os << "var " << var.lowerBound() << ": " << var.identifier();
  return writeAnnotations(os, var.annotations());
}

std::ostream& operator<<(std::ostream& os, const BoolVarArray& var) {
  if (!var.identifier().empty()) {
    os << "array[1.." << std::to_string(var.size()) << "] of"
       << (var.isParArray() ? "" : " var") << " bool: " << var.identifier()
       << " = ";
  }
  os << '[';
  for (size_t i = 0; i < var.size(); ++i) {
    if (i != 0) {
      os << ", ";
    }
    if (std::holds_alternative<bool>(var.at(i))) {
      os << (get<bool>(var.at(i)) ? "true" : "false");
    } else {
      os << get<std::shared_ptr<const BoolVar>>(var.at(i))->identifier();
    }
  }
  os << ']';
  return writeAnnotations(os, var.annotations());
}

std::ostream& operator<<(std::ostream& os, const IntVarArray& var) {
  if (!var.identifier().empty()) {
    os << "array[1.." << std::to_string(var.size()) << "] of"
       << (var.isParArray() ? "" : " var") << " int: " << var.identifier()
       << " = ";
  }
  os << '[';
  for (size_t i = 0; i < var.size(); ++i) {
    if (i != 0) {
      os << ", ";
    }
    if (std::holds_alternative<int64_t>(var.at(i))) {
      os << std::to_string(get<int64_t>(var.at(i)));
    } else {
      os << get<std::shared_ptr<const IntVar>>(var.at(i))->identifier();
    }
  }
  os << ']';
  return writeAnnotations(os, var.annotations());
}

std::ostream& operator<<(std::ostream& os, const FloatVarArray& var) {
  if (!var.identifier().empty()) {
    os << "array[1.." << std::to_string(var.size()) << "] of"
       << (var.isParArray() ? "" : " var") << " float: " << var.identifier()
       << " = ";
  }
  os << '[';
  for (size_t i = 0; i < var.size(); ++i) {
    if (i != 0) {
      os << ", ";
    }
    if (std::holds_alternative<double>(var.at(i))) {
      os << std::to_string(get<double>(var.at(i)));
    } else {
      os << get<std::shared_ptr<const FloatVar>>(var.at(i))->identifier();
    }
  }
  os << ']';
  return writeAnnotations(os, var.annotations());
}

std::ostream& operator<<(std::ostream& os, const SetVarArray& var) {
  if (!var.identifier().empty()) {
    os << "array[1.." << std::to_string(var.size()) << "] of"
       << (var.isParArray() ? " int set" : " var set") << ": "
       << var.identifier() << " = ";
  }
  os << '[';
  for (size_t i = 0; i < var.size(); ++i) {
    if (i != 0) {
      os << ", ";
    }
    if (std::holds_alternative<IntSet>(var.at(i))) {
      os << get<IntSet>(var.at(i));
    } else {
      os << get<std::shared_ptr<const SetVar>>(var.at(i))->identifier();
    }
  }
  os << ']';
  return writeAnnotations(os, var.annotations());
}

std::ostream& operator<<(std::ostream& os, const Var& var) {
  if (std::holds_alternative<std::shared_ptr<BoolVar>>(var)) {
    return os << *get<std::shared_ptr<BoolVar>>(var);
  }
  if (std::holds_alternative<std::shared_ptr<IntVar>>(var)) {
    return os << *get<std::shared_ptr<IntVar>>(var);
  }
  if (std::holds_alternative<std::shared_ptr<FloatVar>>(var)) {
    return os << *get<std::shared_ptr<FloatVar>>(var);
  }
  if (std::holds_alternative<std::shared_ptr<SetVar>>(var)) {
    return os << *get<std::shared_ptr<SetVar>>(var);
  }
  if (std::holds_alternative<std::shared_ptr<BoolVarArray>>(var)) {
    return os << *get<std::shared_ptr<BoolVarArray>>(var);
  }
  if (std::holds_alternative<std::shared_ptr<IntVarArray>>(var)) {
    return os << *get<std::shared_ptr<IntVarArray>>(var);
  }
  if (std::holds_alternative<std::shared_ptr<FloatVarArray>>(var)) {
    return os << *get<std::shared_ptr<FloatVarArray>>(var);
  }
  if (std::holds_alternative<std::shared_ptr<SetVarArray>>(var)) {
    return os << *get<std::shared_ptr<SetVarArray>>(var);
  }
  if (std::holds_alternative<std::shared_ptr<VarReference>>(var)) {
    return os << get<std::shared_ptr<VarReference>>(var);
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const VarReference& var) {
  os << "var: " << var.identifier() << " = " << var.source().identifier();
  writeAnnotations(os, var.annotations());
  return os << ';';
}

}  // namespace fznparser