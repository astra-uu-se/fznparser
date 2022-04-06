#include <gtest/gtest.h>

#include "fznparser/model.hpp"

using namespace fznparser;

TEST(Model, lookup_of_parameters) {
  Parameter n{"n", Int(4)};
  FZNModel model({n}, {}, {}, Satisfy{});

  EXPECT_EQ(model.identify("n"), FZNModel::Identifiable(n));
}

TEST(Model, lookup_of_variables) {
  SearchVariable n{"n", BasicDomain<Int>{}, {}, {}};
  FZNModel model({}, {n}, {}, Satisfy{});

  EXPECT_EQ(model.identify("n"), FZNModel::Identifiable(n));
}

TEST(Model, lookup_of_undefined_literal) {
  SearchVariable n{"n", BasicDomain<Int>{}, {}, {}};
  FZNModel model({}, {n}, {}, Satisfy{});

  EXPECT_EQ(model.identify("other"), std::optional<FZNModel::Identifiable>{});
}
