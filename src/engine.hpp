#pragma once
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include "queue.hpp"

class InferenceEngine {
    ThreadSafeQueue<Request>& request_queue_;
    std::thread worker_thread_;
    // bool running_;
    

public:
    InferenceEngine(ThreadSafeQueue<Request>& queue) 
        : request_queue_(queue) {
        // Start the background worker immediately
        worker_thread_ = std::thread(&InferenceEngine::loop, this);
    }

    ~InferenceEngine() {
        std::cout << "[Main] Destructor called. Stopping queue...\n";
    
        // 1. Unblock the waiting thread
        request_queue_.shutdown();
        if (worker_thread_.joinable()) worker_thread_.join();
        std::cout << "[Main] Worker stopped. Exiting cleanly.\n";
    }

private:

    // Consumer loop
    void loop() {
        while (true) {

            Request req; // Create empty request holder

            // 1. Pass 'req' by reference. Check return value.
            bool active = request_queue_.wait_and_pop(req);

            // 2. If false, Break the loop immediately!
            if (!active) {
                std::cout << "[Worker] Shutdown signal received. Exiting.\n";
                break;
            }

            // Simulate processing time based on input token count
            int processing_time = req.input_token_count * 10; 
            
            std::cout << "[Worker] Processing Request ID: " << req.id 
                      << " (" << req.input_token_count << " tokens)..." << std::endl;
            
            // Artificial delay to simulate GPU/CPU compute
            std::this_thread::sleep_for(std::chrono::milliseconds(processing_time));

            std::cout << "[Worker] Finished Request ID: " << req.id << std::endl;
        }
    }
};

