#pragma once
#include <mutex>
#include <shared_mutex>

#define PS_LOCK_MTX(LK, MT) std::unique_lock<std::mutex> LK(MT)
#define PS_LOCK_REC(LK, MT) std::unique_lock<std::recursive_mutex> LK(MT)
#define PS_LOCK_READ(LK, MT) std::shared_lock<std::shared_mutex> LK(MT)
#define PS_LOCK_WRITE(LK, MT) std::unique_lock<std::shared_mutex> LK(MT)