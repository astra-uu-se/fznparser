#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <boost/spirit/home/x3.hpp>
#include <fstream>
#include <iostream>
#include <boost/spirit/include/support_istream_iterator.hpp>

#include "fznparser/grammar_ast.hpp"
#include "fznparser/grammar.hpp"
#include "fznparser/grammar_def.hpp"

namespace fznparser
{
  class ModelFactory
  {
  public:
    static void parse(const std::filesystem::path &modelPath);
  };
} // namespace fznparser