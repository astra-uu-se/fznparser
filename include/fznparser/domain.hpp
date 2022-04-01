#pragma once

#include <cstdint>

namespace fznparser {
  enum DomainType { BOOL, INTERVAL, SET };

  class Domain {
  public:
    virtual ~Domain() = default;

    virtual int64_t lowerBound() = 0;
    virtual int64_t upperBound() = 0;
    virtual int64_t size() = 0;

    virtual DomainType type() = 0;
  };

  class IntervalDomain : public Domain {
  private:
    int64_t _lb;
    int64_t _ub;

  public:
    IntervalDomain(int64_t lb, int64_t ub) : _lb(lb), _ub(ub) {}
    ~IntervalDomain() override = default;

    int64_t size() override { return _ub - _lb + 1; }
    int64_t lowerBound() override { return _lb; }
    int64_t upperBound() override { return _ub; }

    DomainType type() override { return INTERVAL; }
  };

  class SetDomain : public Domain {
  private:
    std::vector<int64_t> _values;

  public:
    SetDomain(std::vector<int64_t> values) : _values(std::move(values)) {}
    ~SetDomain() override = default;

    int64_t size() override { return static_cast<int64_t>(_values.size()); }
    int64_t lowerBound() override { return _values.front(); }
    int64_t upperBound() override { return _values.back(); }

    DomainType type() override { return SET; }
  };

  class BoolDomain : public Domain {
  public:
    BoolDomain() = default;
    ~BoolDomain() override = default;

    int64_t size() override { return 2; }
    int64_t lowerBound() override { return 0; }
    int64_t upperBound() override { return 1; }

    DomainType type() override { return BOOL; }
  };
}  // namespace fznparser