#include "expectEq.hpp"

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
  EXPECT_EQ(actual.size(), expected.size()) << input;
  for (size_t i = 0; i < actual.size(); ++i) {
    expect_eq(actual.at(i), expected.at(i), input);
  }
}

void expect_eq(const int64_t &actual, const int64_t &expected,
               const std::string &input) {
  const std::string newInput = input + "\nint64_t:\nactual: \n" +
                               std::to_string(actual) + "\nexpected:\n" +
                               std::to_string(expected);
  EXPECT_EQ(actual, expected) << newInput;
}

void expect_eq(const double &actual, const double &expected,
               const std::string &input) {
  const std::string newInput = input + "\ndouble:\nactual: \n" +
                               std::to_string(actual) + "\nexpected:\n" +
                               std::to_string(expected);
  EXPECT_EQ(actual, expected) << newInput;
}

void expect_eq(const std::string &actual, const std::string &expected,
               const std::string &input) {
  const std::string newInput = input + "\nstd::string:\nactual: \n" + actual +
                               "\nexpected:\n" + expected;
  EXPECT_EQ(actual, expected) << newInput;
}

void expect_eq(const BasicParType &actual, const BasicParType &expected,
               const std::string &input) {
  const std::string newInput = input + "\nBasicParType:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  EXPECT_EQ(actual, expected) << newInput;
}

void expect_eq(const IndexSet &actual, const IndexSet &expected,
               const std::string &input) {
  const std::string newInput = input + "\nIndexSet:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  EXPECT_EQ(actual.upperBound, expected.upperBound) << newInput;
}

void expect_eq(const BasicParTypeArray &actual,
               const BasicParTypeArray &expected, const std::string &input) {
  const std::string newInput = input + "\nBasicParTypeArray:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq(actual.indexSet, expected.indexSet, newInput);
  expect_eq(actual.type, expected.type, newInput);
}

void expect_eq(const ParType &actual, const ParType &expected,
               const std::string &input) {
  const std::string newInput = input + "\nParType:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_type(actual, expected, newInput);
  if (actual.type() == typeid(BasicParType)) {
    return expect_eq(get<BasicParType>(actual), get<BasicParType>(expected),
                     input);
  } else if (actual.type() == typeid(BasicParTypeArray)) {
    return expect_eq(get<BasicParTypeArray>(actual),
                     get<BasicParTypeArray>(expected), newInput);
  } else {
    FAIL() << newInput;
  }
}

void expect_eq(const BasicVarBoolType &, const BasicVarBoolType &,
               const std::string &) {}

void expect_eq(const BasicVarIntTypeUnbounded &,
               const BasicVarIntTypeUnbounded &, const std::string &) {}

void expect_eq(const BasicVarIntTypeBounded &actual,
               const BasicVarIntTypeBounded &expected,
               const std::string &input) {
  const std::string newInput = input + "\nBasicVarIntTypeBounded:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_bounded(actual, expected, newInput);
}

void expect_eq(const BasicVarIntTypeSet &actual,
               const BasicVarIntTypeSet &expected, const std::string &input) {
  const std::string newInput = input + "\nBasicVarIntTypeSet:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_vector(actual, expected, newInput);
}

void expect_eq(const BasicVarFloatTypeUnbounded &,
               const BasicVarFloatTypeUnbounded &, const std::string &) {}

void expect_eq(const BasicVarFloatTypeBounded &actual,
               const BasicVarFloatTypeBounded &expected,
               const std::string &input) {
  const std::string newInput =
      input + "\nBasicVarFloatTypeBounded:\nactual: \n" + toString(actual) +
      "\nexpected:\n" + toString(expected);
  expect_eq_bounded(actual, expected, newInput);
}

void expect_eq(const BasicVarSetTypeBounded &actual,
               const BasicVarSetTypeBounded &expected,
               const std::string &input) {
  const std::string newInput = input + "\nBasicVarSetTypeBounded:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_bounded(actual, expected, newInput);
}

