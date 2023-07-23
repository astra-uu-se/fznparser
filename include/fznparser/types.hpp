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

namespace fznparser {

class IntSet {
 private:
  std::variant<std::pair<int64_t, int64_t>, std::vector<int64_t>> _elements;

 public:
  IntSet() = delete;
  IntSet(const IntSet&) = default;
  IntSet(IntSet&&) = default;
  /**
   * @brief Construct a singleton set
   */
  IntSet(int64_t);
  IntSet(int64_t lb, int64_t ub);
  /**
   * @brief Sorts and removes duplicates from the vector
   *
   * @param elems the elements of the set
   */
  IntSet(std::vector<int64_t>&& elems);

  bool contains(int64_t) const;
  size_t size() const;
  int64_t lowerBound() const;
  int64_t upperBound() const;

  const std::vector<int64_t>& populateElements();

  const std::vector<int64_t>& elements() const;

  bool operator==(const IntSet&) const;
  bool operator!=(const IntSet&) const;

  std::string toString() const;
};

class FloatSet {
 private:
  std::variant<std::pair<double, double>, std::vector<double>> _elements;

 public:
  FloatSet() = delete;
  FloatSet(const FloatSet&) = default;
  FloatSet(FloatSet&&) = default;

  FloatSet(double);
  FloatSet(double lb, double ub);
  /**
   * @brief Sorts and removes duplicates from the vector
   *
   * @param elems the elements of the set
   */
  FloatSet(std::vector<double>&& elems);

  bool contains(double) const;
  size_t size() const;
  double lowerBound() const;
  double upperBound() const;

  const std::vector<double>& elements() const;

  bool operator==(const FloatSet&) const;
  bool operator!=(const FloatSet&) const;

  std::string toString() const;
};

class FloatSetArray : public std::vector<FloatSet> {
 public:
  bool operator==(const FloatSetArray&) const;
  bool operator!=(const FloatSetArray&) const;
  std::string toString() const;
};

}  // namespace fznparser
