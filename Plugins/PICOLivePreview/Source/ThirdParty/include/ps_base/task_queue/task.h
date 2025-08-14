#pragma once
#include <memory>

namespace ps_base {
class Task {
 public:
  virtual ~Task() = default;
  virtual bool Run() = 0;
};

template <typename Closure>
class ClosureTask : public Task {
 public:
  explicit ClosureTask(Closure&& closure)
      : closure_(std::forward<Closure>(closure)) {}

 private:
  bool Run() override {
    closure_();
    return true;
  }

  typename std::decay<Closure>::type closure_;
};

template <typename Closure>
std::unique_ptr<Task> ToQueuedTask(Closure&& closure) {
  return std::make_unique<ClosureTask<Closure>>(std::forward<Closure>(closure));
}
}  // namespace ps_base