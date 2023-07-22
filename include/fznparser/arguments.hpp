#pragma once

#include <array>
#include <functional>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include "fznparser/except.hpp"
#include "fznparser/types.hpp"
#include "fznparser/variables.hpp"

namespace fznparser {

class BoolArg
    : public std::variant<bool, std::reference_wrapper<const BoolVar>> {
 public:
  using std::variant<bool, std::reference_wrapper<const BoolVar>>::variant;

  bool isParameter() const;
  bool toParameter() const;
  const BoolVar& toVar(fznparser::Model&);
  bool operator==(const BoolArg&) const;
  bool operator!=(const BoolArg&) const;
  std::string toString() const;
};

class IntArg
    : public std::variant<int64_t, std::reference_wrapper<const IntVar>> {
 public:
  using std::variant<int64_t, std::reference_wrapper<const IntVar>>::variant;

  bool isParameter() const;
  int64_t toParameter() const;
  const IntVar& toVar(fznparser::Model&);
  bool operator==(const IntArg&) const;
  bool operator!=(const IntArg&) const;
  std::string toString() const;
};

class FloatArg
    : public std::variant<double, std::reference_wrapper<const FloatVar>> {
 public:
  using std::variant<double, std::reference_wrapper<const FloatVar>>::variant;

  bool isParameter() const;
  double toParameter() const;
  const FloatVar& toVar(fznparser::Model&);
  bool operator==(const FloatArg&) const;
  bool operator!=(const FloatArg&) const;
  std::string toString() const;
};

class IntSetArg
    : public std::variant<IntSet, std::reference_wrapper<const SetVar>> {
 private:
  std::optional<std::reference_wrapper<const SetVar>> _setVar{};

 public:
  using std::variant<IntSet, std::reference_wrapper<const SetVar>>::variant;

  bool isParameter() const;
  const IntSet& toParameter() const;
  const SetVar& toVar(fznparser::Model&);
  bool operator==(const IntSetArg&) const;
  bool operator!=(const IntSetArg&) const;
  std::string toString() const;
};

class Arg : public std::variant<BoolArg, IntArg, FloatArg, IntSetArg, FloatSet,
                                BoolVarArray, IntVarArray, FloatVarArray,
                                SetVarArray, FloatSetArray> {
 public:
  using std::variant<BoolArg, IntArg, FloatArg, IntSetArg, FloatSet,
                     BoolVarArray, IntVarArray, FloatVarArray, SetVarArray,
                     FloatSetArray>::variant;
  bool isVarArray() const;
  bool isVar() const;
  bool operator==(const Arg&) const;
  bool operator!=(const Arg&) const;
  std::string toString() const;
};

}  // namespace fznparser