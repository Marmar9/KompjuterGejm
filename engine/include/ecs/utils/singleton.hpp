#pragma once

#include "inc/common/exception.hpp"
#include <memory>
#include <mutex>
#include <utility>
template <typename T> class Singleton {
  static std::unique_ptr<T> _instance;
  static std::once_flag _iFlag;
  static std::once_flag _dFlag;

public:
  Singleton(Singleton const &) = delete;
  Singleton &operator=(Singleton const &) = delete;
  Singleton(Singleton &&) = delete;
  Singleton &operator=(Singleton &&) = delete;

  static T &getInstance() {
    if (!_instance)
      THROW_EXCEPTION("Singleton not initialized");
    return *_instance;
  }

  template <typename... Args> static T &init(Args &&...args) {
    std::call_once(
        _iFlag, [&]() { _instance.reset(new T(std::forward<Args>(args)...)); });
    return *_instance;
  }
  static void destroy() {
    std::call_once(_dFlag, [&]() { _instance.reset(); });
  }

protected:
  Singleton() = default;
  ~Singleton() = default;
};

template <typename T> std::unique_ptr<T> Singleton<T>::_instance = nullptr;

template <typename T> std::once_flag Singleton<T>::_iFlag;
template <typename T> std::once_flag Singleton<T>::_dFlag;
