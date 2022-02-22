#pragma once

#include <typeindex>
#include <typeinfo>
#include <unordered_map>

namespace fznparser {
  class Annotation;

  class AnnotationCollection {
  protected:
    std::unordered_map<std::type_index, std::shared_ptr<Annotation>> _annotations;

  public:
    template <typename T> bool has() const {
      return _annotations.find(typeid(T)) != _annotations.end();
    }

    template <typename T> std::shared_ptr<T> get() const {
      auto ann = _annotations.at(typeid(T));
      return std::dynamic_pointer_cast<T>(ann);
    }

    size_t size() const { return _annotations.size(); }
  };

  class MutableAnnotationCollection : public AnnotationCollection {
  public:
    template <class T, typename... Args>
    std::enable_if_t<std::is_base_of<Annotation, T>::value, std::shared_ptr<T>> add(
        Args&&... args) {
      std::shared_ptr<T> ptr = std::make_shared<T>(std::forward<Args>(args)...);
      _annotations.emplace(typeid(T), std::static_pointer_cast<Annotation>(ptr));
      return ptr;
    }
  };
}  // namespace fznparser