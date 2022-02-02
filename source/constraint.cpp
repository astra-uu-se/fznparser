#include "fznparser/constraint.hpp"

#include <algorithm>
#include <memory>
#include <string>

std::shared_ptr<fznparser::Constraint> fznparser::Constraint::create(
    const std::string& name, const std::vector<std::shared_ptr<fznparser::Annotation>>& annotations,
    const std::vector<std::shared_ptr<fznparser::Variable>>& arguments) {
  auto definesVarAnn
      = std::find_if(annotations.begin(), annotations.end(), [](const auto& annotation) {
          return annotation->type() == fznparser::AnnotationType::DEFINES_VAR;
        });

  if (definesVarAnn == std::end(annotations)) {
    return std::make_shared<fznparser::NonFunctionalConstraint>(name, arguments);
  }

  auto definedVars
      = std::dynamic_pointer_cast<fznparser::DefinesVarAnnotation>(*definesVarAnn)->defines();

  std::vector<std::shared_ptr<fznparser::Variable>> outputs;
  for (const auto& weak_ptr : definedVars) {
    if (weak_ptr.expired()) throw std::logic_error("Variable was de-allocated already");
    outputs.push_back(weak_ptr.lock());
  }

  std::vector<std::shared_ptr<fznparser::Variable>> inputs;
  inputs.reserve(arguments.size() - definedVars.size());
  std::copy_if(arguments.begin(), arguments.end(), std::back_inserter(inputs),
               [&outputs](auto var) {
                 return std::find(outputs.begin(), outputs.end(), var) == std::end(outputs);
               });

  std::shared_ptr<fznparser::FunctionalConstraint> constraint
      = std::make_shared<fznparser::FunctionalConstraint>(name, inputs, outputs);
  return std::static_pointer_cast<fznparser::Constraint>(constraint);
}
