# INFERCORE
A High-Throughput Inference Engine

## Performance Benchmarks

All benchmarks were conducted on a simulated inference workload
(variable token length, ~70ms compute per request).

**Test Conditions:** Burst traffic pattern, 50 concurrent client threads.

### Baseline: Serial Execution (v0.1)

| Version | Architecture | Load | Throughput | Max Latency (Tail) | Observations |
| :--- | :--- | :--- | :--- | :--- | :--- |
| v0.1 | Serial Execution | 50 reqs | 12.13 req/s | 3.95s | Severe head-of-line blocking under concurrent load |

**Analysis:**  
Although individual request compute time was modest, serial execution caused requests to queue behind long-running operations, leading to extreme tail latency under burst traffic.
