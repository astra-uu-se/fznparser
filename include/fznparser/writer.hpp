#pragma once
#include <ostream>

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

std::ostream& operator<<(std::ostream&, const Model&);
std::ostream& operator<<(std::ostream&, const Annotation&);
std::ostream& operator<<(std::ostream&, const AnnotationExpression&);
std::ostream& operator<<(std::ostream&, const BoolArg&);
std::ostream& operator<<(std::ostream&, const FloatArg&);
std::ostream& operator<<(std::ostream&, const IntSetArg&);
std::ostream& operator<<(std::ostream&, const Arg&);
std::ostream& operator<<(std::ostream&, const Constraint&);
std::ostream& operator<<(std::ostream&, const SolveType&);
std::ostream& operator<<(std::ostream&, const IntSet&);
std::ostream& operator<<(std::ostream&, const FloatSet&);
std::ostream& operator<<(std::ostream&, const FloatSetArray&);
std::ostream& operator<<(std::ostream&, const BoolVar&);
std::ostream& operator<<(std::ostream&, const IntVar&);
std::ostream& operator<<(std::ostream&, const FloatVar&);
std::ostream& operator<<(std::ostream&, const SetVar& var);
std::ostream& operator<<(std::ostream&, const BoolVarArray&);
std::ostream& operator<<(std::ostream&, const IntVarArray&);
std::ostream& operator<<(std::ostream&, const FloatVarArray&);
std::ostream& operator<<(std::ostream&, const SetVarArray&);
std::ostream& operator<<(std::ostream&, const Var&);
std::ostream& operator<<(std::ostream&, const VarReference&);

}  // namespace fznparser