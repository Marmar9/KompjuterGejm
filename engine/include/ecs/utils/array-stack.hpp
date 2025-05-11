#pragma once

#include "inc/common/exception.hpp"
#include <array>

template <typename T, std::size_t MaxSize> class ArrayStack {
public:
  ArrayStack() : _head(0) {}

  void push(const T &value) {
    if (_head == MaxSize - 1) {
      THROW_EXCEPTION("Reached entity limit");
    }

    _buffer[_head++] = value;
  }

  T pop() {
    if (_head == 0)
      THROW_EXCEPTION("Stack is empty");
    return _buffer[--_head];
  }

private:
  std::size_t _head;

  template <std::size_t... Is>
  static constexpr std::array<T, MaxSize>
  make_buffer(std::index_sequence<Is...>) {
    return std::array<T, MaxSize>{{Is...}};
  }

  std::array<T, MaxSize> _buffer =
      make_buffer(std::make_index_sequence<MaxSize>{});
};
