#pragma once

#include <array>
#include <functional>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "fznparser/annotation.hpp"
#include "fznparser/except.hpp"
#include "fznparser/types.hpp"

namespace fznparser {

class Model;  // forward declaration
class Variable;

class VarBase {
  std::string _identifier;
  std::vector<Annotation> _annotations;

 protected:
  VarBase(const std::string&, std::vector<Annotation>&&);
  bool _isOutput = false;
  bool _isDefinedVar = false;

 public:
  VarBase(const VarBase&) = default;
  VarBase(VarBase&&) = default;
  virtual ~VarBase() = default;

  virtual void interpretAnnotations(
      const std::unordered_map<std::string, Variable>&);

  const std::string& identifier() const;
  const std::vector<Annotation>& annotations() const;
  void addAnnotation(const Annotation&);
  void addAnnotation(const std::string&);
  void addAnnotation(const std::string&, AnnotationExpression&&);
  void addAnnotation(const std::string&, std::vector<AnnotationExpression>&&);
  bool isOutput() const;
  bool isDefinedVar() const;

  virtual bool isFixed() const = 0;
  virtual std::string toString() const = 0;
};

class BoolVar : public VarBase {
  signed char _domain;

 public:
  BoolVar(const BoolVar&) = default;
  BoolVar(BoolVar&&) = default;
  BoolVar(const std::string&, std::vector<Annotation>&& = {});
  BoolVar(bool, const std::string&, std::vector<Annotation>&& = {});
  virtual ~BoolVar() = default;

  bool contains(const bool& val) const;
  bool lowerBound() const;
  bool upperBound() const;

  bool operator==(const BoolVar&) const;
  bool operator!=(const BoolVar&) const;

  virtual bool isFixed() const override;
  virtual std::string toString() const override;
};

class IntVar : public VarBase {
  IntSet _domain;

 public:
  IntVar(const IntVar&) = default;
  IntVar(IntVar&&) = default;
  IntVar(const std::string&, std::vector<Annotation>&& annotations = {});
  IntVar(int64_t, const std::string&,
         std::vector<Annotation>&& annotations = {});
  IntVar(int64_t lb, int64_t ub, const std::string&,
         std::vector<Annotation>&& annotations = {});
  IntVar(std::vector<int64_t>&&, const std::string&,
         std::vector<Annotation>&& annotations = {});
  virtual ~IntVar() = default;

  const IntSet& domain() const;

  bool contains(const int64_t& val) const;
  int64_t lowerBound() const;
  int64_t upperBound() const;

  bool operator==(const IntVar&) const;
  bool operator!=(const IntVar&) const;

  virtual bool isFixed() const override;
  virtual std::string toString() const override;
};

class FloatVar : public VarBase {
  FloatSet _domain;

 public:
  FloatVar(const FloatVar&) = default;
  FloatVar(FloatVar&&) = default;

  const FloatSet& domain() const;

  FloatVar(const std::string&, std::vector<Annotation>&& = {});
  FloatVar(double, const std::string&, std::vector<Annotation>&& = {});
  FloatVar(double lb, double ub, const std::string&,
           std::vector<Annotation>&& = {});
  FloatVar(std::vector<double>&&, const std::string&,
           std::vector<Annotation>&& = {});

  virtual ~FloatVar() = default;

  bool contains(const double& val) const;
  double lowerBound() const;
  double upperBound() const;

  bool operator==(const FloatVar&) const;
  bool operator!=(const FloatVar&) const;

  virtual bool isFixed() const override;
  virtual std::string toString() const override;
};

class SetVar : public VarBase {
  IntSet _domain;

 public:
  SetVar(const SetVar&) = default;
  SetVar(SetVar&&) = default;
  virtual ~SetVar() = default;

  SetVar(int64_t lb, int64_t ub, const std::string&,
         std::vector<Annotation>&& = {});
  SetVar(std::vector<int64_t>&&, const std::string&,
         std::vector<Annotation>&& = {});
  SetVar(IntSet&&, const std::string&, std::vector<Annotation>&& = {});
  SetVar(const IntSet&, const std::string&, std::vector<Annotation>&& = {});

  bool contains(const IntSet& val) const;
  IntSet lowerBound() const;
  IntSet upperBound() const;

  bool operator==(const SetVar&) const;
  bool operator!=(const SetVar&) const;

  virtual bool isFixed() const override;
  virtual std::string toString() const override;
};

class Variable;  // forward declaration

class VarArrayBase : public VarBase {
 private:
  std::vector<int64_t> _outputIndexSetSizes{};

 public:
  VarArrayBase(const VarArrayBase&) = default;
  VarArrayBase(VarArrayBase&&) = default;
  VarArrayBase(const std::string&, std::vector<Annotation>&&);

  virtual void interpretAnnotations(
      const std::unordered_map<std::string, Variable>&);

