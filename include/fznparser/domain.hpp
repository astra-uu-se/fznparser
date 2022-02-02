#pragma once

#include <cstdint>

namespace fznparser {
  enum DomainType { INT };

  class Domain {
  public:
    virtual int64_t lowerBound() = 0;
    virtual int64_t upperBound() = 0;
    virtual int64_t size() = 0;

    virtual DomainType type() = 0;
  };

  class IntDomain : public Domain {
  private:
    int64_t _lb;
    int64_t _ub;

  public:
    IntDomain(int64_t lb, int64_t ub) : _lb(lb), _ub(ub) {}

    int64_t size() override { return _ub - _lb; }
    int64_t lowerBound() override { return _lb; }
    int64_t upperBound() override { return _ub; }

    DomainType type() override { return INT; }
  };
}  // namespace fznparser