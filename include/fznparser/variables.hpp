#pragma once

#include <array>
#include <functional>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <variant>
#include <vector>

#include "fznparser/annotation.hpp"
#include "fznparser/except.hpp"
#include "fznparser/types.hpp"

namespace fznparser {

class Model;  // forward declaration

class BoolVar {
  std::string_view _identifier;
  std::vector<Annotation> _annotations;
  signed char _domain;

 public:
  BoolVar(const BoolVar&) = default;
  BoolVar(BoolVar&&) = default;
  BoolVar(const std::string_view&, std::vector<Annotation>&& = {});
  BoolVar(bool, const std::string_view&, std::vector<Annotation>&& = {});

  const std::string_view& identifier() const;
  bool contains(const bool& val) const;
  bool lowerBound() const;
  bool upperBound() const;
  void addAnnotation(const Annotation&);
  void addAnnotation(const std::string_view&);
  void addAnnotation(const std::string_view&,
                     std::vector<AnnotationExpression>&&);
  void addAnnotation(const std::string_view&,
                     std::vector<std::vector<AnnotationExpression>>&&);

  bool operator==(const BoolVar&) const;
  bool operator!=(const BoolVar&) const;
  std::string toString() const;
};

class IntVar {
  std::string_view _identifier;
  std::vector<Annotation> _annotations;
  IntSet _domain;

 public:
  IntVar(const IntVar&) = default;
  IntVar(IntVar&&) = default;
  IntVar(const std::string_view&, std::vector<Annotation>&& annotations = {});
  IntVar(int64_t, const std::string_view&,
         std::vector<Annotation>&& annotations = {});
  IntVar(int64_t lb, int64_t ub, const std::string_view&,
         std::vector<Annotation>&& annotations = {});
  IntVar(std::vector<int64_t>&&, const std::string_view&,
         std::vector<Annotation>&& annotations = {});

  const std::string_view& identifier() const;
  bool contains(const int64_t& val) const;
  int64_t lowerBound() const;
  int64_t upperBound() const;

  void addAnnotation(const Annotation&);
  void addAnnotation(const std::string_view&);
  void addAnnotation(const std::string_view&, AnnotationExpression&&);
  void addAnnotation(const std::string_view&,
                     std::vector<AnnotationExpression>&&);

  bool operator==(const IntVar&) const;
  bool operator!=(const IntVar&) const;
  std::string toString() const;
};

class FloatVar {
  std::string_view _identifier;
  std::vector<Annotation> _annotations;
  FloatSet _domain;

 public:
  FloatVar(const FloatVar&) = default;
  FloatVar(FloatVar&&) = default;
  FloatVar(const std::string_view&, std::vector<Annotation>&& = {});
  FloatVar(double, const std::string_view&, std::vector<Annotation>&& = {});
  FloatVar(double lb, double ub, const std::string_view&,
           std::vector<Annotation>&& = {});
  FloatVar(std::vector<double>&&, const std::string_view&,
           std::vector<Annotation>&& = {});

  const std::string_view& identifier() const;
  bool contains(const double& val) const;
  double lowerBound() const;
  double upperBound() const;

  bool operator==(const FloatVar&) const;
  bool operator!=(const FloatVar&) const;
  std::string toString() const;
};

class SetVar {
  std::string_view _identifier;
  std::vector<Annotation> _annotations;
  IntSet _domain;

 public:
  SetVar(const SetVar&) = default;
  SetVar(SetVar&&) = default;

  SetVar(int64_t lb, int64_t ub, const std::string_view&,
         std::vector<Annotation>&& = {});
  SetVar(std::vector<int64_t>&&, const std::string_view&,
         std::vector<Annotation>&& = {});
  SetVar(IntSet&&, const std::string_view&, std::vector<Annotation>&& = {});
  SetVar(const IntSet&, const std::string_view&,
         std::vector<Annotation>&& = {});

  const std::string_view& identifier() const;
  bool contains(const IntSet& val) const;
  IntSet lowerBound() const;
  IntSet upperBound() const;

