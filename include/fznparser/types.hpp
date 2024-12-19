#pragma once

#include <cstdint>
#include <variant>
#include <vector>

namespace fznparser {

class IntSet {
  std::variant<std::pair<int64_t, int64_t>, std::vector<int64_t>> _elements;

 public:
  IntSet() = delete;
  IntSet(const IntSet&) = default;
  IntSet(IntSet&&) = default;
  /**
   * @brief Construct a singleton set
   */
  explicit IntSet(int64_t);
  IntSet(int64_t lb, int64_t ub);
  /**
   * @brief Sorts and removes duplicates from the vector
   */
  explicit IntSet(std::vector<int64_t>&&);

  [[nodiscard]] bool contains(int64_t) const;
  [[nodiscard]] size_t size() const;
  [[nodiscard]] int64_t lowerBound() const;
  [[nodiscard]] int64_t upperBound() const;

  [[nodiscard]] bool isInterval() const;

  const std::vector<int64_t>& populateElements();

  [[nodiscard]] const std::vector<int64_t>& elements() const;

  bool operator==(const IntSet&) const;
  bool operator!=(const IntSet&) const;
};

class FloatSet {
  std::variant<std::pair<double, double>, std::vector<double>> _elements;

 public:
  FloatSet() = delete;
  FloatSet(const FloatSet&) = default;
  FloatSet(FloatSet&&) = default;

  explicit FloatSet(double);
  FloatSet(double lb, double ub);
  /**
   * @brief Sorts and removes duplicates from the vector
   */
  explicit FloatSet(std::vector<double>&&);

  [[nodiscard]] bool contains(double) const;
  [[nodiscard]] double lowerBound() const;
  [[nodiscard]] double upperBound() const;

  [[nodiscard]] bool isInterval() const;
  [[nodiscard]] const std::vector<double>& elements() const;

  bool operator==(const FloatSet&) const;
  bool operator!=(const FloatSet&) const;
};

class FloatSetArray : public std::vector<FloatSet> {
 public:
  bool operator==(const FloatSetArray&) const;
  bool operator!=(const FloatSetArray&) const;
};

}  // namespace fznparser