void expect_eq(const BasicVarType &actual, const BasicVarType &expected,
               const std::string &input) {
  const std::string newInput = input + "\nBasicVarType:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_type(actual, expected, newInput);
  if (actual.type() == typeid(BasicVarBoolType)) {
    return expect_eq(get<BasicVarBoolType>(actual),
                     get<BasicVarBoolType>(expected), newInput);
  } else if (actual.type() == typeid(BasicVarIntTypeUnbounded)) {
    return expect_eq(get<BasicVarIntTypeUnbounded>(actual),
                     get<BasicVarIntTypeUnbounded>(expected), newInput);
  } else if (actual.type() == typeid(BasicVarIntTypeBounded)) {
    return expect_eq(get<BasicVarIntTypeBounded>(actual),
                     get<BasicVarIntTypeBounded>(expected), newInput);
  } else if (actual.type() == typeid(BasicVarIntTypeSet)) {
    return expect_eq(get<BasicVarIntTypeSet>(actual),
                     get<BasicVarIntTypeSet>(expected), newInput);
  } else if (actual.type() == typeid(BasicVarFloatTypeBounded)) {
    return expect_eq(get<BasicVarFloatTypeBounded>(actual),
                     get<BasicVarFloatTypeBounded>(expected), newInput);
  } else if (actual.type() == typeid(BasicVarFloatTypeUnbounded)) {
    return expect_eq(get<BasicVarFloatTypeUnbounded>(actual),
                     get<BasicVarFloatTypeUnbounded>(expected), newInput);
  } else if (actual.type() == typeid(BasicVarSetTypeBounded)) {
    return expect_eq(get<BasicVarSetTypeBounded>(actual),
                     get<BasicVarSetTypeBounded>(expected), newInput);
  } else if (actual.type() == typeid(BasicVarSetTypeSet)) {
    return expect_eq(get<BasicVarSetTypeSet>(actual),
                     get<BasicVarSetTypeSet>(expected), newInput);
  } else {
    FAIL() << newInput;
  }
}

void expect_eq(const ArrayVarType &actual, const ArrayVarType &expected,
               const std::string &input) {
  const std::string newInput = input + "\nArrayVarType:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq(actual.indexSet, expected.indexSet, newInput);
  expect_eq(actual.type, expected.type, newInput);
}

void expect_eq(const IndexSetUnbounded &, const IndexSetUnbounded &,
               const std::string &) {}

void expect_eq(const PredIndexSet &actual, const PredIndexSet &expected,
               const std::string &input) {
  const std::string newInput = input + "\nPredIndexSet:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_type(actual, expected, newInput);
  if (actual.type() == typeid(IndexSet)) {
    expect_eq(get<IndexSet>(actual), get<IndexSet>(expected), newInput);
  } else if (actual.type() == typeid(IndexSetUnbounded)) {
    expect_eq(get<IndexSetUnbounded>(actual), get<IndexSetUnbounded>(expected),
              input);
  } else {
    FAIL() << newInput;
  }
}

void expect_eq(const BasicPredParamTypeIntBounded &actual,
               const BasicPredParamTypeIntBounded &expected,
               const std::string &input) {
  const std::string newInput =
      input + "\nBasicPredParamTypeIntBounded:\nactual: \n" + toString(actual) +
      "\nexpected:\n" + toString(expected);
  expect_eq_bounded(actual, expected, newInput);
}

void expect_eq(const BasicPredParamTypeFloatBounded &actual,
               const BasicPredParamTypeFloatBounded &expected,
               const std::string &input) {
  const std::string newInput =
      input + "\nBasicPredParamTypeFloatBounded:\nactual: \n" +
      toString(actual) + "\nexpected:\n" + toString(expected);
  expect_eq_bounded(actual, expected, newInput);
}

void expect_eq(const BasicPredParamTypeIntSet &actual,
               const BasicPredParamTypeIntSet &expected,
               const std::string &input) {
  const std::string newInput =
      input + "\nBasicPredParamTypeIntSet:\nactual: \n" + toString(actual) +
      "\nexpected:\n" + toString(expected);
  expect_eq_vector(actual, expected, newInput);
}