  const std::vector<int64_t>& outputIndexSetSizes() const;
  std::optional<std::reference_wrapper<const Variable>> definedVariable(
      const fznparser::Model&) const;
};

template <typename ParType, typename VarType>
class VarArrayTemplate : public VarArrayBase {
 protected:
  std::vector<std::variant<ParType, std::reference_wrapper<const VarType>>>
      _vars;
  VarArrayTemplate(const std::string& identifier,
                   std::vector<Annotation>&& annotations)
      : VarArrayBase(identifier, std::move(annotations)){};
  virtual ~VarArrayTemplate() = default;

 public:
  VarArrayTemplate(const VarArrayTemplate&) = default;
  VarArrayTemplate(VarArrayTemplate&&) = default;

  bool isParArray() const {
    return std::all_of(_vars.begin(), _vars.end(), [&](const auto& var) {
      return std::holds_alternative<ParType>(var);
    });
  };

  virtual bool isFixed() const override { return isParArray(); };

  std::vector<ParType> toParVector() const {
    std::vector<ParType> parVector;
    parVector.reserve(_vars.size());
    for (const auto& var : _vars) {
      if (!std::holds_alternative<ParType>(var)) {
        throw std::runtime_error("Cannot convert to parameter array");
      }
      parVector.push_back(std::get<ParType>(var));
    }
    return parVector;
  };

  virtual std::vector<std::reference_wrapper<const VarType>> toVarVector(
      fznparser::Model&) = 0;

  void append(const ParType& par) { _vars.emplace_back(par); };
  void append(const VarType& var) {
    _vars.emplace_back(std::reference_wrapper<const VarType>(var));
  };
  virtual std::string toString() const override = 0;
  size_t size() const { return _vars.size(); };
  std::variant<ParType, std::reference_wrapper<const VarType>> operator[](
      size_t index) const {
    return _vars[index];
  };
  std::variant<ParType, std::reference_wrapper<const VarType>> at(
      size_t index) const {
    return _vars.at(index);
  };
};

class BoolVarArray : public VarArrayTemplate<bool, BoolVar> {
 public:
  BoolVarArray(const BoolVarArray&) = default;
  BoolVarArray(BoolVarArray&&) = default;
  BoolVarArray(const std::string&, std::vector<Annotation>&& = {});
  virtual ~BoolVarArray() = default;

  virtual std::vector<std::reference_wrapper<const BoolVar>> toVarVector(
      fznparser::Model&) override;

  bool operator==(const BoolVarArray&) const;
  bool operator!=(const BoolVarArray&) const;
  virtual std::string toString() const override;
};

class IntVarArray : public VarArrayTemplate<int64_t, IntVar> {
 public:
  IntVarArray(const IntVarArray&) = default;
  IntVarArray(IntVarArray&&) = default;
  IntVarArray(const std::string&, std::vector<Annotation>&& = {});
  virtual ~IntVarArray() = default;

  virtual std::vector<std::reference_wrapper<const IntVar>> toVarVector(
      fznparser::Model&) override;

  bool operator==(const IntVarArray&) const;
  bool operator!=(const IntVarArray&) const;
  virtual std::string toString() const override;
};

class FloatVarArray : public VarArrayTemplate<double, FloatVar> {
 public:
  FloatVarArray(const FloatVarArray&) = default;
  FloatVarArray(FloatVarArray&&) = default;
  FloatVarArray(const std::string& identifier, std::vector<Annotation>&& = {});
  virtual ~FloatVarArray() = default;

  virtual std::vector<std::reference_wrapper<const FloatVar>> toVarVector(
      fznparser::Model&) override;

  bool operator==(const FloatVarArray&) const;
  bool operator!=(const FloatVarArray&) const;
  virtual std::string toString() const override;
};

class SetVarArray : public VarArrayTemplate<IntSet, SetVar> {
 public:
  SetVarArray(const SetVarArray&) = default;
  SetVarArray(SetVarArray&&) = default;
  SetVarArray(const std::string&, std::vector<Annotation>&& = {});
  virtual ~SetVarArray() = default;

  virtual std::vector<std::reference_wrapper<const SetVar>> toVarVector(
      fznparser::Model&) override;

  bool operator==(const SetVarArray&) const;
  bool operator!=(const SetVarArray&) const;
  virtual std::string toString() const override;
};

class Variable
    : public std::variant<BoolVar, IntVar, FloatVar, SetVar, BoolVarArray,
                          IntVarArray, FloatVarArray, SetVarArray> {
 public:
  using std::variant<BoolVar, IntVar, FloatVar, SetVar, BoolVarArray,
                     IntVarArray, FloatVarArray, SetVarArray>::variant;

  bool isOutput() const;
  const std::string& identifier() const;
  void interpretAnnotations(const std::unordered_map<std::string, Variable>&);
  bool isArray() const;
  bool operator==(const Variable&) const;
  bool operator!=(const Variable&) const;
  std::string toString() const;
};

}  // namespace fznparser