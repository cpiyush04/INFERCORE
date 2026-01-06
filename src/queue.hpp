#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

// a simple structure to simulate an infernence request to an LLM
struct Request {
    int id; // Unique ID for the user's request
    int input_token_count; // length of input tokens
};

template<typename T>

class ThreadSafeQueue {
private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cv_;
    bool stopped_ = false;

public:

    // push new incoming request to the queue
    void push(T value) {
        {
            // using a lock guard because we need it for this block only
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(value);
        }

         // waking up one waiting worker thread after unlocking
        cv_.notify_one();
    }

    // NEW: Call this from the main thread to kill the worker
    void shutdown() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopped_ = true;
        }
        cv_.notify_all(); // <--- CRITICAL: Wake up everyone sleeping!
    }

    // // Non-blocking pop (for batching logic later)
    // std::optional<T> try_pop() {
    //     std::lock_guard<std::mutex> lock(mutex_);
    //     if (queue_.empty()) {
    //         return std::nullopt;
    //     }
    //     T value = queue_.front();
    //     queue_.pop();
    //     return value;
    // }

    // waiting for new data to be available and popping it
    bool wait_and_pop(T& out_value) {
        std::unique_lock<std::mutex> lock(mutex_);
        
        // FIX 1: Add '|| stopped_' to the check
        // Wake up if we have data OR if we were told to stop
        cv_.wait(lock, [this] { 
            return !queue_.empty() || stopped_; 
        });

        // FIX 2: Check if we are stopping and empty
        if (stopped_ && queue_.empty()) {
            return false; // Signal to the worker to quit
        }

        // We have data, process normally
        out_value = queue_.front();
        queue_.pop();
        return true;
    }
    
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
};

