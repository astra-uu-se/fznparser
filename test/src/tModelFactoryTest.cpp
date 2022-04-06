#include <gtest/gtest.h>

#include "fznparser/modelFactory.hpp"

using namespace fznparser;

TEST(ModelFactoryTest, empty_satisfy_is_parsed) {
  auto model = ModelFactory::create(STUB_DIR "/satisfy-empty.fzn");

  EXPECT_TRUE(std::holds_alternative<fznparser::Satisfy>(model.objective()));
}
