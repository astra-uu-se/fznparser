#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "domain.hpp"

namespace fznparser {
  enum AnnotationType { DEFINES_VAR, OUTPUT };

  class Annotation {
  public:
    virtual ~Annotation() = default;
    virtual AnnotationType type() = 0;
  };

  class MarkerAnnotation : public Annotation {
  private:
    AnnotationType _type;

  public:
    explicit MarkerAnnotation(AnnotationType type) : _type(type) {}
    ~MarkerAnnotation() override = default;

    AnnotationType type() override { return _type; }
  };

  class Variable;
  class DefinesVarAnnotation : public Annotation {
  private:
    std::vector<std::weak_ptr<Variable>> _defines;

  public:
    explicit DefinesVarAnnotation(std::vector<std::weak_ptr<Variable>> defines)
        : _defines(std::move(defines)) {}
    ~DefinesVarAnnotation() override = default;

    [[nodiscard]] const std::vector<std::weak_ptr<Variable>>& defines() const { return _defines; }
    [[nodiscard]] AnnotationType type() override { return AnnotationType::DEFINES_VAR; }
  };

  class Variable {
  private:
    std::string _name;
    std::vector<std::shared_ptr<Annotation>> _annotations;

  public:
    Variable(std::string name, std::vector<std::shared_ptr<Annotation>> annotations)
        : _name(std::move(name)), _annotations(std::move(annotations)) {}
    virtual ~Variable() = default;

    virtual bool isArray() = 0;

    std::string_view name() { return std::string_view(_name); }
    const std::vector<std::shared_ptr<Annotation>>& annotations() { return _annotations; }
  };

  /**
   * @brief A flatzinc array variable is a container for other, explicitly
   * defined, variables. Hence, this is just a collection of pointers to
   * variables.
   *
   * The index set for array variables is always 1..n where n is the size of the
   * array. There is therefore no need to separately store the index set, as the
   * number of variables gives all the information.
   */
  class ArrayVariable : public Variable {
  private:
    std::vector<std::shared_ptr<Variable>> _contents;

  public:
    ArrayVariable(std::string name, std::vector<std::shared_ptr<Annotation>> annotations,
                  std::vector<std::shared_ptr<Variable>> contents)
        : Variable(std::move(name), std::move(annotations)), _contents(std::move(contents)) {}
    ~ArrayVariable() override = default;

    [[nodiscard]] const std::vector<std::shared_ptr<Variable>>& contents() const {
      return _contents;
    }
    [[nodiscard]] size_t size() const { return _contents.size(); }

    bool isArray() override { return true; }
  };

  class SearchVariable : public Variable {
  private:
    std::unique_ptr<Domain> _domain;

  public:
    SearchVariable(std::string name, std::vector<std::shared_ptr<Annotation>> annotations,
                   std::unique_ptr<Domain> domain)
        : Variable(std::move(name), std::move(annotations)), _domain(std::move(domain)) {}
    ~SearchVariable() override = default;

    Domain* domain() { return _domain.get(); }
    bool isArray() override { return false; }
  };
}  // namespace fznparser