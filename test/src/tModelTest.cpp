#include <gtest/gtest.h>

#include "fznparser/model.hpp"

using namespace fznparser;

TEST(Model, lookup_of_parameters) {
  Parameter n = IntParameter{"n", Int(4)};
  FZNModel model({n}, {}, {}, Satisfy{});

  EXPECT_EQ(model.identify("n"), FZNModel::Identifiable(n));
}

TEST(Model, lookup_of_variables) {
  IntVariable n{"n", BasicDomain<Int>{}, {}, {}};
  FZNModel model({}, {n}, {}, Satisfy{});

  EXPECT_EQ(model.identify("n"), FZNModel::Identifiable(n));
}

TEST(Model, lookup_of_undefined_literal) {
  IntVariable n{"n", BasicDomain<Int>{}, {}, {}};
  FZNModel model({}, {n}, {}, Satisfy{});

  EXPECT_EQ(model.identify("other"), std::optional<FZNModel::Identifiable>{});
}