void expect_eq(const BasicPredParamTypeSetBounded &actual,
               const BasicPredParamTypeSetBounded &expected,
               const std::string &input) {
  const std::string newInput =
      input + "\nBasicPredParamTypeSetBounded:\nactual: \n" + toString(actual) +
      "\nexpected:\n" + toString(expected);
  expect_eq_bounded(actual, expected, newInput);
}

void expect_eq(const BasicVarSetTypeSet &actual,
               const BasicVarSetTypeSet &expected, const std::string &input) {
  const std::string newInput = input + "\nBasicVarSetTypeSet:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_vector(actual, expected, newInput);
}

void expect_eq(const BasicPredParamTypeSetSet &actual,
               const BasicPredParamTypeSetSet &expected,
               const std::string &input) {
  const std::string newInput =
      input + "\nBasicPredParamTypeSetSet:\nactual: \n" + toString(actual) +
      "\nexpected:\n" + toString(expected);
  expect_eq_vector(actual, expected, newInput);
}

void expect_eq(const BasicPredParamType &actual,
               const BasicPredParamType &expected, const std::string &input) {
  const std::string newInput = input + "\nBasicPredParamType:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_type(actual, expected, newInput);
  if (actual.type() == typeid(BasicParType)) {
    return expect_eq(get<BasicParType>(expected), get<BasicParType>(actual),
                     input);
  } else if (actual.type() == typeid(BasicVarBoolType)) {
    return expect_eq(get<BasicVarBoolType>(expected),
                     get<BasicVarBoolType>(actual), newInput);
  } else if (actual.type() == typeid(BasicVarIntTypeUnbounded)) {
    return expect_eq(get<BasicVarIntTypeUnbounded>(expected),
                     get<BasicVarIntTypeUnbounded>(actual), newInput);
  } else if (actual.type() == typeid(BasicVarIntTypeBounded)) {
    return expect_eq(get<BasicVarIntTypeBounded>(expected),
                     get<BasicVarIntTypeBounded>(actual), newInput);
  } else if (actual.type() == typeid(BasicVarIntTypeSet)) {
    return expect_eq(get<BasicVarIntTypeSet>(expected),
                     get<BasicVarIntTypeSet>(actual), newInput);
  } else if (actual.type() == typeid(BasicVarFloatTypeUnbounded)) {
    return expect_eq(get<BasicVarFloatTypeUnbounded>(expected),
                     get<BasicVarFloatTypeUnbounded>(actual), newInput);
  } else if (actual.type() == typeid(BasicVarFloatTypeBounded)) {
    return expect_eq(get<BasicVarFloatTypeBounded>(expected),
                     get<BasicVarFloatTypeBounded>(actual), newInput);
  } else if (actual.type() == typeid(BasicVarSetTypeBounded)) {
    return expect_eq(get<BasicVarSetTypeBounded>(expected),
                     get<BasicVarSetTypeBounded>(actual), newInput);
  } else if (actual.type() == typeid(BasicVarSetTypeSet)) {
    return expect_eq(get<BasicVarSetTypeSet>(expected),
                     get<BasicVarSetTypeSet>(actual), newInput);
  } else if (actual.type() == typeid(BasicPredParamTypeIntBounded)) {
    return expect_eq(get<BasicPredParamTypeIntBounded>(expected),
                     get<BasicPredParamTypeIntBounded>(actual), newInput);
  } else if (actual.type() == typeid(BasicPredParamTypeFloatBounded)) {
    return expect_eq(get<BasicPredParamTypeFloatBounded>(expected),
                     get<BasicPredParamTypeFloatBounded>(actual), newInput);
  } else if (actual.type() == typeid(BasicPredParamTypeIntSet)) {
    return expect_eq(get<BasicPredParamTypeIntSet>(expected),
                     get<BasicPredParamTypeIntSet>(actual), newInput);
  } else if (actual.type() == typeid(BasicPredParamTypeSetBounded)) {
    return expect_eq(get<BasicPredParamTypeSetBounded>(expected),
                     get<BasicPredParamTypeSetBounded>(actual), newInput);
  } else if (actual.type() == typeid(BasicPredParamTypeSetSet)) {
    return expect_eq(get<BasicPredParamTypeSetSet>(expected),
                     get<BasicPredParamTypeSetSet>(actual), newInput);
  } else {
    FAIL() << newInput;
  }
}

