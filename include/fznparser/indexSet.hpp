#pragma once

namespace fznparser {
struct indexSet {
  const size_t lowerBound = 0;
  const size_t upperBound;
  indexSet(size_t ub) : upperBound(ub) {}
}

}  // namespace fznparser
