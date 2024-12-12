#include "fznparser/parser.hpp"

#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <istream>

#include "fznparser/except.hpp"
#include "fznparser/parser/grammarDef.hpp"
#include "fznparser/transformer/modelTransformer.hpp"

namespace fznparser {
namespace x3 = ::boost::spirit::x3;

Model parseFznIstream(std::istream& fznStream) {
  boost::spirit::istream_iterator fileIterator(fznStream >> std::noskipws), eof;

  parser::Model parserModel;

  x3::phrase_parse(fileIterator, eof, parser::model, parser::skipper,
                   parserModel);

  if (fileIterator != eof) {
    throw FznException("Could not parse FlatZinc");
  }

  ModelTransformer modelTransformer(std::move(parserModel));

  return modelTransformer.generateModel();
}

Model parseFznFile(const std::string& fznFilePath) {
  std::ifstream fznFile(fznFilePath);

  if (!fznFile) {
    throw FznException("Could not open file: " + fznFilePath);
  }

  return parseFznIstream(fznFile);
}

Model parseFznString(const std::string& fznContent) {
  std::stringstream ss(fznContent);
  return parseFznIstream(ss);
}
Model parseFznString(std::string&& fznContent) {
  std::stringstream ss(fznContent);
  return parseFznIstream(ss);
}

}  // namespace fznparser