void expect_eq(const PredParamType &actual, const PredParamType &expected,
               const std::string &input) {
  const std::string newInput = input + "\nPredParamType:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_type(actual, expected, newInput);
  if (actual.type() == typeid(BasicParType)) {
    expect_eq(get<BasicParType>(actual), get<BasicParType>(expected), newInput);
  } else if (actual.type() == typeid(BasicVarBoolType)) {
    expect_eq(get<BasicVarBoolType>(actual), get<BasicVarBoolType>(expected),
              input);
  } else if (actual.type() == typeid(BasicVarIntTypeUnbounded)) {
    expect_eq(get<BasicVarIntTypeUnbounded>(actual),
              get<BasicVarIntTypeUnbounded>(expected), newInput);
  } else if (actual.type() == typeid(BasicVarIntTypeBounded)) {
    expect_eq(get<BasicVarIntTypeBounded>(actual),
              get<BasicVarIntTypeBounded>(expected), newInput);
  } else if (actual.type() == typeid(BasicVarIntTypeSet)) {
    expect_eq(get<BasicVarIntTypeSet>(actual),
              get<BasicVarIntTypeSet>(expected), newInput);
  } else if (actual.type() == typeid(BasicVarFloatTypeBounded)) {
    expect_eq(get<BasicVarFloatTypeBounded>(actual),
              get<BasicVarFloatTypeBounded>(expected), newInput);
  } else if (actual.type() == typeid(BasicVarFloatTypeUnbounded)) {
    expect_eq(get<BasicVarFloatTypeUnbounded>(actual),
              get<BasicVarFloatTypeUnbounded>(expected), newInput);
  } else if (actual.type() == typeid(BasicVarSetTypeBounded)) {
    expect_eq(get<BasicVarSetTypeBounded>(actual),
              get<BasicVarSetTypeBounded>(expected), newInput);
  } else if (actual.type() == typeid(BasicVarSetTypeSet)) {
    expect_eq(get<BasicVarSetTypeSet>(actual),
              get<BasicVarSetTypeSet>(expected), newInput);
  } else if (actual.type() == typeid(BasicPredParamTypeIntBounded)) {
    expect_eq(get<BasicPredParamTypeIntBounded>(actual),
              get<BasicPredParamTypeIntBounded>(expected), newInput);
  } else if (actual.type() == typeid(BasicPredParamTypeFloatBounded)) {
    expect_eq(get<BasicPredParamTypeFloatBounded>(actual),
              get<BasicPredParamTypeFloatBounded>(expected), newInput);
  } else if (actual.type() == typeid(BasicPredParamTypeIntSet)) {
    expect_eq(get<BasicPredParamTypeIntSet>(actual),
              get<BasicPredParamTypeIntSet>(expected), newInput);
  } else if (actual.type() == typeid(BasicPredParamTypeSetBounded)) {
    expect_eq(get<BasicPredParamTypeSetBounded>(actual),
              get<BasicPredParamTypeSetBounded>(expected), newInput);
  } else if (actual.type() == typeid(BasicPredParamTypeSetSet)) {
    expect_eq(get<BasicPredParamTypeSetSet>(actual),
              get<BasicPredParamTypeSetSet>(expected), newInput);
  } else if (actual.type() == typeid(PredParamArrayType)) {
    expect_eq(get<PredParamArrayType>(actual),
              get<PredParamArrayType>(expected), newInput);
  }
}

void expect_eq(const PredParam &actual, const PredParam &expected,
               const std::string &input) {
  const std::string newInput = input + "\nPredParam:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq(actual.type, expected.type, newInput);
  EXPECT_EQ(actual.identifier, expected.identifier) << newInput;
}

void expect_eq(const SetLiteralEmpty &, const SetLiteralEmpty &,
               const std::string &) {}

void expect_eq(const IntSetLiteralBounded &actual,
               const IntSetLiteralBounded &expected, const std::string &input) {
  const std::string newInput = input + "\nIntSetLiteralBounded:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_bounded(actual, expected, newInput);
}

