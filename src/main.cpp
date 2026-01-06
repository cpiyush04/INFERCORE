#include <iostream>
#include <vector>
#include <thread>
#include "queue.hpp"
#include "engine.hpp"

const int TOTAL_REQUESTS = 50;

int main() {
    std::cout << "Starting LiteServe Benchmark..." << std::endl;

    ThreadSafeQueue<Request> global_queue;
    InferenceEngine engine(global_queue);

    // 1. Start the Timer
    auto start_time = std::chrono::high_resolution_clock::now();

    // 2. Launch Producers (High Load)
    std::vector<std::thread> producers;
    for (int i = 0; i < TOTAL_REQUESTS; ++i) {
        producers.emplace_back([&global_queue, i]() {
            // Minimal delay to simulate high throughput
            std::this_thread::sleep_for(std::chrono::microseconds(100)); 
            Request req{i, 5 + (i % 5)};
            global_queue.push(req);
        });
    }

    // 3. Wait for producers
    for (auto& t : producers) t.join();
    // 4. Wait for queue to drain
    while (!global_queue.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Give the worker a moment to finish the last batch logic
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 5. Stop Timer
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end_time - start_time;

    std::cout << "\n========================================\n";
    std::cout << "Total Requests: " << TOTAL_REQUESTS << "\n";
    std::cout << "Total Time: " << diff.count() << " s\n";
    std::cout << "Throughput: " << TOTAL_REQUESTS / diff.count() << " req/s\n";
    std::cout << "========================================\n";

    return 0;
}
