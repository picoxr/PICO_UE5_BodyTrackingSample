#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>

namespace ps_base {
class Semaphore {
 public:
  explicit Semaphore(int n = 0) : count_(n) {}
  Semaphore(const Semaphore& other) = delete;
  Semaphore(Semaphore&& other) = delete;
  Semaphore& operator=(const Semaphore& other) = delete;
  Semaphore& operator=(Semaphore&& other) = delete;

  void Release(int n = 1) {
    std::unique_lock<std::mutex> lock(mu_);
    count_ += n;
    cv_.notify_all();
  }
  void Acquire(int n = 1) {
    std::unique_lock<std::mutex> lock(mu_);
    cv_.wait(lock, [this, n] { return count_ >= n; });
    count_ -= n;
  }
  template <class R, class P>
  bool Acquire(const std::chrono::duration<R, P>& time_out, int n = 1) {
    std::unique_lock<std::mutex> lock(mu_);
    if(!cv_.wait_for(lock, time_out, [this, n] { return count_ >= n; })) {
      return false;
    }
    count_ -= n;
    return true;
  }

 private:
  int count_;
  std::mutex mu_;
  std::condition_variable cv_;
};

class ThreadBarrier {
 public:
  explicit ThreadBarrier(int n) : arrived_(0), total_(n) {}
  ThreadBarrier(const ThreadBarrier& other) = delete;
  ThreadBarrier(ThreadBarrier&& other) = delete;
  ThreadBarrier& operator=(const ThreadBarrier& other) = delete;
  ThreadBarrier& operator=(ThreadBarrier&& other) = delete;

  void Arrive() {
    std::unique_lock<std::mutex> lock(mu_);
    ++arrived_;
    if (arrived_ == total_) {
      cv_.notify_all();
    } else {
      cv_.wait(lock, [this] { return arrived_ == total_; });
    }
  }
  template <class R, class P>
  bool Arrive(const std::chrono::duration<R, P>& time_out) {
    std::unique_lock<std::mutex> lock(mu_);
    ++arrived_;
    if (arrived_ == total_) {
      cv_.notify_all();
      return true;
    } else {
      return cv_.wait_for(lock, time_out, [this] { return arrived_ == total_; });
    }
  }

 private:
  int arrived_;
  int total_;
  std::mutex mu_;
  std::condition_variable cv_;
};

class ThreadEvent {
 public:
  ThreadEvent() = default;
  ThreadEvent(const ThreadEvent& other) = delete;
  ThreadEvent(ThreadEvent&& other) = delete;
  ThreadEvent& operator=(const ThreadEvent& other) = delete;
  ThreadEvent& operator=(ThreadEvent&& other) = delete;

  void Set() {
    std::unique_lock<std::mutex> lock(mu_);
    signaled_ = true;
    cv_.notify_all();
  }
  void Wait() {
    std::unique_lock<std::mutex> lock(mu_);
    cv_.wait(lock, [this] { return signaled_; });
  };
  template <class R, class P>
  bool Wait(const std::chrono::duration<R, P>& time_out) {
    std::unique_lock<std::mutex> lock(mu_);
    return cv_.wait_for(lock, time_out, [this] { return signaled_; });
  }

 private:
  bool signaled_ = false;
  std::mutex mu_;
  std::condition_variable cv_;
};

}  // namespace ps_base
