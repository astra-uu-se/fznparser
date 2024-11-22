#include <gtest/gtest.h>

#include "fznparser/variables.hpp"

namespace fznparser::testing {

using namespace fznparser;

TEST(BoolVarArrayTest, isFixed) {
  auto array = BoolVarArray("array");
  array.append(false);

  auto par = std::make_shared<BoolVar>(false, "par");
  array.append(par);

  EXPECT_TRUE(array.isFixed());

  auto var = std::make_shared<BoolVar>("var");
  array.append(var);

  EXPECT_FALSE(array.isFixed());
}

TEST(IntVarArrayTest, isFixed) {
  auto array = IntVarArray("array");
  array.append(0);

  auto par = std::make_shared<IntVar>(1, "par");
  array.append(par);

  EXPECT_TRUE(array.isFixed());

  auto var = std::make_shared<IntVar>(2, 3, "var");
  array.append(var);

  EXPECT_FALSE(array.isFixed());
}

}  // namespace fznparser::testing