  bool operator==(const SetVar&) const;
  bool operator!=(const SetVar&) const;
  std::string toString() const;
};

class BoolVarArray
    : public std::vector<
          std::variant<bool, std::reference_wrapper<const BoolVar>>> {
 private:
  std::string_view _identifier;
  std::vector<Annotation> _annotations;

 public:
  BoolVarArray(const BoolVarArray&) = default;
  BoolVarArray(BoolVarArray&&) = default;
  BoolVarArray(const std::string_view&, std::vector<Annotation>&& = {});

  const std::string_view& identifier() const;
  bool isParArray() const;
  std::vector<bool> toParVector() const;
  std::vector<std::reference_wrapper<const BoolVar>> toVarVector(
      fznparser::Model&);

  void addArgument(const BoolVar&);
  void addArgument(bool);

  bool operator==(const BoolVarArray&) const;
  bool operator!=(const BoolVarArray&) const;
  std::string toString() const;
};

class IntVarArray
    : public std::vector<
          std::variant<int64_t, std::reference_wrapper<const IntVar>>> {
 private:
  std::string_view _identifier;
  std::vector<Annotation> _annotations;

 public:
  IntVarArray(const IntVarArray&) = default;
  IntVarArray(IntVarArray&&) = default;
  IntVarArray(const std::string_view&, std::vector<Annotation>&& = {});

  const std::string_view& identifier() const;
  bool isParArray() const;
  std::vector<int64_t> toParVector() const;
  std::vector<std::reference_wrapper<const IntVar>> toVarVector(
      fznparser::Model&);

  void addArgument(const IntVar&);
  void addArgument(int64_t);

  void addAnnotation(const Annotation&);
  void addAnnotation(const std::string_view&);
  void addAnnotation(const std::string_view&, AnnotationExpression&&);
  void addAnnotation(const std::string_view&,
                     std::vector<AnnotationExpression>&&);

  bool operator==(const IntVarArray&) const;
  bool operator!=(const IntVarArray&) const;
  std::string toString() const;
};

class FloatVarArray
    : public std::vector<
          std::variant<double, std::reference_wrapper<const FloatVar>>> {
 private:
  std::string_view _identifier;
  std::vector<Annotation> _annotations;

 public:
  FloatVarArray(const FloatVarArray&) = default;
  FloatVarArray(FloatVarArray&&) = default;
  FloatVarArray(const std::string_view& identifier,
                std::vector<Annotation>&& = {});

  const std::string_view& identifier() const;
  bool isParArray() const;
  std::vector<double> toParVector() const;
  std::vector<std::reference_wrapper<const FloatVar>> toVarVector(
      fznparser::Model&);

  void addArgument(const FloatVar&);
  void addArgument(double);

  bool operator==(const FloatVarArray&) const;
  bool operator!=(const FloatVarArray&) const;
  std::string toString() const;
};

class SetVarArray
    : public std::vector<
          std::variant<IntSet, std::reference_wrapper<const SetVar>>> {
 private:
  std::string_view _identifier;
  std::vector<Annotation> _annotations;

 public:
  SetVarArray(const SetVarArray&) = default;
  SetVarArray(SetVarArray&&) = default;
  SetVarArray(const std::string_view& identifier,
              std::vector<Annotation>&& = {});

  const std::string_view& identifier() const;
  bool isParArray() const;
  std::vector<IntSet> toParVector() const;
  std::vector<std::reference_wrapper<const SetVar>> toVarVector(
      fznparser::Model&);

  void addArgument(const SetVar&);
  void addArgument(IntSet&&);

  bool operator==(const SetVarArray&) const;
  bool operator!=(const SetVarArray&) const;
  std::string toString() const;
};

class Variable
    : public std::variant<BoolVar, IntVar, FloatVar, SetVar, BoolVarArray,
                          IntVarArray, FloatVarArray, SetVarArray> {
 public:
  using std::variant<BoolVar, IntVar, FloatVar, SetVar, BoolVarArray,
                     IntVarArray, FloatVarArray, SetVarArray>::variant;

  const std::string_view& identifier() const;
  bool isArray() const;
  bool operator==(const Variable&) const;
  bool operator!=(const Variable&) const;
  std::string toString() const;
};

}  // namespace fznparser