void expect_eq(const IntSetLiteralSet &actual, const IntSetLiteralSet &expected,
               const std::string &input) {
  const std::string newInput = input + "\nIntSetLiteralSet:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_vector(actual, expected, newInput);
}

void expect_eq(const FloatSetLiteralBounded &actual,
               const FloatSetLiteralBounded &expected,
               const std::string &input) {
  const std::string newInput = input + "\nFloatSetLiteralBounded:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_bounded(actual, expected, newInput);
}

void expect_eq(const FloatSetLiteralSet &actual,
               const FloatSetLiteralSet &expected, const std::string &input) {
  const std::string newInput = input + "\nFloatSetLiteralSet:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_vector(actual, expected, newInput);
}

void expect_eq(const IntSetLiteralSet &actual,
               const FloatSetLiteralSet &expected, const std::string &input) {
  const std::string newInput = input + "\nIntSetLiteralSet:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  EXPECT_TRUE(actual.empty()) << newInput;
  EXPECT_TRUE(expected.empty()) << newInput;
}

void expect_eq(const FloatSetLiteralSet &actual,
               const IntSetLiteralSet &expected, const std::string &input) {
  const std::string newInput = input + "\nFloatSetLiteralSet:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  EXPECT_TRUE(actual.empty()) << newInput;
  EXPECT_TRUE(expected.empty()) << newInput;
}

void expect_eq(const BasicLiteralExpr &actual, const BasicLiteralExpr &expected,
               const std::string &input) {
  const std::string newInput = input + "\nBasicLiteralExpr:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_type(actual, expected, newInput);
  if (actual.type() == typeid(bool)) {
    EXPECT_EQ(get<bool>(actual), get<bool>(expected)) << newInput;
  } else if (actual.type() == typeid(int64_t)) {
    EXPECT_EQ(get<int64_t>(actual), get<int64_t>(expected)) << newInput;
  } else if (actual.type() == typeid(double)) {
    EXPECT_EQ(get<double>(actual), get<double>(expected)) << newInput;
  } else if (actual.type() == typeid(SetLiteralEmpty)) {
    expect_eq(get<SetLiteralEmpty>(actual), get<SetLiteralEmpty>(expected),
              input);
  } else if (actual.type() == typeid(IntSetLiteralBounded)) {
    expect_eq(get<IntSetLiteralBounded>(actual),
              get<IntSetLiteralBounded>(expected), newInput);
  } else if (actual.type() == typeid(IntSetLiteralSet)) {
    expect_eq(get<IntSetLiteralSet>(actual), get<IntSetLiteralSet>(expected),
              input);
  } else if (actual.type() == typeid(FloatSetLiteralBounded)) {
    expect_eq(get<FloatSetLiteralBounded>(actual),
              get<FloatSetLiteralBounded>(expected), newInput);
  } else if (actual.type() == typeid(FloatSetLiteralSet)) {
    expect_eq(get<FloatSetLiteralSet>(actual),
              get<FloatSetLiteralSet>(expected), newInput);
  } else {
    FAIL() << newInput;
  }
}

void expect_eq(const BasicExpr &actual, const BasicExpr &expected,
               const std::string &input) {
  const std::string newInput = input + "\nBasicExpr:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_type(actual, expected, newInput);
  if (actual.type() == typeid(bool)) {
    EXPECT_EQ(get<bool>(actual), get<bool>(expected)) << newInput;
  } else if (actual.type() == typeid(int64_t)) {
    EXPECT_EQ(get<int64_t>(actual), get<int64_t>(expected)) << newInput;
  } else if (actual.type() == typeid(double)) {
    EXPECT_EQ(get<double>(actual), get<double>(expected)) << newInput;
  } else if (actual.type() == typeid(SetLiteralEmpty)) {
    expect_eq(get<SetLiteralEmpty>(actual), get<SetLiteralEmpty>(expected),
              input);
  } else if (actual.type() == typeid(IntSetLiteralBounded)) {
    expect_eq(get<IntSetLiteralBounded>(actual),
              get<IntSetLiteralBounded>(expected), newInput);
  } else if (actual.type() == typeid(FloatSetLiteralBounded)) {
    expect_eq(get<FloatSetLiteralBounded>(actual),
              get<FloatSetLiteralBounded>(expected), newInput);
  } else if (actual.type() == typeid(IntSetLiteralSet)) {
    expect_eq(get<IntSetLiteralSet>(actual), get<IntSetLiteralSet>(expected),
              input);
  } else if (actual.type() == typeid(FloatSetLiteralSet)) {
    expect_eq(get<FloatSetLiteralSet>(actual),
              get<FloatSetLiteralSet>(expected), newInput);
  } else if (actual.type() == typeid(std::string)) {
    EXPECT_EQ(get<std::string>(actual), get<std::string>(expected)) << newInput;
  } else {
    FAIL() << newInput;
  }
}

