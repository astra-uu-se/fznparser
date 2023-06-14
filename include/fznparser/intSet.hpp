#pragma once

#include <numeric>
#include <variant>
#include <vector>

namespace fznparser {

inline std::vector<int64_t> vec_to_set(const std::vector<int64_t> vals) {
  std::vector<int64_t> values(vals);
  std::sort(values.begin(), values.end());
  values.erase(std::unique(values.begin(), values.end()), values.end());
  return values;
}

class IntSet {
  std::variant<std::pair<int64_t, int64_t>, std::vector<int64_t>> values;

 public:
  IntSet(const IntSet &o) = default;
  IntSet(IntSet &&o) = default;

  IntSet(const std::pair<int64_t, int64_t> &bounds) : values(bounds) {
    assert(bounds.first <= bounds.second);
  }

  IntSet(const std::vector<int64_t> &vals) : values{vec_to_set(vals)} {}

  int64_t lowerBound() const {
    return std::visit(
        {
            [](const std::pair<int64_t, int64_t> &bounds) {
              return bounds.first;
            },
            [](const std::vector<int64_t> &vals) { return vals.front(); },
        },
        values);
  }

  int64_t upperBound() const {
    return std::visit(
        {
            [](const std::pair<int64_t, int64_t> &bounds) {
              return bounds.second;
            },
            [](const std::vector<int64_t> &vals) { return vals.back(); },
        },
        values);
  }

  bool isInterval() const {
    return std::holds_alternative<std::pair<int64_t, int64_t>>(values);
  }

  const std::vector<int64_t> &interval() {
    return std::visit(
        {
            [](const std::pair<int64_t, int64_t> bounds) {
              std::vector<int64_t> vals(bounds.second - bounds.first + 1);
              std::iota(vals.begin(), vals.end(), bounds.first);
              values = vals;
            },
            [](const std::vector<int64_t> &vals) { return vals; },
        },
        values);
  }
};
}  // namespace fznparser