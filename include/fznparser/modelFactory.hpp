#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

namespace fznparser {
class ModelFactory {
 public:
  static void parse(const std::filesystem::path &modelPath);
};
}  // namespace fznparser