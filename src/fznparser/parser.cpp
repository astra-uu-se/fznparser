#include "fznparser/parser.hpp"

#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

#include "fznparser/parser/grammarDef.hpp"
#include "fznparser/transformer/modelTransformer.hpp"

namespace fznparser {
namespace x3 = ::boost::spirit::x3;

Model parseFznFile(const std::string &fznFilePath) {
  std::ifstream fznFile(fznFilePath);

  if (!fznFile) {
    throw FznException("Could not open file: " + fznFilePath);
  }

  boost::spirit::istream_iterator fileIterator(fznFile >> std::noskipws), eof;

  parser::Model parserModel;

  x3::phrase_parse(fileIterator, eof, parser::model, x3::standard::space,
                   parserModel);

  if (fileIterator != eof) {
    throw FznException("Could not parse file: " + fznFilePath);
  }

  ModelTransformer modelTransformer(std::move(parserModel));

  return modelTransformer.generateModel();
}

}  // namespace fznparser