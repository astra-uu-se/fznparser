#include "expect_eq.hpp"

namespace fznparser::testing {

using namespace fznparser::parser;

namespace x3 = ::boost::spirit::x3;

template <class T>
void expect_eq_type(const T &actual, const T &expected,
                    const std::string &input) {
  EXPECT_TRUE(actual.type() == expected.type())
      << "Expected type: " << expected.type().name()
      << " but got: " << actual.type().name() << "\n\"" << input << "\"";
}

template <class T>
void expect_eq_bounded(const T &actual, const T &expected,
                       const std::string &input) {
  expect_eq(actual.lowerBound, expected.lowerBound, input);
  expect_eq(actual.upperBound, expected.upperBound, input);
}

template <class T>
void expect_eq_vector(const T &actual, const T &expected,
                      const std::string &input) {
  EXPECT_EQ(actual.size(), expected.size()) << ("\"" + input + "\"");
  for (size_t i = 0; i < actual.size(); ++i) {
    expect_eq(actual.at(i), expected.at(i), input);
  }
}

void expect_eq(const int64_t &actual, const int64_t &expected,
               const std::string &input) {
  EXPECT_EQ(actual, expected) << ("\"" + input + "\"");
}

void expect_eq(const double &actual, const double &expected,
               const std::string &input) {
  EXPECT_EQ(actual, expected) << ("\"" + input + "\"");
}

void expect_eq(const std::string &actual, const std::string &expected,
               const std::string &input) {
  EXPECT_EQ(actual, expected) << ("\"" + input + "\"");
}

void expect_eq(const BasicParType &actual, const BasicParType &expected,
               const std::string &input) {
  EXPECT_EQ(actual, expected) << ("\"" + input + "\"");
}

void expect_eq(const IndexSet &actual, const IndexSet &expected,
               const std::string &input) {
  EXPECT_EQ(actual.upperBound, expected.upperBound) << ("\"" + input + "\"");
}

void expect_eq(const BasicParTypeArray &actual,
               const BasicParTypeArray &expected, const std::string &input) {
  expect_eq(actual.indexSet, expected.indexSet, input);
  expect_eq(actual.type, expected.type, input);
}

void expect_eq(const ParType &actual, const ParType &expected,
               const std::string &input) {
  expect_eq_type(actual, expected, input);
  if (actual.type() == typeid(BasicParType)) {
    return expect_eq(get<BasicParType>(actual), get<BasicParType>(expected),
                     input);
  } else if (actual.type() == typeid(BasicParTypeArray)) {
    return expect_eq(get<BasicParTypeArray>(actual),
                     get<BasicParTypeArray>(expected), input);
  } else {
    FAIL() << ("\"" + input + "\"");
  }
}

void expect_eq(const BoolVar &, const BoolVar &, const std::string &) {}

void expect_eq(const BasicVarBoolType &, const BasicVarBoolType &,
               const std::string &) {}

void expect_eq(const BasicVarIntTypeUnbounded &,
               const BasicVarIntTypeUnbounded &, const std::string &) {}

void expect_eq(const BasicVarIntTypeBounded &actual,
               const BasicVarIntTypeBounded &expected,
               const std::string &input) {
  expect_eq_bounded(actual, expected, input);
}

void expect_eq(const BasicVarIntTypeSet &actual,
               const BasicVarIntTypeSet &expected, const std::string &input) {
  expect_eq_vector(actual, expected, input);
}

void expect_eq(const BasicVarFloatTypeUnbounded &,
               const BasicVarFloatTypeUnbounded &, const std::string &) {}

void expect_eq(const BasicVarFloatTypeBounded &actual,
               const BasicVarFloatTypeBounded &expected,
               const std::string &input) {
  expect_eq_bounded(actual, expected, input);
}

void expect_eq(const BasicVarSetTypeBounded &actual,
               const BasicVarSetTypeBounded &expected,
               const std::string &input) {
  expect_eq_bounded(actual, expected, input);
}

void expect_eq(const BasicVarType &actual, const BasicVarType &expected,
               const std::string &input) {
  expect_eq_type(actual, expected, input);
  if (actual.type() == typeid(BasicVarBoolType)) {
    return expect_eq(get<BasicVarBoolType>(actual),
                     get<BasicVarBoolType>(expected), input);
  } else if (actual.type() == typeid(BasicVarIntTypeUnbounded)) {
    return expect_eq(get<BasicVarIntTypeUnbounded>(actual),
                     get<BasicVarIntTypeUnbounded>(expected), input);
  } else if (actual.type() == typeid(BasicVarIntTypeBounded)) {
    return expect_eq(get<BasicVarIntTypeBounded>(actual),
                     get<BasicVarIntTypeBounded>(expected), input);
  } else if (actual.type() == typeid(BasicVarIntTypeSet)) {
    return expect_eq(get<BasicVarIntTypeSet>(actual),
                     get<BasicVarIntTypeSet>(expected), input);
  } else if (actual.type() == typeid(BasicVarFloatTypeBounded)) {
    return expect_eq(get<BasicVarFloatTypeBounded>(actual),
                     get<BasicVarFloatTypeBounded>(expected), input);
  } else if (actual.type() == typeid(BasicVarFloatTypeUnbounded)) {
    return expect_eq(get<BasicVarFloatTypeUnbounded>(actual),
                     get<BasicVarFloatTypeUnbounded>(expected), input);
  } else if (actual.type() == typeid(BasicVarSetTypeBounded)) {
    return expect_eq(get<BasicVarSetTypeBounded>(actual),
                     get<BasicVarSetTypeBounded>(expected), input);
  } else if (actual.type() == typeid(BasicVarSetTypeSet)) {
    return expect_eq(get<BasicVarSetTypeSet>(actual),
                     get<BasicVarSetTypeSet>(expected), input);
  } else {
    FAIL() << ("\"" + input + "\"");
  }
}

void expect_eq(const ArrayVarType &actual, const ArrayVarType &expected,
               const std::string &input) {
  expect_eq(actual.indexSet, expected.indexSet, input);
  expect_eq(actual.type, expected.type, input);
}

void expect_eq(const IndexSetUnbounded &, const IndexSetUnbounded &,
               const std::string &) {}

void expect_eq(const PredIndexSet &actual, const PredIndexSet &expected,
               const std::string &input) {
  expect_eq_type(actual, expected, input);
  if (actual.type() == typeid(IndexSet)) {
    expect_eq(get<IndexSet>(actual), get<IndexSet>(expected), input);
  } else if (actual.type() == typeid(IndexSetUnbounded)) {
    expect_eq(get<IndexSetUnbounded>(actual), get<IndexSetUnbounded>(expected),
              input);
  } else {
    FAIL() << ("\"" + input + "\"");
  }
}

void expect_eq(const BasicPredParamTypeIntBounded &actual,
               const BasicPredParamTypeIntBounded &expected,
               const std::string &input) {
  expect_eq_bounded(actual, expected, input);
}

void expect_eq(const BasicPredParamTypeFloatBounded &actual,
               const BasicPredParamTypeFloatBounded &expected,
               const std::string &input) {
  expect_eq_bounded(actual, expected, input);
}

void expect_eq(const BasicPredParamTypeIntSet &actual,
               const BasicPredParamTypeIntSet &expected,
               const std::string &input) {
  expect_eq_vector(actual, expected, input);
}

void expect_eq(const BasicPredParamTypeSetBounded &actual,
               const BasicPredParamTypeSetBounded &expected,
               const std::string &input) {
  expect_eq_bounded(actual, expected, input);
}

void expect_eq(const BasicVarSetTypeSet &actual,
               const BasicVarSetTypeSet &expected, const std::string &input) {
  expect_eq_vector(actual, expected, input);
}

void expect_eq(const BasicPredParamTypeSetSet &actual,
               const BasicPredParamTypeSetSet &expected,
               const std::string &input) {
  expect_eq_vector(actual, expected, input);
}

void expect_eq(const BasicPredParamType &actual,
               const BasicPredParamType &expected, const std::string &input) {
  expect_eq_type(actual, expected, input);
  if (actual.type() == typeid(BasicParType)) {
    return expect_eq(get<BasicParType>(expected), get<BasicParType>(actual),
                     input);
  } else if (actual.type() == typeid(BasicVarBoolType)) {
    return expect_eq(get<BasicVarBoolType>(expected),
                     get<BasicVarBoolType>(actual), input);
  } else if (actual.type() == typeid(BasicVarIntTypeUnbounded)) {
    return expect_eq(get<BasicVarIntTypeUnbounded>(expected),
                     get<BasicVarIntTypeUnbounded>(actual), input);
  } else if (actual.type() == typeid(BasicVarIntTypeBounded)) {
    return expect_eq(get<BasicVarIntTypeBounded>(expected),
                     get<BasicVarIntTypeBounded>(actual), input);
  } else if (actual.type() == typeid(BasicVarIntTypeSet)) {
    return expect_eq(get<BasicVarIntTypeSet>(expected),
                     get<BasicVarIntTypeSet>(actual), input);
  } else if (actual.type() == typeid(BasicVarFloatTypeUnbounded)) {
    return expect_eq(get<BasicVarFloatTypeUnbounded>(expected),
                     get<BasicVarFloatTypeUnbounded>(actual), input);
  } else if (actual.type() == typeid(BasicVarFloatTypeBounded)) {
    return expect_eq(get<BasicVarFloatTypeBounded>(expected),
                     get<BasicVarFloatTypeBounded>(actual), input);
  } else if (actual.type() == typeid(BasicVarSetTypeBounded)) {
    return expect_eq(get<BasicVarSetTypeBounded>(expected),
                     get<BasicVarSetTypeBounded>(actual), input);
  } else if (actual.type() == typeid(BasicVarSetTypeSet)) {
    return expect_eq(get<BasicVarSetTypeSet>(expected),
                     get<BasicVarSetTypeSet>(actual), input);
  } else if (actual.type() == typeid(BasicPredParamTypeIntBounded)) {
    return expect_eq(get<BasicPredParamTypeIntBounded>(expected),
                     get<BasicPredParamTypeIntBounded>(actual), input);
  } else if (actual.type() == typeid(BasicPredParamTypeFloatBounded)) {
    return expect_eq(get<BasicPredParamTypeFloatBounded>(expected),
                     get<BasicPredParamTypeFloatBounded>(actual), input);
  } else if (actual.type() == typeid(BasicPredParamTypeIntSet)) {
    return expect_eq(get<BasicPredParamTypeIntSet>(expected),
                     get<BasicPredParamTypeIntSet>(actual), input);
  } else if (actual.type() == typeid(BasicPredParamTypeSetBounded)) {
    return expect_eq(get<BasicPredParamTypeSetBounded>(expected),
                     get<BasicPredParamTypeSetBounded>(actual), input);
  } else if (actual.type() == typeid(BasicPredParamTypeSetSet)) {
    return expect_eq(get<BasicPredParamTypeSetSet>(expected),
                     get<BasicPredParamTypeSetSet>(actual), input);
  } else {
    FAIL() << ("\"" + input + "\"");
  }
}

void expect_eq(const PredParamType &actual, const PredParamType &expected,
               const std::string &input) {
  expect_eq_type(actual, expected, input);
  if (actual.type() == typeid(BasicParType)) {
    expect_eq(get<BasicParType>(actual), get<BasicParType>(expected), input);
  } else if (actual.type() == typeid(BasicVarBoolType)) {
    expect_eq(get<BasicVarBoolType>(actual), get<BasicVarBoolType>(expected),
              input);
  } else if (actual.type() == typeid(BasicVarIntTypeUnbounded)) {
    expect_eq(get<BasicVarIntTypeUnbounded>(actual),
              get<BasicVarIntTypeUnbounded>(expected), input);
  } else if (actual.type() == typeid(BasicVarIntTypeBounded)) {
    expect_eq(get<BasicVarIntTypeBounded>(actual),
              get<BasicVarIntTypeBounded>(expected), input);
  } else if (actual.type() == typeid(BasicVarIntTypeSet)) {
    expect_eq(get<BasicVarIntTypeSet>(actual),
              get<BasicVarIntTypeSet>(expected), input);
  } else if (actual.type() == typeid(BasicVarFloatTypeBounded)) {
    expect_eq(get<BasicVarFloatTypeBounded>(actual),
              get<BasicVarFloatTypeBounded>(expected), input);
  } else if (actual.type() == typeid(BasicVarFloatTypeUnbounded)) {
    expect_eq(get<BasicVarFloatTypeUnbounded>(actual),
              get<BasicVarFloatTypeUnbounded>(expected), input);
  } else if (actual.type() == typeid(BasicVarSetTypeBounded)) {
    expect_eq(get<BasicVarSetTypeBounded>(actual),
              get<BasicVarSetTypeBounded>(expected), input);
  } else if (actual.type() == typeid(BasicVarSetTypeSet)) {
    expect_eq(get<BasicVarSetTypeSet>(actual),
              get<BasicVarSetTypeSet>(expected), input);
  } else if (actual.type() == typeid(BasicPredParamTypeIntBounded)) {
    expect_eq(get<BasicPredParamTypeIntBounded>(actual),
              get<BasicPredParamTypeIntBounded>(expected), input);
  } else if (actual.type() == typeid(BasicPredParamTypeFloatBounded)) {
    expect_eq(get<BasicPredParamTypeFloatBounded>(actual),
              get<BasicPredParamTypeFloatBounded>(expected), input);
  } else if (actual.type() == typeid(BasicPredParamTypeIntSet)) {
    expect_eq(get<BasicPredParamTypeIntSet>(actual),
              get<BasicPredParamTypeIntSet>(expected), input);
  } else if (actual.type() == typeid(BasicPredParamTypeSetBounded)) {
    expect_eq(get<BasicPredParamTypeSetBounded>(actual),
              get<BasicPredParamTypeSetBounded>(expected), input);
  } else if (actual.type() == typeid(BasicPredParamTypeSetSet)) {
    expect_eq(get<BasicPredParamTypeSetSet>(actual),
              get<BasicPredParamTypeSetSet>(expected), input);
  } else if (actual.type() == typeid(PredParamArrayType)) {
    expect_eq(get<PredParamArrayType>(actual),
              get<PredParamArrayType>(expected), input);
  }
}

void expect_eq(const PredParam &actual, const PredParam &expected,
               const std::string &input) {
  expect_eq(actual.type, expected.type, input);
  EXPECT_EQ(actual.identifier, expected.identifier) << ("\"" + input + "\"");
}

void expect_eq(const SetLiteralEmpty &, const SetLiteralEmpty &,
               const std::string &) {}

void expect_eq(const IntSetLiteralBounded &actual,
               const IntSetLiteralBounded &expected, const std::string &input) {
  expect_eq_bounded(actual, expected, input);
}

void expect_eq(const IntSetLiteralSet &actual, const IntSetLiteralSet &expected,
               const std::string &input) {
  expect_eq_vector(actual, expected, input);
}

void expect_eq(const FloatSetLiteralBounded &actual,
               const FloatSetLiteralBounded &expected,
               const std::string &input) {
  expect_eq_bounded(actual, expected, input);
}

void expect_eq(const FloatSetLiteralSet &actual,
               const FloatSetLiteralSet &expected, const std::string &input) {
  expect_eq_vector(actual, expected, input);
}

void expect_eq(const IntSetLiteralSet &actual,
               const FloatSetLiteralSet &expected, const std::string &input) {
  EXPECT_TRUE(actual.empty()) << ("\"" + input + "\"");
  EXPECT_TRUE(expected.empty()) << ("\"" + input + "\"");
}

void expect_eq(const FloatSetLiteralSet &actual,
               const IntSetLiteralSet &expected, const std::string &input) {
  EXPECT_TRUE(actual.empty()) << ("\"" + input + "\"");
  EXPECT_TRUE(expected.empty()) << ("\"" + input + "\"");
}

void expect_eq(const BasicLiteralExpr &actual, const BasicLiteralExpr &expected,
               const std::string &input) {
  expect_eq_type(actual, expected, input);
  if (actual.type() == typeid(bool)) {
    EXPECT_EQ(get<bool>(actual), get<bool>(expected)) << ("\"" + input + "\"");
  } else if (actual.type() == typeid(int64_t)) {
    EXPECT_EQ(get<int64_t>(actual), get<int64_t>(expected))
        << ("\"" + input + "\"");
  } else if (actual.type() == typeid(double)) {
    EXPECT_EQ(get<double>(actual), get<double>(expected))
        << ("\"" + input + "\"");
  } else if (actual.type() == typeid(SetLiteralEmpty)) {
    expect_eq(get<SetLiteralEmpty>(actual), get<SetLiteralEmpty>(expected),
              input);
  } else if (actual.type() == typeid(IntSetLiteralBounded)) {
    expect_eq(get<IntSetLiteralBounded>(actual),
              get<IntSetLiteralBounded>(expected), input);
  } else if (actual.type() == typeid(IntSetLiteralSet)) {
    expect_eq(get<IntSetLiteralSet>(actual), get<IntSetLiteralSet>(expected),
              input);
  } else if (actual.type() == typeid(FloatSetLiteralBounded)) {
    expect_eq(get<FloatSetLiteralBounded>(actual),
              get<FloatSetLiteralBounded>(expected), input);
  } else if (actual.type() == typeid(FloatSetLiteralSet)) {
    expect_eq(get<FloatSetLiteralSet>(actual),
              get<FloatSetLiteralSet>(expected), input);
  } else {
    FAIL() << ("\"" + input + "\"");
  }
}

void expect_eq(const BasicExpr &actual, const BasicExpr &expected,
               const std::string &input) {
  expect_eq_type(actual, expected, input);
  if (actual.type() == typeid(bool)) {
    EXPECT_EQ(get<bool>(actual), get<bool>(expected)) << ("\"" + input + "\"");
  } else if (actual.type() == typeid(int64_t)) {
    EXPECT_EQ(get<int64_t>(actual), get<int64_t>(expected))
        << ("\"" + input + "\"");
  } else if (actual.type() == typeid(double)) {
    EXPECT_EQ(get<double>(actual), get<double>(expected))
        << ("\"" + input + "\"");
  } else if (actual.type() == typeid(SetLiteralEmpty)) {
    expect_eq(get<SetLiteralEmpty>(actual), get<SetLiteralEmpty>(expected),
              input);
  } else if (actual.type() == typeid(IntSetLiteralBounded)) {
    expect_eq(get<IntSetLiteralBounded>(actual),
              get<IntSetLiteralBounded>(expected), input);
  } else if (actual.type() == typeid(FloatSetLiteralBounded)) {
    expect_eq(get<FloatSetLiteralBounded>(actual),
              get<FloatSetLiteralBounded>(expected), input);
  } else if (actual.type() == typeid(IntSetLiteralSet)) {
    expect_eq(get<IntSetLiteralSet>(actual), get<IntSetLiteralSet>(expected),
              input);
  } else if (actual.type() == typeid(FloatSetLiteralSet)) {
    expect_eq(get<FloatSetLiteralSet>(actual),
              get<FloatSetLiteralSet>(expected), input);
  } else if (actual.type() == typeid(std::string)) {
    EXPECT_EQ(get<std::string>(actual), get<std::string>(expected))
        << ("\"" + input + "\"");
  } else {
    FAIL() << ("\"" + input + "\"");
  }
}

void expect_eq(const ArrayLiteral &actual, const ArrayLiteral &expected,
               const std::string &input) {
  expect_eq_vector(actual, expected, input);
}

void expect_eq(const Expr &actual, const Expr &expected,
               const std::string &input) {
  expect_eq_type(actual, expected, input);
  if (actual.type() == typeid(bool)) {
    EXPECT_EQ(get<bool>(actual), get<bool>(expected)) << ("\"" + input + "\"");
  } else if (actual.type() == typeid(int64_t)) {
    EXPECT_EQ(get<int64_t>(actual), get<int64_t>(expected))
        << ("\"" + input + "\"");
  } else if (actual.type() == typeid(double)) {
    EXPECT_EQ(get<double>(actual), get<double>(expected))
        << ("\"" + input + "\"");
  } else if (actual.type() == typeid(SetLiteralEmpty)) {
    expect_eq(get<SetLiteralEmpty>(actual), get<SetLiteralEmpty>(expected),
              input);
  } else if (actual.type() == typeid(IntSetLiteralBounded)) {
    expect_eq(get<IntSetLiteralBounded>(actual),
              get<IntSetLiteralBounded>(expected), input);
  } else if (actual.type() == typeid(IntSetLiteralSet)) {
    expect_eq(get<IntSetLiteralSet>(actual), get<IntSetLiteralSet>(expected),
              input);
  } else if (actual.type() == typeid(FloatSetLiteralBounded)) {
    expect_eq(get<FloatSetLiteralBounded>(actual),
              get<FloatSetLiteralBounded>(expected), input);
  } else if (actual.type() == typeid(FloatSetLiteralSet)) {
    expect_eq(get<FloatSetLiteralSet>(actual),
              get<FloatSetLiteralSet>(expected), input);
  } else if (actual.type() == typeid(std::string)) {
    EXPECT_EQ(get<std::string>(actual), get<std::string>(expected))
        << ("\"" + input + "\"");
  } else if (actual.type() == typeid(ArrayLiteral)) {
    expect_eq(get<ArrayLiteral>(actual), get<ArrayLiteral>(expected), input);
  } else {
    FAIL() << ("\"" + input + "\"");
  }
}

void expect_eq(const ParArrayLiteral &actual, const ParArrayLiteral &expected,
               const std::string &input) {
  expect_eq_vector(actual, expected, input);
}

void expect_eq(const ParExpr &actual, const ParExpr &expected,
               const std::string &input) {
  expect_eq_type(actual, expected, input);
  if (actual.type() == typeid(bool)) {
    EXPECT_EQ(get<bool>(actual), get<bool>(expected)) << ("\"" + input + "\"");
  } else if (actual.type() == typeid(int64_t)) {
    EXPECT_EQ(get<int64_t>(actual), get<int64_t>(expected))
        << ("\"" + input + "\"");
  } else if (actual.type() == typeid(double)) {
    EXPECT_EQ(get<double>(actual), get<double>(expected))
        << ("\"" + input + "\"");
  } else if (actual.type() == typeid(SetLiteralEmpty)) {
    expect_eq(get<SetLiteralEmpty>(actual), get<SetLiteralEmpty>(expected),
              input);
  } else if (actual.type() == typeid(IntSetLiteralBounded)) {
    expect_eq(get<IntSetLiteralBounded>(actual),
              get<IntSetLiteralBounded>(expected), input);
  } else if (actual.type() == typeid(IntSetLiteralSet)) {
    expect_eq(get<IntSetLiteralSet>(actual), get<IntSetLiteralSet>(expected),
              input);
  } else if (actual.type() == typeid(FloatSetLiteralBounded)) {
    expect_eq(get<FloatSetLiteralBounded>(actual),
              get<FloatSetLiteralBounded>(expected), input);
  } else if (actual.type() == typeid(FloatSetLiteralSet)) {
    expect_eq(get<FloatSetLiteralSet>(actual),
              get<FloatSetLiteralSet>(expected), input);
  } else if (actual.type() == typeid(ParArrayLiteral)) {
    expect_eq(get<ParArrayLiteral>(actual), get<ParArrayLiteral>(expected),
              input);
  } else {
    FAIL() << ("\"" + input + "\"");
  }
}

void expect_eq(const ParDeclItem &actual, const ParDeclItem &expected,
               const std::string &input) {
  expect_eq(actual.type, expected.type, input);
  EXPECT_EQ(actual.identifier, expected.identifier) << ("\"" + input + "\"");
  expect_eq(actual.expr, expected.expr, input);
}

void expect_eq(const PredParamArrayType &actual,
               const PredParamArrayType &expected, const std::string &input) {
  expect_eq(actual.predIndexSet, expected.predIndexSet, input);
  expect_eq(actual.basicPredParamType, expected.basicPredParamType, input);
}

void expect_eq(const Annotation &actual, const Annotation &expected,
               const std::string &input) {
  EXPECT_EQ(actual.identifier, expected.identifier) << ("\"" + input + "\"");
  expect_eq_vector(actual.expressions, expected.expressions, input);
}

void expect_eq(const BasicAnnExpr &actual, const BasicAnnExpr &expected,
               const std::string &input) {
  expect_eq_type(actual, expected, input);
  if (actual.type() == typeid(bool)) {
    EXPECT_EQ(get<bool>(actual), get<bool>(expected)) << ("\"" + input + "\"");
  } else if (actual.type() == typeid(int64_t)) {
    EXPECT_EQ(get<int64_t>(actual), get<int64_t>(expected))
        << ("\"" + input + "\"");
  } else if (actual.type() == typeid(double)) {
    EXPECT_EQ(get<double>(actual), get<double>(expected))
        << ("\"" + input + "\"");
  } else if (actual.type() == typeid(SetLiteralEmpty)) {
    expect_eq(get<SetLiteralEmpty>(actual), get<SetLiteralEmpty>(expected),
              input);
  } else if (actual.type() == typeid(IntSetLiteralBounded)) {
    expect_eq(get<IntSetLiteralBounded>(actual),
              get<IntSetLiteralBounded>(expected), input);
  } else if (actual.type() == typeid(IntSetLiteralSet)) {
    expect_eq(get<IntSetLiteralSet>(actual), get<IntSetLiteralSet>(expected),
              input);
  } else if (actual.type() == typeid(FloatSetLiteralBounded)) {
    expect_eq(get<FloatSetLiteralBounded>(actual),
              get<FloatSetLiteralBounded>(expected), input);
  } else if (actual.type() == typeid(FloatSetLiteralSet)) {
    expect_eq(get<FloatSetLiteralSet>(actual),
              get<FloatSetLiteralSet>(expected), input);
  } else if (actual.type() == typeid(std::string)) {
    EXPECT_EQ(get<std::string>(actual), get<std::string>(expected))
        << ("\"" + input + "\"");
  } else if (actual.type() == typeid(x3::forward_ast<Annotation>)) {
    expect_eq(get<x3::forward_ast<Annotation>>(actual).get(),
              get<x3::forward_ast<Annotation>>(expected).get(), input);
  } else {
    FAIL() << ("\"" + input + "\"");
  }
}

void expect_eq(const AnnExpr &actual, const AnnExpr &expected,
               const std::string &input) {
  expect_eq_vector(actual, expected, input);
}

void expect_eq(const Annotations &actual, const Annotations &expected,
               const std::string &input) {
  expect_eq_vector(actual, expected, input);
}

void expect_eq(const BasicVarDecl &actual, const BasicVarDecl &expected,
               const std::string &input) {
  expect_eq(actual.type, expected.type, input);
  expect_eq(actual.annotations, expected.annotations, input);
  EXPECT_EQ(actual.expr.has_value(), expected.expr.has_value())
      << ("\"" + input + "\"");
  if (actual.expr.has_value()) {
    expect_eq(actual.expr.value(), expected.expr.value(), input);
  }
}

void expect_eq(const ArrayVarDecl &actual, const ArrayVarDecl &expected,
               const std::string &input) {
  expect_eq(actual.type, expected.type, input);
  expect_eq(actual.annotations, expected.annotations, input);
  expect_eq(actual.literals, expected.literals, input);
}

void expect_eq(const VarDeclItem &actual, const VarDeclItem &expected,
               const std::string &input) {
  expect_eq_type(actual, expected, input);
  if (actual.type() == typeid(BasicVarDecl)) {
    expect_eq(get<BasicVarDecl>(actual), get<BasicVarDecl>(expected), input);
  } else if (actual.type() == typeid(ArrayVarDecl)) {
    expect_eq(get<ArrayVarDecl>(actual), get<ArrayVarDecl>(expected), input);
  } else {
    FAIL() << ("\"" + input + "\"");
  }
}

void expect_eq(const ConstraintItem &actual, const ConstraintItem &expected,
               const std::string &input) {
  expect_eq(actual.identifier, expected.identifier, input);
  expect_eq_vector(actual.expressions, expected.expressions, input);
  expect_eq(actual.annotations, expected.annotations, input);
}

void expect_eq(const SolveSatisfy &actual, const SolveSatisfy &expected,
               const std::string &input) {
  expect_eq(actual.annotations, expected.annotations, input);
}

void expect_eq(const SolveOptimize &actual, const SolveOptimize &expected,
               const std::string &input) {
  expect_eq(actual.annotations, expected.annotations, input);
  EXPECT_EQ(actual.type, expected.type) << ("\"" + input + "\"");
  expect_eq(actual.expr, expected.expr, input);
}

void expect_eq(const SolveItem &actual, const SolveItem &expected,
               const std::string &input) {
  expect_eq_type(actual, expected, input);
  if (actual.type() == typeid(SolveSatisfy)) {
    expect_eq(get<SolveSatisfy>(actual), get<SolveSatisfy>(expected), input);
  } else if (actual.type() == typeid(SolveOptimize)) {
    expect_eq(get<SolveOptimize>(actual), get<SolveOptimize>(expected), input);
  } else {
    FAIL() << ("\"" + input + "\"");
  }
}

void expect_eq(const PredicateItem &actual, const PredicateItem &expected,
               const std::string &input) {
  EXPECT_EQ(actual.identifier, expected.identifier) << ("\"" + input + "\"");
  expect_eq_vector(actual.params, expected.params, input);
}

void expect_eq(const Model &actual, const Model &expected,
               const std::string &input) {
  expect_eq_vector(actual.predicateItems, expected.predicateItems, input);
  expect_eq_vector(actual.parDeclItems, expected.parDeclItems, input);
  expect_eq_vector(actual.varDeclItems, expected.varDeclItems, input);
  expect_eq_vector(actual.constraintItems, expected.constraintItems, input);
  expect_eq(actual.solveItem, expected.solveItem, input);
}
}  // namespace fznparser::testing