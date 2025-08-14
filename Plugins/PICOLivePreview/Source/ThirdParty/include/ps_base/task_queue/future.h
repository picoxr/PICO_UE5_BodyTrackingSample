#pragma once

#include <condition_variable>
#include <functional>
#include <list>
#include <mutex>

namespace ps_base {

template<typename Result, typename Value>
struct InternalState {
  std::mutex mutex;
  std::condition_variable condition;
  Result result;
  Value value;
  bool complete = false;

  std::list<typename std::function<void(Result, const Value &)> > callbacks;
};

template<typename Result, typename Value>
class Future {
 public:
  typedef std::function<void(Result, const Value &)> ListenerCallback;

  Future &Then(ListenerCallback callback) {
    std::unique_lock<std::mutex> lock(ap_state_->mutex);
    if (ap_state_->complete) {
      lock.unlock();
      callback(ap_state_->result, ap_state_->value);
    } else {
      ap_state_->callbacks.push_back(callback);
    }
    return *this;
  }

  Result Get(Value *p_value) {
    std::unique_lock<std::mutex> lock(ap_state_->mutex);
    ap_state_->condition.wait(lock, [this] {
      return ap_state_->complete;
    });
    if(p_value) {
      *p_value = ap_state_->value;
    }
    return ap_state_->result;
  }

 private:
  explicit Future(std::shared_ptr<InternalState<Result, Value> > state)
      : ap_state_(state) {}

  std::shared_ptr<InternalState<Result, Value> > ap_state_;

  template <typename U, typename V>
  friend class Promise;
};

template<typename Result, typename Value>
class Promise {
 public:
  Promise() : ap_state_(std::make_shared<InternalState<Result, Value> >()) {}
  Promise(const Promise<Result, Value> &other) : ap_state_(other.ap_state_) {}
  Promise(Promise<Result, Value> &&other) noexcept
      : ap_state_(std::move(other.ap_state_)) {}
  Promise<Result, Value> &operator=(const Promise<Result, Value> &other) {
    ap_state_ = other.ap_state_;
    return *this;
  }
  Promise<Result, Value> &operator=(Promise<Result, Value> &&other)  noexcept {
    ap_state_ = std::move(other.ap_state_);
    return *this;
  }

  bool SetValue(Result result, Value &&value) const {
    std::unique_lock<std::mutex> lock(ap_state_->mutex);

    if (ap_state_->complete) {
      return false;
    }

    ap_state_->result = result;
    ap_state_->value = std::forward<Value>(value);
    ap_state_->complete = true;

    decltype(ap_state_->callbacks) listeners;
    listeners.swap(ap_state_->callbacks);

    lock.unlock();

    for (auto& callback : listeners) {
      callback(result, ap_state_->value);
    }

    ap_state_->condition.notify_all();
    return true;
  }

  [[nodiscard]] bool Completed() const {
    std::unique_lock<std::mutex> lock(ap_state_->mutex);
    return ap_state_->complete;
  }

  Future<Result, Value> GetFuture() const {
    return Future<Result, Value>(ap_state_);
  }

 private:
  std::shared_ptr<InternalState<Result, Value> > ap_state_;
};

} // namespace ps_base
