#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "fznparser/except.hpp"
#include "fznparser/types.hpp"
#include "fznparser/variables.hpp"

namespace fznparser {

class BoolArg
    : public std::variant<bool, std::shared_ptr<const BoolVar>> {
 public:
  using std::variant<bool, std::shared_ptr<const BoolVar>>::variant;

  bool isParameter() const;
  bool isFixed() const;

  std::shared_ptr<const BoolVar> var() const;
  bool parameter() const;

  bool toParameter() const;
  std::shared_ptr<const BoolVar> toVar(fznparser::Model&);
  bool operator==(const BoolArg&) const;
  bool operator!=(const BoolArg&) const;
  std::string toString() const;
};

class IntArg
    : public std::variant<int64_t, std::shared_ptr<const IntVar>> {
 public:
  using std::variant<int64_t, std::shared_ptr<const IntVar>>::variant;

  bool isParameter() const;
  bool isFixed() const;

  std::shared_ptr<const IntVar> var() const;
  int64_t parameter() const;

  int64_t toParameter() const;
  std::shared_ptr<const IntVar> toVar(fznparser::Model&);

  bool operator==(const IntArg&) const;
  bool operator!=(const IntArg&) const;
  std::string toString() const;
};

class FloatArg
    : public std::variant<double, std::shared_ptr<const FloatVar>> {
 public:
  using std::variant<double, std::shared_ptr<const FloatVar>>::variant;

  bool isParameter() const;
  bool isFixed() const;

  std::shared_ptr<const FloatVar> var() const;
  double parameter() const;

  double toParameter() const;
  std::shared_ptr<const FloatVar> toVar(fznparser::Model&);

  bool operator==(const FloatArg&) const;
  bool operator!=(const FloatArg&) const;
  std::string toString() const;
};

class IntSetArg
    : public std::variant<IntSet, std::shared_ptr<const SetVar>> {
 private:
  std::shared_ptr<const SetVar> _setVar{nullptr};

 public:
  using std::variant<IntSet, std::shared_ptr<const SetVar>>::variant;

  bool isParameter() const;
  bool isFixed() const;

  std::shared_ptr<const SetVar> var() const;
  const IntSet& parameter() const;

  const IntSet& toParameter() const;
  std::shared_ptr<const SetVar> toVar(fznparser::Model&);
  bool operator==(const IntSetArg&) const;
  bool operator!=(const IntSetArg&) const;
  std::string toString() const;
};

class Arg : public std::variant<BoolArg, IntArg, FloatArg, IntSetArg, FloatSet,
                     std::shared_ptr<BoolVarArray>, std::shared_ptr<IntVarArray>, std::shared_ptr<FloatVarArray>, std::shared_ptr<SetVarArray>,
                     std::shared_ptr<FloatSetArray>> {
 public:
  using std::variant<BoolArg, IntArg, FloatArg, IntSetArg, FloatSet,
                     std::shared_ptr<BoolVarArray>, std::shared_ptr<IntVarArray>, std::shared_ptr<FloatVarArray>, std::shared_ptr<SetVarArray>,
                     std::shared_ptr<FloatSetArray>>::variant;

  bool isArray() const;
  bool isParameter() const;
  bool isFixed() const;

  bool operator==(const Arg&) const;
  bool operator!=(const Arg&) const;
  std::string toString() const;
};

}  // namespace fznparser