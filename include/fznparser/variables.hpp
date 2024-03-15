#pragma once

#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "fznparser/annotation.hpp"
#include "fznparser/types.hpp"

namespace fznparser {

class Model;  // forward declaration
class Var;
class VarReference;

class VarBase {
  std::string _identifier;
  std::vector<fznparser::Annotation> _annotations;

 protected:
  VarBase(std::string, std::vector<fznparser::Annotation>&&);
  bool _isOutput = false;
  bool _isDefinedVar = false;

 public:
  VarBase(const VarBase&) = default;
  VarBase(VarBase&&) = default;
  virtual ~VarBase() = default;

  virtual void interpretAnnotations(
      const std::unordered_map<std::string, Var>&);

  [[nodiscard]] const std::string& identifier() const;
  [[nodiscard]] const std::vector<fznparser::Annotation>& annotations() const;
  void addAnnotation(const fznparser::Annotation&);
  void addAnnotation(const std::string&);
  void addAnnotation(const std::string&, AnnotationExpression&&);
  void addAnnotation(const std::string&, std::vector<AnnotationExpression>&&);
  [[nodiscard]] bool isOutput() const;
  [[nodiscard]] bool isDefinedVar() const;

  [[nodiscard]] virtual bool isFixed() const = 0;
  [[nodiscard]] virtual std::string toString() const = 0;
};

class BoolVar : public VarBase {
  signed char _domain;

 public:
  BoolVar(const BoolVar&) = default;
  BoolVar(BoolVar&&) = default;
  explicit BoolVar(const std::string&,
                   std::vector<fznparser::Annotation>&& = {});
  BoolVar(bool, const std::string&, std::vector<fznparser::Annotation>&& = {});

  [[nodiscard]] bool contains(const bool& val) const;
  [[nodiscard]] bool lowerBound() const;
  [[nodiscard]] bool upperBound() const;

  bool operator==(const BoolVar&) const;
  bool operator!=(const BoolVar&) const;

  [[nodiscard]] bool isFixed() const override;
  [[nodiscard]] std::string toString() const override;
};

class IntVar : public VarBase {
  IntSet _domain;

 public:
  IntVar(const IntVar&) = default;
  IntVar(IntVar&&) = default;
  explicit IntVar(const std::string&,
                  std::vector<fznparser::Annotation>&& annotations = {});
  IntVar(int64_t, const std::string&,
         std::vector<fznparser::Annotation>&& annotations = {});
  IntVar(int64_t lb, int64_t ub, const std::string&,
         std::vector<fznparser::Annotation>&& annotations = {});
  IntVar(std::vector<int64_t>&&, const std::string&,
         std::vector<fznparser::Annotation>&& annotations = {});

  [[nodiscard]] const IntSet& domain() const;

  [[nodiscard]] bool contains(const int64_t& val) const;
  [[nodiscard]] int64_t lowerBound() const;
  [[nodiscard]] int64_t upperBound() const;

  bool operator==(const IntVar&) const;
  bool operator!=(const IntVar&) const;

  [[nodiscard]] bool isFixed() const override;
  [[nodiscard]] std::string toString() const override;
};

class FloatVar : public VarBase {
  FloatSet _domain;

 public:
  FloatVar(const FloatVar&) = default;
  FloatVar(FloatVar&&) = default;

  [[nodiscard]] const FloatSet& domain() const;

  explicit FloatVar(const std::string&,
                    std::vector<fznparser::Annotation>&& = {});
  FloatVar(double, const std::string&,
           std::vector<fznparser::Annotation>&& = {});
  FloatVar(double lb, double ub, const std::string&,
           std::vector<fznparser::Annotation>&& = {});
  FloatVar(std::vector<double>&&, const std::string&,
           std::vector<fznparser::Annotation>&& = {});

  [[nodiscard]] bool contains(const double& val) const;
  [[nodiscard]] double lowerBound() const;
  [[nodiscard]] double upperBound() const;

  bool operator==(const FloatVar&) const;
  bool operator!=(const FloatVar&) const;

  [[nodiscard]] bool isFixed() const override;
  [[nodiscard]] std::string toString() const override;
};

class SetVar : public VarBase {
  IntSet _domain;

 public:
  SetVar(const SetVar&) = default;
  SetVar(SetVar&&) = default;

  SetVar(int64_t lb, int64_t ub, const std::string&,
         std::vector<fznparser::Annotation>&& = {});
  SetVar(std::vector<int64_t>&&, const std::string&,
         std::vector<fznparser::Annotation>&& = {});
  SetVar(IntSet&&, const std::string&,
         std::vector<fznparser::Annotation>&& = {});
  SetVar(const IntSet&, const std::string&,
         std::vector<fznparser::Annotation>&& = {});

  [[nodiscard]] bool contains(const IntSet& val) const;
  [[nodiscard]] IntSet lowerBound() const;
  [[nodiscard]] IntSet upperBound() const;

  bool operator==(const SetVar&) const;
  bool operator!=(const SetVar&) const;

  [[nodiscard]] bool isFixed() const override;
  [[nodiscard]] std::string toString() const override;
};

class Var;  // forward declaration

class VarArrayBase : public VarBase {
 private:
  std::vector<int64_t> _outputIndexSetSizes{};

 public:
  VarArrayBase(const VarArrayBase&) = default;
  VarArrayBase(VarArrayBase&&) = default;
  VarArrayBase(const std::string&, std::vector<fznparser::Annotation>&&);

  void interpretAnnotations(
      const std::unordered_map<std::string, Var>&) override;

