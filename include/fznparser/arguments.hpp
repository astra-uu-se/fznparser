#pragma once

#include <memory>
#include <variant>

#include "fznparser/types.hpp"

namespace fznparser {

class BoolVar;
class BoolVarArray;
class Model;
class IntVar;
class FloatVar;
class SetVar;
class IntVarArray;
class SetVarArray;
class FloatVarArray;

class BoolArg : public std::variant<bool, std::shared_ptr<const BoolVar>> {
 public:
  using std::variant<bool, std::shared_ptr<const BoolVar>>::variant;

  [[nodiscard]] bool isParameter() const;
  [[nodiscard]] bool isFixed() const;

  [[nodiscard]] std::shared_ptr<const BoolVar> var() const;
  [[nodiscard]] bool parameter() const;

  [[nodiscard]] bool toParameter() const;
  std::shared_ptr<const BoolVar> toVar(Model&) const;
  bool operator==(const BoolArg&) const;
  bool operator!=(const BoolArg&) const;
};

class IntArg : public std::variant<int64_t, std::shared_ptr<const IntVar>> {
 public:
  using std::variant<int64_t, std::shared_ptr<const IntVar>>::variant;

  [[nodiscard]] bool isParameter() const;
  [[nodiscard]] bool isFixed() const;

  [[nodiscard]] std::shared_ptr<const IntVar> var() const;
  [[nodiscard]] int64_t parameter() const;

  [[nodiscard]] int64_t toParameter() const;
  [[nodiscard]] std::shared_ptr<const IntVar> toVar(Model&) const;

  bool operator==(const IntArg&) const;
  bool operator!=(const IntArg&) const;
};

class FloatArg : public std::variant<double, std::shared_ptr<const FloatVar>> {
 public:
  using std::variant<double, std::shared_ptr<const FloatVar>>::variant;

  [[nodiscard]] bool isParameter() const;
  [[nodiscard]] bool isFixed() const;

  [[nodiscard]] std::shared_ptr<const FloatVar> var() const;
  [[nodiscard]] double parameter() const;

  [[nodiscard]] double toParameter() const;
  std::shared_ptr<const FloatVar> toVar(Model&) const;

  bool operator==(const FloatArg&) const;
  bool operator!=(const FloatArg&) const;
  [[nodiscard]] std::string toString() const;
};

class IntSetArg : public std::variant<IntSet, std::shared_ptr<const SetVar>> {
  std::shared_ptr<const SetVar> _setVar{nullptr};

 public:
  using std::variant<IntSet, std::shared_ptr<const SetVar>>::variant;

  [[nodiscard]] bool isParameter() const;
  [[nodiscard]] bool isFixed() const;

  [[nodiscard]] std::shared_ptr<const SetVar> var() const;
  [[nodiscard]] const IntSet& parameter() const;

  [[nodiscard]] const IntSet& toParameter() const;
  std::shared_ptr<const SetVar> toVar(Model&);
  bool operator==(const IntSetArg&) const;
  bool operator!=(const IntSetArg&) const;
};

class Arg : public std::variant<
                BoolArg, IntArg, FloatArg, IntSetArg, FloatSet,
                std::shared_ptr<BoolVarArray>, std::shared_ptr<IntVarArray>,
                std::shared_ptr<FloatVarArray>, std::shared_ptr<SetVarArray>,
                std::shared_ptr<FloatSetArray>> {
 public:
  using std::variant<
      BoolArg, IntArg, FloatArg, IntSetArg, FloatSet,
      std::shared_ptr<BoolVarArray>, std::shared_ptr<IntVarArray>,
      std::shared_ptr<FloatVarArray>, std::shared_ptr<SetVarArray>,
      std::shared_ptr<FloatSetArray>>::variant;

  [[nodiscard]] bool isArray() const;
  [[nodiscard]] bool isEmptyArray() const;
  [[nodiscard]] bool isParameter() const;
  [[nodiscard]] bool isFixed() const;

  bool operator==(const Arg&) const;
  bool operator!=(const Arg&) const;
};

}  // namespace fznparser