void expect_eq(const ArrayLiteral &actual, const ArrayLiteral &expected,
               const std::string &input) {
  const std::string newInput = input + "\nArrayLiteral:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_vector(actual, expected, newInput);
}

void expect_eq(const Expr &actual, const Expr &expected,
               const std::string &input) {
  const std::string newInput = input + "\nExpr:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_type(actual, expected, newInput);
  if (actual.type() == typeid(bool)) {
    EXPECT_EQ(get<bool>(actual), get<bool>(expected)) << newInput;
  } else if (actual.type() == typeid(int64_t)) {
    EXPECT_EQ(get<int64_t>(actual), get<int64_t>(expected)) << newInput;
  } else if (actual.type() == typeid(double)) {
    EXPECT_EQ(get<double>(actual), get<double>(expected)) << newInput;
  } else if (actual.type() == typeid(SetLiteralEmpty)) {
    expect_eq(get<SetLiteralEmpty>(actual), get<SetLiteralEmpty>(expected),
              input);
  } else if (actual.type() == typeid(IntSetLiteralBounded)) {
    expect_eq(get<IntSetLiteralBounded>(actual),
              get<IntSetLiteralBounded>(expected), newInput);
  } else if (actual.type() == typeid(IntSetLiteralSet)) {
    expect_eq(get<IntSetLiteralSet>(actual), get<IntSetLiteralSet>(expected),
              input);
  } else if (actual.type() == typeid(FloatSetLiteralBounded)) {
    expect_eq(get<FloatSetLiteralBounded>(actual),
              get<FloatSetLiteralBounded>(expected), newInput);
  } else if (actual.type() == typeid(FloatSetLiteralSet)) {
    expect_eq(get<FloatSetLiteralSet>(actual),
              get<FloatSetLiteralSet>(expected), newInput);
  } else if (actual.type() == typeid(std::string)) {
    EXPECT_EQ(get<std::string>(actual), get<std::string>(expected)) << newInput;
  } else if (actual.type() == typeid(ArrayLiteral)) {
    expect_eq(get<ArrayLiteral>(actual), get<ArrayLiteral>(expected), newInput);
  } else {
    FAIL() << newInput;
  }
}

void expect_eq(const ParArrayLiteral &actual, const ParArrayLiteral &expected,
               const std::string &input) {
  const std::string newInput = input + "\nParArrayLiteral:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_vector(actual, expected, newInput);
}

void expect_eq(const ParExpr &actual, const ParExpr &expected,
               const std::string &input) {
  const std::string newInput = input + "\nParExpr:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_type(actual, expected, newInput);
  if (actual.type() == typeid(bool)) {
    EXPECT_EQ(get<bool>(actual), get<bool>(expected)) << newInput;
  } else if (actual.type() == typeid(int64_t)) {
    EXPECT_EQ(get<int64_t>(actual), get<int64_t>(expected)) << newInput;
  } else if (actual.type() == typeid(double)) {
    EXPECT_EQ(get<double>(actual), get<double>(expected)) << newInput;
  } else if (actual.type() == typeid(SetLiteralEmpty)) {
    expect_eq(get<SetLiteralEmpty>(actual), get<SetLiteralEmpty>(expected),
              input);
  } else if (actual.type() == typeid(IntSetLiteralBounded)) {
    expect_eq(get<IntSetLiteralBounded>(actual),
              get<IntSetLiteralBounded>(expected), newInput);
  } else if (actual.type() == typeid(IntSetLiteralSet)) {
    expect_eq(get<IntSetLiteralSet>(actual), get<IntSetLiteralSet>(expected),
              input);
  } else if (actual.type() == typeid(FloatSetLiteralBounded)) {
    expect_eq(get<FloatSetLiteralBounded>(actual),
              get<FloatSetLiteralBounded>(expected), newInput);
  } else if (actual.type() == typeid(FloatSetLiteralSet)) {
    expect_eq(get<FloatSetLiteralSet>(actual),
              get<FloatSetLiteralSet>(expected), newInput);
  } else if (actual.type() == typeid(ParArrayLiteral)) {
    expect_eq(get<ParArrayLiteral>(actual), get<ParArrayLiteral>(expected),
              input);
  } else {
    FAIL() << newInput;
  }
}

