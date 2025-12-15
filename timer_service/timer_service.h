#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>
#include <memory>
#include <atomic>
#include <iostream>
#include <string>

/**
 * A generic timer service that manages multiple timers with a single worker thread.
 * Callbacks can accept any type of argument, which is copied and stored with the timer.
 */
class TimerService {
public:
    using TimerId = uint64_t;
    
    TimerService();
    ~TimerService();
    
    // Delete copy and move operations
    TimerService(const TimerService&) = delete;
    TimerService& operator=(const TimerService&) = delete;
    TimerService(TimerService&&) = delete;
    TimerService& operator=(TimerService&&) = delete;
    
    /**
     * Schedule a timer that will call the callback with the given argument after delay.
     * The argument is copied and stored within the timer.
     * 
     * @param delay Time to wait before executing callback
     * @param callback Function to call (must return void and accept single argument)
     * @param arg Argument to pass to callback (will be copied)
     * @return TimerId that can be used to cancel the timer
     */
    template<typename Func, typename Arg>
    TimerId schedule(std::chrono::milliseconds delay, Func&& callback, Arg&& arg) {
        auto expiry = std::chrono::steady_clock::now() + delay;
        
        // Create a type-erased wrapper that captures both callback and argument
        // The argument is copied here, so caller doesn't need to preserve it
        auto timer_callback = [callback = std::forward<Func>(callback), 
                              arg = std::forward<Arg>(arg)]() mutable {
            callback(arg);
        };
        
        std::lock_guard<std::mutex> lock(mutex_);
        TimerId id = next_id_++;
        timers_.emplace(expiry, TimerData{id, std::move(timer_callback)});
        cv_.notify_one(); // Wake up worker thread
        return id;
    }
    
    /**
     * Cancel a scheduled timer by ID.
     * 
     * @param id Timer ID returned from schedule()
     * @return true if timer was found and cancelled, false otherwise
     */
    bool cancel(TimerId id);
    
private:
    using TimePoint = std::chrono::steady_clock::time_point;

    struct TimerData {
        TimerId id;
        std::function<void()> callback;
    };
    
    /**
     * Worker thread that processes expired timers
     */
    void worker_();
    
    std::thread worker_thread_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    std::multimap<TimePoint, TimerData> timers_; // Ordered by expiration time
    std::atomic<bool> running_;
    std::atomic<TimerId> next_id_;
};