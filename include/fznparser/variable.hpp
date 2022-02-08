#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include "annotationcollection.hpp"
#include "domain.hpp"

namespace fznparser {
  enum AnnotationType { DEFINES_VAR, DEFINED_VAR, INTRODUCED_VAR, OUTPUT };

  class Annotation {
  public:
    virtual ~Annotation() = default;
    virtual AnnotationType type() = 0;
  };

  class OutputAnnotation : public Annotation {
  public:
    ~OutputAnnotation() override = default;
    AnnotationType type() override { return OUTPUT; }
  };

  class DefinedVarAnnotation : public Annotation {
  public:
    ~DefinedVarAnnotation() override = default;
    AnnotationType type() override { return DEFINED_VAR; }
  };

  class IntroducedVarAnnotation : public Annotation {
  public:
    ~IntroducedVarAnnotation() override = default;
    AnnotationType type() override { return INTRODUCED_VAR; }
  };

  enum class LiteralType { SEARCH_VARIABLE, VARIABLE_ARRAY, PARAMETER, PARAMETER_ARRAY, VALUE };
  class Literal {
  public:
    virtual ~Literal() = default;
    virtual LiteralType type() = 0;
  };

  class ValueLiteral : public Literal {
  private:
    int64_t _value;

  public:
    explicit ValueLiteral(int64_t value) : _value(value) {}
    ~ValueLiteral() override = default;

    [[nodiscard]] int64_t value() const { return _value; }
    [[nodiscard]] LiteralType type() override { return LiteralType::VALUE; }
  };

  class NamedLiteral : public Literal {
  private:
    std::string _name;

  public:
    explicit NamedLiteral(std::string name) : _name(std::move(name)) {}
    ~NamedLiteral() override = default;

    std::string_view name() { return std::string_view(_name); }
  };

  class Variable;
  class DefinesVarAnnotation : public Annotation {
  private:
    std::weak_ptr<Variable> _defines;

  public:
    explicit DefinesVarAnnotation(std::weak_ptr<Variable> defines) : _defines(std::move(defines)) {}
    ~DefinesVarAnnotation() override = default;

    [[nodiscard]] std::weak_ptr<Variable> defines() const { return _defines; }
    [[nodiscard]] AnnotationType type() override { return AnnotationType::DEFINES_VAR; }
  };

  class Variable : public NamedLiteral {
  private:
    AnnotationCollection _annotations;

  public:
    Variable(std::string name, AnnotationCollection annotations)
        : NamedLiteral(std::move(name)), _annotations(std::move(annotations)) {}
    ~Variable() override = default;

    const AnnotationCollection& annotations() { return _annotations; }
  };

  class SearchVariable : public Variable {
  private:
    std::unique_ptr<Domain> _domain;

  public:
    SearchVariable(std::string name, AnnotationCollection annotations,
                   std::unique_ptr<Domain> domain)
        : Variable(std::move(name), annotations), _domain(std::move(domain)) {}
    ~SearchVariable() override = default;

    Domain* domain() { return _domain.get(); }
    LiteralType type() override { return LiteralType::SEARCH_VARIABLE; }
  };

  /**
   * An array of search variables. In line with the FlatZinc specification, variable arrays are
   * always 1-dimensional. Hence, the type of the content items is SearchVariable instead of the
   * common Variable type.
   *
   * The index set for array variables is always 1..n where n is the size of the
   * array. There is therefore no need to separately store the index set, as the
   * number of variables gives all the information.
   */
  class VariableArray : public Variable {
  private:
    std::vector<std::shared_ptr<SearchVariable>> _contents;

  public:
    VariableArray(std::string name, AnnotationCollection annotations,
                  std::vector<std::shared_ptr<SearchVariable>> contents)
        : Variable(std::move(name), annotations), _contents(std::move(contents)) {}
    ~VariableArray() override = default;

    [[nodiscard]] const std::vector<std::shared_ptr<SearchVariable>>& contents() const {
      return _contents;
    }
    [[nodiscard]] size_t size() const { return _contents.size(); }

    LiteralType type() override { return LiteralType::VARIABLE_ARRAY; }
  };

  class SingleParameter : public NamedLiteral {
  private:
    int64_t _value;

  public:
    SingleParameter(std::string name, int64_t value)
        : NamedLiteral(std::move(name)), _value(value) {}
    ~SingleParameter() override = default;

    [[nodiscard]] int64_t value() const { return _value; }
    LiteralType type() override { return LiteralType::PARAMETER; }
  };

  /**
   * An array of parameters. According to the FlatZinc specification, parameter arrays are
   * 1-dimensional, and therefore the contents is of type int64_t instead of Parameter or other.
   */
  class ParameterArray : public NamedLiteral {
  private:
    std::vector<int64_t> _contents;

  public:
    ParameterArray(std::string name, std::vector<int64_t> contents)
        : NamedLiteral(std::move(name)), _contents(std::move(contents)) {}

    [[nodiscard]] const std::vector<int64_t>& contents() const { return _contents; }
    [[nodiscard]] size_t size() const { return _contents.size(); }

    LiteralType type() override { return LiteralType::PARAMETER_ARRAY; }
  };

  typedef std::variant<std::shared_ptr<SingleParameter>, std::shared_ptr<ParameterArray>> Parameter;
}  // namespace fznparser