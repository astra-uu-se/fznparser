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

class Model;
class Var;
class VarReference;
class Annotation;
class AnnotationExpression;

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
};

class Var;  // forward declaration

class VarArrayBase : public VarBase {
  std::vector<int64_t> _outputIndexSetSizes{};

 public:
  VarArrayBase(const VarArrayBase&) = default;
  VarArrayBase(VarArrayBase&&) = default;
  VarArrayBase(const std::string&, std::vector<fznparser::Annotation>&&);

  void interpretAnnotations(
      const std::unordered_map<std::string, Var>&) override;

  [[nodiscard]] const std::vector<int64_t>& outputIndexSetSizes() const;
};

template <typename ParType, class VarType>
class VarArrayTemplate : public VarArrayBase {
  static_assert(std::is_base_of_v<VarBase, VarType>,
                "VarType must inherit VarBase");

 protected:
  std::vector<std::variant<ParType, std::shared_ptr<const VarType>>> _vars;
  VarArrayTemplate(const std::string& identifier,
                   std::vector<Annotation>&& annotations);

 public:
  VarArrayTemplate(const VarArrayTemplate&) = default;
  VarArrayTemplate(VarArrayTemplate&&) = default;

  [[nodiscard]] bool isParArray() const;

  [[nodiscard]] bool isFixed() const override;

  [[nodiscard]] std::vector<ParType> toParVector() const;

  virtual std::vector<std::shared_ptr<const VarType>> toVarVector(
      fznparser::Model&) = 0;

  void append(const ParType& par);
  void append(std::shared_ptr<VarType> var);
  void append(const VarType& var);
  [[nodiscard]] size_t size() const;
  std::variant<ParType, std::shared_ptr<const VarType>> operator[](
      size_t index) const;
  [[nodiscard]] std::variant<ParType, std::shared_ptr<const VarType>> at(
      size_t index) const;
};
template class VarArrayTemplate<bool, BoolVar>;
template class VarArrayTemplate<int64_t, IntVar>;
template class VarArrayTemplate<double, FloatVar>;
template class VarArrayTemplate<IntSet, SetVar>;

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
  void interpretAnnotations(const std::unordered_map<std::string, Var>&) const;
  [[nodiscard]] bool isArray() const;
  bool operator==(const Var&) const;
  bool operator!=(const Var&) const;
  [[nodiscard]] bool isFixed() const;
};

class VarReference : public VarBase {
  Var _source;

 public:
  VarReference(const VarReference&) = default;
  VarReference(VarReference&&) = default;
  VarReference(const std::string&, Var,
               std::vector<fznparser::Annotation>&& = {});
  [[nodiscard]] const Var& source() const;
  [[nodiscard]] bool isFixed() const override;
  [[nodiscard]] bool operator==(const VarReference&) const;
  [[nodiscard]] bool operator!=(const VarReference&) const;
};

}  // namespace fznparser