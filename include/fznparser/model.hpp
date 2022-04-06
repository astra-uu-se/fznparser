#pragma once

#include <utility>

#include "fznparser/ast.hpp"

namespace fznparser {

  /**
   * The container for the FlatZinc model.
   */
  class FZNModel {
  private:
    Objective _objective;

  public:
    explicit FZNModel(Objective objective) : _objective(std::move(objective)) {}

    /**
     * Since the models can be quite large, the copy constructor
     * is deleted, but a move constructor is defined.
     */
    FZNModel(FZNModel& other) = delete;
    FZNModel(FZNModel&& other) = default;

    [[nodiscard]] const Objective& objective() const noexcept { return _objective; }
  };

}  // namespace fznparser