#pragma once

#include <mutex>
#include <queue>
#include <condition_variable>

template<typename T>
class BlockingQueue
{
 public:
	 BlockingQueue()
		 : mutex_(),
		 notEmpty_(),
		 queue_()
  {
  }

  void put(const T& x)
  {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push_back(x);
	notEmpty_.notify_one();
  }

  void put(T&& x)
  {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push_back(std::move(x));
	notEmpty_.notify_one();
  }

  T take()
  {
    std::unique_lock<std::mutex> lock(mutex_);
    // always use a while-loop, due to spurious wakeup
    while (queue_.empty()) {
      notEmpty_.wait(lock);
    }

    assert(!queue_.empty());
    T front(std::move(queue_.front()));
    queue_.pop_front();
	return front;
  }

  size_t size() const
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
  }

 private:
  mutable std::mutex mutex_;
  std::condition_variable notEmpty_;
  std::deque<T>     queue_;
};

