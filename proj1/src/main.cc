// Copyright Jerard Austin 2026

#include <unistd.h>
#include <iostream>
#include <pthread.h>  // For my use of threads
#include <vector>
#include <string>
#include <fstream>  // For handling my files

// For the provided libraries
#include <proj1/lib/timings.h> 
#include <proj1/lib/cli_parser.h>
#include <proj1/lib/thread_log.h> // For displaying; used like a print statement
#include <proj1/lib/sha256.h>


using namespace std;

// Structure for the data of my threads
struct ThreadsArg {
    int index;  // The ID
    int k;  // The active threads
    bool* released_flags;  // Pointer for flagged array

    CliMode mode;  // Strategy for release
    const std::vector<std::string>* fdata;  // Pointer to the file data

};

// Function for what the pthreads execute
void ThreadRoutine (void* arg) {
    
    //
    ThreadsArg* data = static_cast<ThreadsArg*>(arg);

    // My local variables for connecting my variables through member access operators (arrows)
    // Used cppreference for this part to understand what I was doing
    int my_id = data->index;
    int limit = data->k;
    bool* flags = data->released_flags;
    CliMode strat = data->mode;
    


    // Waits until released
    while (!flags[limit]) {
        Timings_SleepMs(1);
    }

    // If the index is higher than k it exits immediately
    if (my_id > limit) {
        return nullptr;
    }

    // Goes from thread to thread
    if (strat == CLI_MODE_THREAD && my_id < limit) {
        flags[limit + 1] = true;
    }

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

    CliMode mode ;
    uint32_t timeout_ms;
    CliParse(argc, argv, &mode, &timeout_ms);


    std::vector<std::string> file_rows;
    std::string line;

    while (std::getline(std::cin, line)) {

    }


}