  [[nodiscard]] const std::vector<int64_t>& outputIndexSetSizes() const;
};

template <typename ParType, typename VarType>
class VarArrayTemplate : public VarArrayBase {
 protected:
  std::vector<std::variant<ParType, std::shared_ptr<const VarType>>> _vars;
  VarArrayTemplate(const std::string& identifier,
                   std::vector<Annotation>&& annotations)
      : VarArrayBase(identifier, std::move(annotations)){};

 public:
  VarArrayTemplate(const VarArrayTemplate&) = default;
  VarArrayTemplate(VarArrayTemplate&&) = default;

  [[nodiscard]] bool isParArray() const {
    return std::all_of(_vars.begin(), _vars.end(), [&](const auto& var) {
      return std::holds_alternative<ParType>(var);
    });
  };

  [[nodiscard]] bool isFixed() const override { return isParArray(); };

  [[nodiscard]] std::vector<ParType> toParVector() const {
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

  virtual std::vector<std::shared_ptr<const VarType>> toVarVector(
      fznparser::Model&) = 0;

  void append(const ParType& par) { _vars.emplace_back(par); };
  void append(std::shared_ptr<VarType> var) { _vars.emplace_back(var); };
  void append(const VarType& var) {
    _vars.emplace_back(std::make_shared<VarType>(var));
  };
  [[nodiscard]] std::string toString() const override = 0;
  [[nodiscard]] size_t size() const { return _vars.size(); };
  std::variant<ParType, std::shared_ptr<const VarType>> operator[](
      size_t index) const {
    return _vars[index];
  };
  [[nodiscard]] std::variant<ParType, std::shared_ptr<const VarType>> at(
      size_t index) const {
    return _vars.at(index);
  };
};

class BoolVarArray : public VarArrayTemplate<bool, BoolVar> {
 public:
  BoolVarArray(const BoolVarArray&) = default;
  BoolVarArray(BoolVarArray&&) = default;
  explicit BoolVarArray(const std::string&,
                        std::vector<fznparser::Annotation>&& = {});

  [[nodiscard]] std::vector<std::shared_ptr<const BoolVar>> toVarVector(
      fznparser::Model&) override;

  bool operator==(const BoolVarArray&) const;
  bool operator!=(const BoolVarArray&) const;
  [[nodiscard]] std::string toString() const override;
};

class IntVarArray : public VarArrayTemplate<int64_t, IntVar> {
 public:
  IntVarArray(const IntVarArray&) = default;
  IntVarArray(IntVarArray&&) = default;
  explicit IntVarArray(const std::string&,
                       std::vector<fznparser::Annotation>&& = {});

  [[nodiscard]] std::vector<std::shared_ptr<const IntVar>> toVarVector(
      fznparser::Model&) override;

  bool operator==(const IntVarArray&) const;
  bool operator!=(const IntVarArray&) const;
  [[nodiscard]] std::string toString() const override;
};

class FloatVarArray : public VarArrayTemplate<double, FloatVar> {
 public:
  FloatVarArray(const FloatVarArray&) = default;
  FloatVarArray(FloatVarArray&&) = default;
  explicit FloatVarArray(const std::string& identifier,
                         std::vector<fznparser::Annotation>&& = {});

  [[nodiscard]] std::vector<std::shared_ptr<const FloatVar>> toVarVector(
      fznparser::Model&) override;

  bool operator==(const FloatVarArray&) const;
  bool operator!=(const FloatVarArray&) const;
  [[nodiscard]] std::string toString() const override;
};

class SetVarArray : public VarArrayTemplate<IntSet, SetVar> {
 public:
  SetVarArray(const SetVarArray&) = default;
  SetVarArray(SetVarArray&&) = default;
  explicit SetVarArray(const std::string&,
                       std::vector<fznparser::Annotation>&& = {});

  [[nodiscard]] std::vector<std::shared_ptr<const SetVar>> toVarVector(
      fznparser::Model&) override;

  bool operator==(const SetVarArray&) const;
  bool operator!=(const SetVarArray&) const;
  [[nodiscard]] std::string toString() const override;
};

class Var : public std::variant<
                std::shared_ptr<BoolVar>, std::shared_ptr<IntVar>,
                std::shared_ptr<FloatVar>, std::shared_ptr<SetVar>,
                std::shared_ptr<BoolVarArray>, std::shared_ptr<IntVarArray>,
                std::shared_ptr<FloatVarArray>, std::shared_ptr<SetVarArray>,
                std::shared_ptr<VarReference>> {
 public:
  using std::variant<
      std::shared_ptr<BoolVar>, std::shared_ptr<IntVar>,
      std::shared_ptr<FloatVar>, std::shared_ptr<SetVar>,
      std::shared_ptr<BoolVarArray>, std::shared_ptr<IntVarArray>,
      std::shared_ptr<FloatVarArray>, std::shared_ptr<SetVarArray>,
          std::shared_ptr<VarReference>>::variant;

  [[nodiscard]] bool isOutput() const;
  [[nodiscard]] const std::string& identifier() const;
  void interpretAnnotations(const std::unordered_map<std::string, Var>&);
  [[nodiscard]] bool isArray() const;
  bool operator==(const Var&) const;
  bool operator!=(const Var&) const;
  [[nodiscard]] bool isFixed() const;
  [[nodiscard]] std::string toString() const;
};

class VarReference : public VarBase {
 private:
  Var _source;
 public:
  VarReference(const VarReference&) = default;
  VarReference(VarReference&&) = default;
  VarReference(const std::string&, Var, std::vector<fznparser::Annotation>&& = {});
  [[nodiscard]] const Var& source() const;
  [[nodiscard]] bool isFixed() const override;
  [[nodiscard]] bool operator==(const VarReference&) const;
  [[nodiscard]] bool operator!=(const VarReference&) const;
  [[nodiscard]] std::string toString() const override;
};

}  // namespace fznparser