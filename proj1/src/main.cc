// Copyright Jerard Austin 2026

#include <unistd.h>
#include <iostream>
#include <pthread.h>  // For my use of threads
#include <vector>
#include <string>
#include <fstream>  // For handling my files

// For the provided libraries
#include <proj1/lib/timings.h> // For getting my threads to wait
#include <proj1/lib/cli_parser.h> // 
#include <proj1/lib/thread_log.h> // For displaying; used like a print statement
#include <proj1/lib/sha256.h>  // For output and doing the math


using namespace std;

// Structure for the data of my threads
struct ThreadsArg {
    int index;  // The ID
    int k;  // The active threads
    bool* released_flags;  // Pointer for flagged array

    CliMode mode;  // For releasing of threads
    const std::vector<std::string>* fdata;  // Pointer to the file data

};


struct Row {

    std::string ids;
    std::string values;
    std::size_t iterations;
};

// Function for what the pthreads execute
void ThreadRoutine (void* arg) {
    
    // Works as a pointer the the variables within my struct
    ThreadsArg* data = static_cast<ThreadsArg*>(arg);

    // My local variables for connecting my variables through member access operators (arrows)
    // Used cppreference for this part to understand what I was doing
    int my_id = data->index;
    int limit = data->k;
    bool* flags = data->released_flags;
    CliMode mode = data->mode;
    


    // Make threads wait until released
    while (!flags[limit]) {
        Timings_SleepMs(1);
    }

    std::cout << my_id << "'s Starting Routine" << std::endl;

    // If the index is higher than k it exits immediately
    if (my_id > limit) {
        return nullptr;
    }

    // Goes from thread to thread
    if (mode == CLI_MODE_THREAD && my_id < limit) {
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

        ComputeIterativeSha256Hex(uint8_t*)rows[row_ind].c_str(), rows[row_ind].length(), 100000, results;

        ThreadLog("Thread %d processing row %d", my_id, row_ind);
    }

    ThreadLog("Thread %d returned ", my_id);
    return nullptr;

}

// Main function
int main (int argc, char* argv[]) {

    // My variables
    CliMode mode ;
    uint32_t timeout_ms;
    CliParse(argc, argv, &mode, &timeout_ms);

    
    std::vector<std::string> file_rows;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (!line.empty()) {
            file_rows.push_back(line);
        }
    }

    // The number of cores
    int ncores = sysconf(_SC_NPROCESSORS_ONLN);

    // Thread pointer created
    pthread_t* threads = new pthread_t[n + 1];
    bool* released_flags = new bool[n + 1];

    for (int i = 0; i <= n; ++i) {
        released_flags[i] = false;
    }

    for (int i = 1; i <= n; ++i) {
        ThreadsArg* args = new ThreadsArg{i, 0, released_flags, mode, &file_rows};
        pthread_create(&threads[i], nullptr, ThreadRoutine, args);
    }

    // Sets up prompt for K with tty
    std::ifstream tty("/dev/tty");

    int k;

    std::cout << "Enter the max amount of threads 1-" << n << " :";
    tty >> k;


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



    // 
    for (int i = 1; i <= n; ++i) {
        pthread_join(threads[i], nullptr);
    }

    return 0;
}
