#include "fznparser/types.hpp"

#include <iostream>

using std::get;

std::vector<int64_t> toIntSet(std::vector<int64_t>& vals) {
  std::sort(vals.begin(), vals.end());
  vals.erase(std::unique(vals.begin(), vals.end()), vals.end());
  return vals;
}

std::vector<double> toFloatSet(std::vector<double>& vals) {
  std::sort(vals.begin(), vals.end());
  vals.erase(std::unique(vals.begin(), vals.end()), vals.end());
  return vals;
}

namespace fznparser {
IntSet::IntSet(int64_t val) : _elements(std::make_pair(val, val)) {}
IntSet::IntSet(int64_t lb, int64_t ub) : _elements(std::make_pair(lb, ub)) {
  if (lb > ub) {
    throw std::runtime_error("Lower bound cannot be greater than upper bound");
  }
}

IntSet::IntSet(std::vector<int64_t>&& vals)
    : _elements(std::move(toIntSet(vals))) {}

bool IntSet::contains(int64_t val) const {
  if (holds_alternative<std::pair<int64_t, int64_t>>(_elements)) {
    auto [lb, ub] = get<std::pair<int64_t, int64_t>>(_elements);
    return lb <= val && val <= ub;
  }
  return std::binary_search(get<std::vector<int64_t>>(_elements).begin(),
                            get<std::vector<int64_t>>(_elements).end(), val);
}

int64_t IntSet::lowerBound() const {
  return holds_alternative<std::pair<int64_t, int64_t>>(_elements)
             ? get<std::pair<int64_t, int64_t>>(_elements).first
             : get<std::vector<int64_t>>(_elements).front();
}

int64_t IntSet::upperBound() const {
  return holds_alternative<std::pair<int64_t, int64_t>>(_elements)
             ? get<std::pair<int64_t, int64_t>>(_elements).second
             : get<std::vector<int64_t>>(_elements).back();
}

const std::vector<int64_t>& IntSet::populateElements() {
  if (holds_alternative<std::pair<int64_t, int64_t>>(_elements)) {
    std::vector<int64_t> elems(upperBound() - lowerBound() + 1);
    std::iota(elems.begin(), elems.end(), lowerBound());
    _elements = std::move(elems);
  }
  return get<std::vector<int64_t>>(_elements);
}

const std::vector<int64_t>& IntSet::elements() const {
  if (holds_alternative<std::pair<int64_t, int64_t>>(_elements)) {
    throw std::runtime_error("Cannot get elements from interval");
  }
  return get<std::vector<int64_t>>(_elements);
}

bool IntSet::operator==(const IntSet& other) const {
  std::cout << "IntSet: comparing: " << toString() << " and "
            << other.toString() << '\n';
  if (holds_alternative<std::pair<int64_t, int64_t>>(_elements) &&
      holds_alternative<std::pair<int64_t, int64_t>>(other._elements)) {
    return lowerBound() == other.lowerBound() &&
           upperBound() == other.upperBound();
  } else if (holds_alternative<std::vector<int64_t>>(_elements) &&
             holds_alternative<std::vector<int64_t>>(other._elements)) {
    const std::vector<int64_t>& e1 = get<std::vector<int64_t>>(_elements);
    const std::vector<int64_t>& e2 = get<std::vector<int64_t>>(other._elements);
    if (e1.size() != e2.size()) {
      return false;
    }
    for (size_t i = 0; i < e1.size(); ++i) {
      if (e1[i] != e2[i]) {
        return false;
      }
    }
  }
  return true;
}

bool IntSet::operator!=(const IntSet& other) const {
  return !operator==(other);
}

std::string IntSet::toString() const {
  if (holds_alternative<std::pair<int64_t, int64_t>>(_elements)) {
    auto [lb, ub] = get<std::pair<int64_t, int64_t>>(_elements);
    return std::to_string(lb) + ".." + std::to_string(ub);
  }
  std::string str = "{";
  for (size_t i = 0; i < get<std::vector<int64_t>>(_elements).size(); ++i) {
    if (i != 0) {
      str += ", ";
    }
    str += std::to_string(get<std::vector<int64_t>>(_elements)[i]);
  }
  return str + "}";
}

FloatSet::FloatSet(double val) : _elements(std::make_pair(val, val)) {}
FloatSet::FloatSet(double lb, double ub) : _elements(std::make_pair(lb, ub)) {
  if (lb > ub) {
    throw std::runtime_error("Lower bound cannot be greater than upper bound");
  }
}

FloatSet::FloatSet(std::vector<double>&& vals)
    : _elements(std::move(toFloatSet(vals))) {}

bool FloatSet::contains(double val) const {
  if (holds_alternative<std::pair<double, double>>(_elements)) {
    auto [lb, ub] = get<std::pair<double, double>>(_elements);
    return lb <= val && val <= ub;
  }
  return std::binary_search(get<std::vector<double>>(_elements).begin(),
                            get<std::vector<double>>(_elements).end(), val);
}

double FloatSet::lowerBound() const {
  return holds_alternative<std::pair<double, double>>(_elements)
             ? get<std::pair<double, double>>(_elements).first
             : get<std::vector<double>>(_elements).front();
}

double FloatSet::upperBound() const {
  return holds_alternative<std::pair<double, double>>(_elements)
             ? get<std::pair<double, double>>(_elements).second
             : get<std::vector<double>>(_elements).back();
}

bool FloatSet::operator==(const FloatSet& other) const {
  std::cout << "FloatSet: comparing: " << toString() << " and "
            << other.toString() << '\n';
  if (holds_alternative<std::pair<double, double>>(_elements) &&
      holds_alternative<std::pair<double, double>>(other._elements)) {
    return get<std::pair<double, double>>(_elements) ==
           get<std::pair<double, double>>(other._elements);
  } else if (holds_alternative<std::vector<double>>(_elements) &&
             holds_alternative<std::vector<double>>(other._elements)) {
    const std::vector<double>& e1 = get<std::vector<double>>(_elements);
    const std::vector<double>& e2 = get<std::vector<double>>(other._elements);
    if (e1.size() != e2.size()) {
      return false;
    }
    for (size_t i = 0; i < e1.size(); ++i) {
      if (e1[i] != e2[i]) {
        return false;
      }
    }
  }
  return true;
}

bool FloatSet::operator!=(const FloatSet& other) const {
  return !operator==(other);
}

std::string FloatSet::toString() const {
  if (holds_alternative<std::pair<double, double>>(_elements)) {
    auto [lb, ub] = get<std::pair<double, double>>(_elements);
    return std::to_string(lb) + ".." + std::to_string(ub);
  }
  std::string str = "{";
  for (size_t i = 0; i < get<std::vector<double>>(_elements).size(); ++i) {
    if (i != 0) {
      str += ", ";
    }
    str += std::to_string(get<std::vector<double>>(_elements)[i]);
  }
  return str + "}";
}

const std::vector<double>& FloatSet::elements() const {
  if (holds_alternative<std::pair<double, double>>(_elements)) {
    throw std::runtime_error("Cannot get elements from interval");
  }
  return get<std::vector<double>>(_elements);
}

bool FloatSetArray::operator==(const FloatSetArray& other) const {
  if (size() != other.size()) {
    return false;
  }
  for (size_t i = 0; i < size(); ++i) {
    if (at(i) != other.at(i)) {
      return false;
    }
  }
  return true;
}

bool FloatSetArray::operator!=(const FloatSetArray& other) const {
  return !operator==(other);
}

std::string FloatSetArray::toString() const {
  std::string s = "[";
  for (size_t i = 0; i < size(); ++i) {
    if (i != 0) {
      s += ", ";
    }
    s += at(i).toString();
  }
  return s + "]";
}

}  // namespace fznparser