#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "domain.hpp"

namespace fznparser {
  enum AnnotationType { DEFINES_VAR, OUTPUT };

  class Annotation {
  private:
    AnnotationType _type;

  public:
    Annotation(AnnotationType type) : _type(type) {}

    AnnotationType type() { return _type; }
  };

  class Variable;
  class DefinesVarAnnotation : public Annotation {
  private:
    std::weak_ptr<Variable> _defines;

  public:
    DefinesVarAnnotation(std::weak_ptr<Variable> defines)
        : Annotation(AnnotationType::DEFINES_VAR), _defines(defines) {}

    std::weak_ptr<Variable> defines() { return _defines; }
  };

  class Variable {
  private:
    std::string _name;
    std::vector<std::shared_ptr<Annotation>> _annotations;

  public:
    Variable(std::string name, std::vector<std::shared_ptr<Annotation>> annotations)
        : _name(name), _annotations(annotations) {}

    virtual bool isArray() = 0;

    std::string_view name() { return std::string_view(_name); }
    const std::vector<std::shared_ptr<Annotation>>& annotations() { return _annotations; }
  };

  class ArrayVariable : public Variable {
  private:
    std::vector<std::shared_ptr<Variable>> _contents;

  public:
    ArrayVariable(std::string name, std::vector<std::shared_ptr<Annotation>> annotations,
                  std::vector<std::shared_ptr<Variable>> contents)
        : Variable(name, annotations), _contents(contents) {}

    const std::vector<std::shared_ptr<Variable>>& contents() { return _contents; }
    virtual bool isArray() override { return true; }
  };

  class SearchVariable : public Variable {
  private:
    std::unique_ptr<Domain> _domain;

  public:
    SearchVariable(std::string name, std::vector<std::shared_ptr<Annotation>> annotations,
                   std::unique_ptr<Domain> domain)
        : Variable(name, annotations), _domain(std::move(domain)) {}

    Domain* domain() { return _domain.get(); }
    virtual bool isArray() override { return false; }
  };
}  // namespace fznparser