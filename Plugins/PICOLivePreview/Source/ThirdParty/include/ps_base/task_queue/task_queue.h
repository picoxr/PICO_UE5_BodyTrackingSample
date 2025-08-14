#pragma once
#include <memory>

#include "ps_base/ps_exports.h"
#include "ps_base/task_queue/task.h"
namespace ps_base {
class Task;
class PS_BASE_API TaskQueue {
 public:
  typedef std::shared_ptr<TaskQueue> Ptr_t;
  static Ptr_t Create(size_t thread_num, const char* queue_name);
  TaskQueue() = default;
  virtual ~TaskQueue() = default;

  virtual void PostTask(std::unique_ptr<Task> task, bool in_order = false) = 0;
  virtual void DispatchTask(std::unique_ptr<Task> task,
                            bool in_order = false) = 0;

  template <typename Closure>
  inline void PostTask(Closure&& closure) {
    PostTask(ToQueuedTask(closure));
  }

  template <typename Closure>
  inline void DispatchTask(Closure&& closure) {
    DispatchTask(ToQueuedTask(closure));
  }

  virtual bool IsQueueThread() = 0;
  virtual void PostDelayedTask(std::unique_ptr<Task> task,
                               uint32_t delay_mills) = 0;
  template <typename Closure>
  inline void PostDelayedTask(Closure&& closure, uint32_t delay_mills) {
    PostDelayedTask(ToQueuedTask(closure), delay_mills);
  }
};

void PS_BASE_API WaitAllTaskQueueExit();
}  // namespace ps_base
