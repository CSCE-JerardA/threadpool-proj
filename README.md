# Multi-Threaded Batch SHA-256 Encryptor

## Description
A high-performance C++ utility that demonstrates parallel processing using the POSIX Threads (pthreads) library. The application reads a dataset of strings and performs iterative SHA-256 hashing.
It is designed to showcase thread synchronization, workload distribution, and memory safety in a Linux environment.

## Key Features
- **Concurrent Execution:** Supports multiple threading modes including simultaneous launch (`--all`), sequential hand-off (`--thread`), and timed intervals (`--rate`).
- **Race Condition Avoidance:** Implements a strided data decomposition strategy. Each thread is assigned unique indices in the results table, ensuring no two threads write to the same memory location.
- **Thread-Safe Logging:** Utilizes a synchronized logging wrapper to prevent terminal output corruption (interleaving) during parallel execution.
- **Memory Integrity:** Verified with Valgrind to ensure zero memory leaks and proper deallocation of heap-allocated thread arguments.



## Technical Architecture
### Workload Distribution
The project avoids expensive mutex locking by using a mathematical stride for data assignment:
`index = thread_id + (iteration * total_threads)`
This ensures total isolation of data writes between threads.

### Synchronization Flow
1. **Initialization:** Threads are spawned and enter a "busy-wait" state using volatile boolean flags.
2. **Input:** The main thread captures the user-defined thread limit ($k$) from `/dev/tty`.
3. **Release:** Flags are toggled to trigger execution based on the selected mode.
4. **Collation:** `pthread_join` acts as a barrier, ensuring all computations are complete before the final results table is rendered to `stdout`.



## How to Run

### Build
`bash`
`make clean` && `make`

### Execute
`./bin/proj1 --all --timeout 5000 < dat/small.txt`
`./bin/proj1 --all --timeout 5000 < dat/medium.txt`

