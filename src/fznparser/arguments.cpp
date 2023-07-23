#include "fznparser/arguments.hpp"

#include "fznparser/model.hpp"

namespace fznparser {

using std::get;
using std::reference_wrapper;

bool BoolArg::isParameter() const {
  return std::holds_alternative<bool>(*this);
}

bool BoolArg::toParameter() const {
  if (isParameter()) {
    return get<bool>(*this);
  }
  throw FznException("Argument is not a parameter");
}

const BoolVar& BoolArg::toVar(Model& model) {
  if (isParameter()) {
    return model.boolVarPar(toParameter());
  }
  return get<reference_wrapper<const BoolVar>>(*this).get();
}

bool BoolArg::operator==(const BoolArg& other) const {
  if (isParameter() != other.isParameter()) {
    return false;
  } else if (isParameter() && other.isParameter()) {
    return toParameter() == other.toParameter();
  }
  return get<reference_wrapper<const BoolVar>>(*this).get().operator==(
      get<reference_wrapper<const BoolVar>>(other).get());
}

bool BoolArg::operator!=(const BoolArg& other) const {
  return !operator==(other);
}

std::string BoolArg::toString() const {
  if (isParameter()) {
    return toParameter() ? "true" : "false";
  }
  return std::string(
      get<reference_wrapper<const BoolVar>>(*this).get().identifier());
}

bool IntArg::isParameter() const {
  return std::holds_alternative<int64_t>(*this);
}

int64_t IntArg::toParameter() const {
  if (isParameter()) {
    return get<int64_t>(*this);
  }
  throw FznException("Argument is not a parameter");
}

const IntVar& IntArg::toVar(Model& model) {
  if (isParameter()) {
    return model.addIntVarPar(toParameter());
  }
  return get<reference_wrapper<const IntVar>>(*this).get();
}

bool IntArg::operator==(const IntArg& other) const {
  if (isParameter() != other.isParameter()) {
    return false;
  } else if (isParameter() && other.isParameter()) {
    return toParameter() == other.toParameter();
  }
  return get<reference_wrapper<const IntVar>>(*this).get().operator==(
      get<reference_wrapper<const IntVar>>(other).get());
}

bool IntArg::operator!=(const IntArg& other) const {
  return !operator==(other);
}

std::string IntArg::toString() const {
  if (isParameter()) {
    return std::to_string(toParameter());
  }
  return std::string(
      get<reference_wrapper<const IntVar>>(*this).get().identifier());
}

bool FloatArg::isParameter() const {
  return std::holds_alternative<double>(*this);
}

double FloatArg::toParameter() const {
  if (isParameter()) {
    return get<double>(*this);
  }
  throw FznException("Argument is not a parameter");
}

const FloatVar& FloatArg::toVar(Model& model) {
  if (isParameter()) {
    return model.addFloatVarPar(toParameter());
  }
  return get<reference_wrapper<const FloatVar>>(*this).get();
}

bool FloatArg::operator==(const FloatArg& other) const {
  if (isParameter() != other.isParameter()) {
    return false;
  } else if (isParameter() && other.isParameter()) {
    return toParameter() == other.toParameter();
  }
  return get<reference_wrapper<const FloatVar>>(*this).get().operator==(
      get<reference_wrapper<const FloatVar>>(other).get());
}

bool FloatArg::operator!=(const FloatArg& other) const {
  return !operator==(other);
}

std::string FloatArg::toString() const {
  if (isParameter()) {
    return std::to_string(toParameter());
  }
  return std::string(
      get<reference_wrapper<const FloatVar>>(*this).get().identifier());
}

bool IntSetArg::isParameter() const {
  return std::holds_alternative<IntSet>(*this);
}

const IntSet& IntSetArg::toParameter() const {
  if (isParameter()) {
    return get<IntSet>(*this);
  }
  throw FznException("Argument is not a parameter");
}

const SetVar& IntSetArg::toVar(Model& model) {
  if (_setVar.has_value()) {
    return _setVar.value();
  }
  if (isParameter()) {
    _setVar =
        reference_wrapper<const SetVar>{model.addSetVarPar(toParameter())};
    return _setVar.value();
  }
  return get<reference_wrapper<const SetVar>>(*this);
}

bool IntSetArg::operator==(const IntSetArg& other) const {
  if (isParameter() != other.isParameter()) {
    return false;
  } else if (isParameter() && other.isParameter()) {
    return toParameter() == other.toParameter();
  }
  return get<reference_wrapper<const SetVar>>(*this).get().operator==(
      get<reference_wrapper<const SetVar>>(other).get());
}

bool IntSetArg::operator!=(const IntSetArg& other) const {
  return !operator==(other);
}

std::string IntSetArg::toString() const {
  if (isParameter()) {
    return toParameter().toString();
  }
  return std::string(
      get<reference_wrapper<const SetVar>>(*this).get().identifier());
}

bool Arg::operator==(const Arg& other) const {
  if (holds_alternative<BoolArg>(*this) && holds_alternative<BoolArg>(other)) {
    return get<BoolArg>(*this).operator==(get<BoolArg>(other));
  } else if (holds_alternative<IntArg>(*this) &&
             holds_alternative<IntArg>(other)) {
    return get<IntArg>(*this).operator==(get<IntArg>(other));
  } else if (holds_alternative<FloatArg>(*this) &&
             holds_alternative<FloatArg>(other)) {
    return get<FloatArg>(*this).operator==(get<FloatArg>(other));
  } else if (holds_alternative<IntSetArg>(*this) &&
             holds_alternative<IntSetArg>(other)) {
    return get<IntSetArg>(*this).operator==(get<IntSetArg>(other));
  } else if (holds_alternative<FloatSet>(*this) &&
             holds_alternative<FloatSet>(other)) {
    return get<FloatSet>(*this).operator==(get<FloatSet>(other));
  } else if (holds_alternative<BoolVarArray>(*this) &&
             holds_alternative<BoolVarArray>(other)) {
    return get<BoolVarArray>(*this).operator==(get<BoolVarArray>(other));
  } else if (holds_alternative<IntVarArray>(*this) &&
             holds_alternative<IntVarArray>(other)) {
    return get<IntVarArray>(*this).operator==(get<IntVarArray>(other));
  } else if (holds_alternative<FloatVarArray>(*this) &&
             holds_alternative<FloatVarArray>(other)) {
    return get<FloatVarArray>(*this).operator==(get<FloatVarArray>(other));
  } else if (holds_alternative<SetVarArray>(*this) &&
             holds_alternative<SetVarArray>(other)) {
    return get<SetVarArray>(*this).operator==(get<SetVarArray>(other));
  } else if (holds_alternative<FloatSetArray>(*this) &&
             holds_alternative<FloatSetArray>(other)) {
    return get<FloatSetArray>(*this).operator==(get<FloatSetArray>(other));
  }
  return false;
}

bool Arg::operator!=(const Arg& other) const { return !operator==(other); }

std::string Arg::toString() const {
  if (holds_alternative<BoolArg>(*this)) {
    return get<BoolArg>(*this).toString();
  } else if (holds_alternative<IntArg>(*this)) {
    return get<IntArg>(*this).toString();
  } else if (holds_alternative<FloatArg>(*this)) {
    return get<FloatArg>(*this).toString();
  } else if (holds_alternative<IntSetArg>(*this)) {
    return get<IntSetArg>(*this).toString();
  } else if (holds_alternative<FloatSet>(*this)) {
    return get<FloatSet>(*this).toString();
  } else if (std::holds_alternative<BoolVarArray>(*this)) {
    return get<BoolVarArray>(*this).toString();
  } else if (std::holds_alternative<IntVarArray>(*this)) {
    return get<IntVarArray>(*this).toString();
  } else if (std::holds_alternative<FloatVarArray>(*this)) {
    return get<FloatVarArray>(*this).toString();
  } else if (std::holds_alternative<SetVarArray>(*this)) {
    return get<SetVarArray>(*this).toString();
  } else if (std::holds_alternative<FloatSetArray>(*this)) {
    return get<FloatSetArray>(*this).toString();
  }
  return "";
}

}  // namespace fznparser