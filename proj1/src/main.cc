// Copyright Jerard Austin 2026

#include <unistd.h>
#include <iostream>
#include <pthread.h>  // For my use of threads
#include <vector>
#include <string>
#include <fstream>  // For handling my files
#include <sys/sysinfo.h>  // Used this for the get_nprocs

// For the provided libraries
#include <proj1/lib/timings.h> // For getting my threads to wait
#include <proj1/lib/cli_parser.h> // 
#include <proj1/lib/thread_log.h> // For displaying; used like a print statement
#include <proj1/lib/sha256.h>  // For output and doing the math


using namespace std;

// Structure for the data of my threads
struct ThreadsArg {
    int index;  // The ID
    int* k_ptr;  // The active threads
    bool* released_flags;  // Pointer for flagged array

    CliMode mode;  // For releasing of threads
    const std::vector<std::string>* fdata;  // Pointer to the file data

};


// Function for what the pthreads execute
void ThreadRoutine (void* arg) {
    
    // Works as a pointer the the variables within my struct
    ThreadsArg* data = static_cast<ThreadsArg*>(arg);

    // My local variables for connecting my variables through member access operators (arrows)
    // Used cppreference for this part to understand what I was doing
    int my_id = data->index;
    bool* flags = data->released_flags;
    CliMode mode = data->mode;
    

    // Make threads wait until released
    while (!flags[my_id]) {
        Timings_SleepMs(1);
    }

    std::cout << my_id << "'s Starting Routine" << std::endl;


    int current_k = *data->k_ptr;

    // If the index is higher than k it exits immediately
    if (my_id > current_k) {
        return nullptr;
    }

    // Goes from thread to thread
    if (mode == CLI_MODE_THREAD && my_id < current_k) {
        flags[limit + 1] = true;
    }

    // I'm using the thread log library and statement the same as a print statement
    ThreadLog("[thread %d] started", my_id);

    // Local variable for file data pointer
    const auto& rows = *(data->fdata);

    for (int m = 0; ; m++) {

        int row_ind = my_id * m;

        // Statement that checks if we're out of rows
        if (row_ind >= rows.size()) {
            break;
        }

        char results[65]; // Turns the data into a hexademical 
        // Becoming 64 characters long w/ a null terminator at the end

        ComputeIterativeSha256Hex((uint8_t*)rows[row_ind].c_str(), rows[row_ind].length(), 100000, results);

        ThreadLog("Thread %d processing row %d", my_id, row_ind);
    }

    ThreadLog("Thread %d returned ", my_id);
    return nullptr;

}

// Main function
int main (int argc, char* argv[]) {

    
    CliMode mode ;
    uint32_t timeout_ms;  // 
    CliParse(argc, argv, &mode, &timeout_ms);  // For parsing the command line

    
    // Reads the file data from STDIN in rows
    std::vector<std::string> file_rows;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (!line.empty()) {
            file_rows.push_back(line);
        }
    }

    // The number of cores
    int n = ::get_nprocs();
    int k = 0;

    // Threads pointer created. Took this from class earlier
    std::vector<pthread_t> threads(n + 1);
    bool* released_flags = new bool[n + 1];

    for (int i = 0; i <= num; ++i) {
        released_flags[i] = false;
    }

    for (int i = 1; i <= n; ++i) {
        ThreadsArg* args = new ThreadsArg{i, &k, released_flags, mode, &file_rows};
        pthread_create(&threads[i], nullptr, ThreadRoutine, args);
    }

    // Sets up prompt for K with tty
    std::ifstream tty("/dev/tty");
    std::cout << "Enter the max amount of threads (1-" << n << ") :";
    tty >> k;


    // If statements that release threads based on which mode it is
    if (mode == CLI_MODE_ALL) {

        for (int i = 1; i <= k; ++i) {
            released_flags[i] = true;
        }

    }

    else if (mode == CLI_MODE_RATE){

        for (int i = 1; i <= k; ++i) {
            released_flags[i] = true;
            Timings_SleepMs(1); 
        }
    }

    else if (mode == CLI_MODE_THREAD) {
        released_flags[1] = true;
    }



    // Waits for all of the threads
    for (int i = 1; i <= n; ++i) {
        pthread_join(threads[i], nullptr);
    }

    return 0;
}
