#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "fznparser/model.hpp"

namespace fznparser {

Model parseFznFile(const std::string& fznFilePath);

}  // namespace fznparser