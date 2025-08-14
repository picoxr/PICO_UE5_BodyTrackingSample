#pragma once
#include <mutex>
#include <utility>

#include "ps_base/logger.h"
#include "ps_base/task_queue/task_queue.h"
namespace ps_base {
template <class T>
class TaskQueueCaller {
 private:
  typedef std::shared_ptr<T> Ptr_t;

 public:
  template <class Func>
  inline bool CallInTaskQueue(Func f, size_t timeout_ms = 0) {
    Ptr_t ptr = static_cast<T*>(this)->shared_from_this();
    if (task_queue_->IsQueueThread()) {
      return f(ptr);
    }

    bool res = false;
    if (timeout_ms) {
      std::mutex mtx;
      std::unique_lock<std::mutex> lck(mtx);
      std::condition_variable cond;
      task_queue_->PostTask([f, &res, &cond, ptr]() {
        res = f(ptr);
        cond.notify_all();
      });
      auto tmout = std::chrono::milliseconds(timeout_ms);
      if (cond.wait_for(lck, tmout) != std::cv_status::no_timeout) {
        return false;
      }
    } else {
      task_queue_->PostTask([f, ptr]() { f(ptr); });
      res = true;
    }
    return res;
  }

  template <class Func>
  inline bool CallInTaskQueue(Func f, const std::string& task_name,
                              size_t timeout_ms = 0) {
    Ptr_t ptr = static_cast<T*>(this)->shared_from_this();
    if (task_queue_->IsQueueThread()) {
      return f(ptr);
    }

    bool res = false;
    if (timeout_ms) {
      bool complete = false;
      std::mutex mtx;
      std::condition_variable cond;
      task_queue_->PostTask([f, &res, &complete, &mtx, &cond, ptr]() {
        res = f(ptr);
        {
          std::unique_lock<std::mutex> lock(mtx);
          complete = true;
        }
        cond.notify_one();
      });
      auto tmout = std::chrono::milliseconds(timeout_ms);
      std::unique_lock<std::mutex> lock(mtx);
      if (!cond.wait_for(lock, tmout, [&complete]() { return complete; })) {
        PS_WARN_THIS("Task timeout, name: " << task_name << ", timeout in ms: "
                                            << timeout_ms)
        return false;
      }

    } else {
      task_queue_->PostTask([f, ptr]() { f(ptr); });
      res = true;
    }
    return res;
  }

 protected:
  explicit TaskQueueCaller(std::shared_ptr<TaskQueue> task_queue)
      : task_queue_(std::move(task_queue)) {}
  std::shared_ptr<TaskQueue> task_queue_;
};
}  // namespace ps_base