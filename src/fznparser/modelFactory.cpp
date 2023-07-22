#include "fznparser/modelFactory.hpp"

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

#include "fznparser/parser/grammarDef.hpp"

namespace fznparser {
namespace x3 = ::boost::spirit::x3;
void ModelFactory::parse(const std::filesystem::path &modelFile) {
  std::ifstream fznFile(modelFile);
  if (!fznFile) {
    return;
  }
  ::boost::spirit::istream_iterator fileIterator(fznFile >> std::noskipws), eof;
  x3::parse(fileIterator, eof, parser::identifier);
  fznFile.close();
}
}  // namespace fznparser