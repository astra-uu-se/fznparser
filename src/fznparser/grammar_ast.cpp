#include "fznparser/grammar_ast.hpp"

namespace fznparser {
inline std::vector<int64_t> convert(
    const std::initializer_list<int64_t> &v) noexcept {
  std::vector<int64_t> ret;
  ret.reserve(v.size());
  std::copy(v.begin(), v.end(), std::back_inserter(ret));
  return ret;
}

inline std::vector<double> convert(
    const std::initializer_list<double> &v) noexcept {
  std::vector<double> ret;
  ret.reserve(v.size());
  std::copy(v.begin(), v.end(), std::back_inserter(ret));
  return ret;
}

}  // namespace fznparser
