#include "fznparser/modelFactory.hpp"

namespace fznparser
{
  namespace x3 = ::boost::spirit::x3;
  void ModelFactory::parse(const std::filesystem::path &modelFile)
  {
    std::ifstream fznFile(modelFile);
    if (!fznFile)
    {
      return;
    }
    ::boost::spirit::istream_iterator fileIterator(fznFile >> std::noskipws), eof;
    x3::parse(fileIterator, eof, parser::identifier);
    fznFile.close();
  }
}