void expect_eq(const ParDeclItem &actual, const ParDeclItem &expected,
               const std::string &input) {
  const std::string newInput = input + "\nParDeclItem:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq(actual.type, expected.type, newInput);
  EXPECT_EQ(actual.identifier, expected.identifier) << newInput;
  expect_eq(actual.expr, expected.expr, newInput);
}

void expect_eq(const PredParamArrayType &actual,
               const PredParamArrayType &expected, const std::string &input) {
  const std::string newInput = input + "\nPredParamArrayType:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq(actual.indexSet, expected.indexSet, newInput);
  expect_eq(actual.type, expected.type, newInput);
}

void expect_eq(const parser::Annotation &actual,
               const parser::Annotation &expected, const std::string &input) {
  const std::string newInput = input + "\nAnnotation:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  EXPECT_EQ(actual.identifier, expected.identifier) << newInput;
  expect_eq_vector(actual.expressions, expected.expressions, newInput);
}

void expect_eq(const BasicAnnExpr &actual, const BasicAnnExpr &expected,
               const std::string &input) {
  const std::string newInput = input + "\nBasicAnnExpr:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_type(actual, expected, newInput);
  if (actual.type() == typeid(bool)) {
    EXPECT_EQ(get<bool>(actual), get<bool>(expected)) << newInput;
  } else if (actual.type() == typeid(int64_t)) {
    EXPECT_EQ(get<int64_t>(actual), get<int64_t>(expected)) << newInput;
  } else if (actual.type() == typeid(double)) {
    EXPECT_EQ(get<double>(actual), get<double>(expected)) << newInput;
  } else if (actual.type() == typeid(SetLiteralEmpty)) {
    expect_eq(get<SetLiteralEmpty>(actual), get<SetLiteralEmpty>(expected),
              input);
  } else if (actual.type() == typeid(IntSetLiteralBounded)) {
    expect_eq(get<IntSetLiteralBounded>(actual),
              get<IntSetLiteralBounded>(expected), newInput);
  } else if (actual.type() == typeid(IntSetLiteralSet)) {
    expect_eq(get<IntSetLiteralSet>(actual), get<IntSetLiteralSet>(expected),
              input);
  } else if (actual.type() == typeid(FloatSetLiteralBounded)) {
    expect_eq(get<FloatSetLiteralBounded>(actual),
              get<FloatSetLiteralBounded>(expected), newInput);
  } else if (actual.type() == typeid(FloatSetLiteralSet)) {
    expect_eq(get<FloatSetLiteralSet>(actual),
              get<FloatSetLiteralSet>(expected), newInput);
  } else if (actual.type() == typeid(std::string)) {
    EXPECT_EQ(get<std::string>(actual), get<std::string>(expected)) << newInput;
  } else if (actual.type() == typeid(x3::forward_ast<parser::Annotation>)) {
    expect_eq(get<x3::forward_ast<parser::Annotation>>(actual).get(),
              get<x3::forward_ast<parser::Annotation>>(expected).get(),
              newInput);
  } else {
    FAIL() << newInput;
  }
}

void expect_eq(const AnnExpr &actual, const AnnExpr &expected,
               const std::string &input) {
  const std::string newInput = input + "\nAnnExpr:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_vector(actual, expected, newInput);
}

