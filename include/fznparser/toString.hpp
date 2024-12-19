#pragma once
#include <string>

namespace fznparser {
class FloatSetArray;
class IntVarArray;
class BoolVarArray;
class SetVar;
class FloatVarArray;
class SetVarArray;
class VarReference;
class Var;
class FloatVar;
class IntVar;
class BoolVar;
class FloatSet;
class IntSet;
class SolveType;
class Constraint;
class Arg;
class IntSetArg;
class FloatArg;
class BoolArg;
class Model;
class Annotation;
class AnnotationExpression;

[[nodiscard]] std::string toString(const Model&);
[[nodiscard]] std::string toString(const Annotation&);
[[nodiscard]] std::string toString(const AnnotationExpression&);
[[nodiscard]] std::string toString(const BoolArg&);
[[nodiscard]] std::string toString(const FloatArg&);
[[nodiscard]] std::string toString(const IntSetArg&);
[[nodiscard]] std::string toString(const Arg&);
[[nodiscard]] std::string toString(const Constraint&);
[[nodiscard]] std::string toString(const SolveType&);
[[nodiscard]] std::string toString(const IntSet&);
[[nodiscard]] std::string toString(const FloatSet&);
[[nodiscard]] std::string toString(const FloatSetArray&);
[[nodiscard]] std::string toString(const BoolVar&);
[[nodiscard]] std::string toString(const IntVar&);
[[nodiscard]] std::string toString(const FloatVar&);
[[nodiscard]] std::string toString(const SetVar& var);
[[nodiscard]] std::string toString(const BoolVarArray&);
[[nodiscard]] std::string toString(const IntVarArray&);
[[nodiscard]] std::string toString(const FloatVarArray&);
[[nodiscard]] std::string toString(const SetVarArray&);
[[nodiscard]] std::string toString(const Var&);
[[nodiscard]] std::string toString(const VarReference&);

}  // namespace fznparser