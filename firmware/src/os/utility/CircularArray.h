//
// Created by L Shaf on 17/12/25.
//

#pragma once

template<typename T>
class CircularBuffer {
private:
  T* _buffer;
  size_t _capacity;
  size_t _head;
  size_t _size;

public:
  explicit CircularBuffer(size_t capacity)
      : _capacity(capacity), _head(0), _size(0) {
    _buffer = new T[capacity];
  }

  ~CircularBuffer() {
    delete[] _buffer;
  }

  void push(const T& item) {
    _buffer[_head] = item;
    _head = (_head + 1) % _capacity;
    if (_size < _capacity) {
      _size++;
    }
  }

  T get(size_t index) const {
    if (index >= _size) {
      return T();
    }
    size_t actualIndex = (_head + _capacity - _size + index) % _capacity;
    return _buffer[actualIndex];
  }

  size_t size() const {
    return _size;
  }

  size_t capacity() const {
    return _capacity;
  }

  void clear() {
    _head = 0;
    _size = 0;
  }

  bool isEmpty() const {
    return _size == 0;
  }

  bool isFull() const {
    return _size == _capacity;
  }
};