void expect_eq(const Annotations &actual, const Annotations &expected,
               const std::string &input) {
  const std::string newInput = input + "\nAnnotations:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_vector(actual, expected, newInput);
}

void expect_eq(const BasicVarDecl &actual, const BasicVarDecl &expected,
               const std::string &input) {
  const std::string newInput = input + "\nBasicVarDecl:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq(actual.type, expected.type, newInput);
  expect_eq(actual.annotations, expected.annotations, newInput);
  EXPECT_EQ(actual.expr.has_value(), expected.expr.has_value()) << newInput;
  if (actual.expr.has_value()) {
    expect_eq(actual.expr.value(), expected.expr.value(), newInput);
  }
}

void expect_eq(const ArrayVarDecl &actual, const ArrayVarDecl &expected,
               const std::string &input) {
  const std::string newInput = input + "\nArrayVarDecl:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq(actual.type, expected.type, newInput);
  expect_eq(actual.annotations, expected.annotations, newInput);
  expect_eq(actual.literals, expected.literals, newInput);
}

void expect_eq(const VarDeclItem &actual, const VarDeclItem &expected,
               const std::string &input) {
  const std::string newInput = input + "\nVarDeclItem:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_type(actual, expected, newInput);
  if (actual.type() == typeid(BasicVarDecl)) {
    expect_eq(get<BasicVarDecl>(actual), get<BasicVarDecl>(expected), newInput);
  } else if (actual.type() == typeid(ArrayVarDecl)) {
    expect_eq(get<ArrayVarDecl>(actual), get<ArrayVarDecl>(expected), newInput);
  } else {
    FAIL() << newInput;
  }
}

void expect_eq(const ConstraintItem &actual, const ConstraintItem &expected,
               const std::string &input) {
  const std::string newInput = input + "\nConstraintItem:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq(actual.identifier, expected.identifier, newInput);
  expect_eq_vector(actual.expressions, expected.expressions, newInput);
  expect_eq(actual.annotations, expected.annotations, newInput);
}

void expect_eq(const SolveSatisfy &actual, const SolveSatisfy &expected,
               const std::string &input) {
  const std::string newInput = input + "\nSolveSatisfy:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq(actual.annotations, expected.annotations, newInput);
}

void expect_eq(const SolveOptimize &actual, const SolveOptimize &expected,
               const std::string &input) {
  const std::string newInput = input + "\nSolveOptimize:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq(actual.annotations, expected.annotations, newInput);
  EXPECT_EQ(actual.type, expected.type) << newInput;
  expect_eq(actual.expr, expected.expr, newInput);
}

void expect_eq(const SolveItem &actual, const SolveItem &expected,
               const std::string &input) {
  const std::string newInput = input + "\nSolveItem:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_type(actual, expected, newInput);
  if (actual.type() == typeid(SolveSatisfy)) {
    expect_eq(get<SolveSatisfy>(actual), get<SolveSatisfy>(expected), newInput);
  } else if (actual.type() == typeid(SolveOptimize)) {
    expect_eq(get<SolveOptimize>(actual), get<SolveOptimize>(expected),
              newInput);
  } else {
    FAIL() << newInput;
  }
}

void expect_eq(const PredicateItem &actual, const PredicateItem &expected,
               const std::string &input) {
  const std::string newInput = input + "\nPredicateItem:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  EXPECT_EQ(actual.identifier, expected.identifier) << newInput;
  expect_eq_vector(actual.params, expected.params, newInput);
}

void expect_eq(const parser::Model &actual, const parser::Model &expected,
               const std::string &input) {
  const std::string newInput = input + "\nModel:\nactual: \n" +
                               toString(actual) + "\nexpected:\n" +
                               toString(expected);
  expect_eq_vector(actual.predicateItems, expected.predicateItems, newInput);
  expect_eq_vector(actual.parDeclItems, expected.parDeclItems, newInput);
  expect_eq_vector(actual.varDeclItems, expected.varDeclItems, newInput);
  expect_eq_vector(actual.constraintItems, expected.constraintItems, newInput);
  expect_eq(actual.solveItem, expected.solveItem, newInput);
}
}  // namespace fznparser::testing