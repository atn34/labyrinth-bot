#ifndef RING_BUFFER_H
#define RING_BUFFER_H

template <typename T, int CAPACITY>
class RingBuffer {
 public:
  static_assert(CAPACITY > 0, "CAPACITY must be positive and nonzero");

  RingBuffer() { contents_.resize(CAPACITY); }
  virtual ~RingBuffer() = default;

  void add(T element) {
    using std::swap;
    if (size_ == CAPACITY) {
      swap(contents_[front_], element);
      front_ = (front_ + 1) % CAPACITY;
    } else {
      swap(contents_[size_], element);
      ++size_;
    }
  }

  /// get(0) gets the most recent entry.
  const T &get(int i) const {
    assert(size_ > 0);
    assert(i < size_);
    return contents_[(front_ + size_ - i - 1) % CAPACITY];
  }

  int size() { return size_; }

  bool full() { return size_ == CAPACITY; }

 private:
  int front_ = 0;
  int size_ = 0;
  std::vector<T> contents_;
};

#endif /* RING_BUFFER_H */
