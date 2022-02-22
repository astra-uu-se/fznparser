#include <filesystem>
#include <memory>

#include "model.hpp"

namespace fznparser {
  class ModelFactory {
  public:
    /**
     * @brief Create a Model instance from a FlatZinc file.
     *
     * @param modelPath The path to the file that contains the FlatZinc model.
     * @return Model A representation of the model in the file.
     */
    static std::unique_ptr<Model> create(const std::filesystem::path& modelPath);
  };
}  // namespace fznparser