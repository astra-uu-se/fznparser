#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "fznparser/model.hpp"

namespace fznparser {

Model parseFznIstream(std::istream& fznStream);
Model parseFznFile(const std::string& fznFilePath);
Model parseFznString(const std::string& fznContent);
Model parseFznString(std::string&& fznContent);

}  // namespace fznparser
