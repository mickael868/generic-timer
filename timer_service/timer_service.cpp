#include "timer_service.h"

TimerService::TimerService() : running_(true), next_id_(1) {
    worker_thread_ = std::thread(&TimerService::worker_, this);
}

TimerService::~TimerService() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = false;
    }
    cv_.notify_one();
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
}

bool TimerService::cancel(TimerId id) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = timers_.begin(); it != timers_.end(); ++it) {
        if (it->second.id == id) {
            timers_.erase(it);
            cv_.notify_one(); // Wake up worker to recalculate next wait time
            return true;
        }
    }
    return false;
}

void TimerService::worker_() {
    while (true) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // Check if we should stop
        if (!running_) {
            break;
        }
        
        // If no timers, wait for notification
        if (timers_.empty()) {
            cv_.wait(lock);
            continue;
        }
        
        auto now = std::chrono::steady_clock::now();
        auto it = timers_.begin();
        
        if (it->first <= now) {
            // Timer expired, execute callback
            auto callback = std::move(it->second.callback);
            timers_.erase(it);
            
            // Execute callback without holding the lock
            lock.unlock();
            callback();
        } else {
            // Wait until next timer expires or notification
            cv_.wait_until(lock, it->first);
        }
    }
}