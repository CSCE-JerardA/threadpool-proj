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
#include <proj1/lib/cli_parser.h> // For parsing the command line
#include <proj1/lib/thread_log.h> // For displaying statements. Used like a print statement
#include <proj1/lib/sha256.h>  // For output and doing the math


using namespace std;


struct Row {

    std::string ids;
    std::string vals;
    std::string h_vals;
    std::size_t iterations;

};



// Structure for the data of my threads
struct ThreadsArg {
    int index;  // The ID
    int* k_ptr;  // The active thread pointer
    bool* released_flags;  // Pointer for flagged array

    CliMode mode;  // For releasing of threads
    const std::vector<std::string>* fdata;  // Pointer to the file data

    // Added the row pointer
    std::vector<Row>* results_table_ptr;

};





// Function for what the pthreads execute
void* ThreadRoutine (void* arg) {
    
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
        flags[my_id + 1] = true;
    }

    // I'm using the thread log library and statement the same as a print statement
    ThreadLog("[thread %d] started", my_id);

    // Local variable for file data pointer
    const auto& rows = *(data->fdata);

    for (int m = 0; ; m++) {

        int row_ind = my_id * m;

        // Statement that checks if we're out of rows
        // The compiler literally made me change this part
        if (row_ind >= (int)rows.size()) {
            break;
        }

        char results[65]; // Turns the data into a hexademical 
        // Becoming 64 characters long w/ a null terminator at the end

        ComputeIterativeSha256Hex((uint8_t*)rows[row_ind].c_str(), rows[row_ind].length(), 100000, results);

        data->results_table_ptr->at(row_ind).ids = to_string(row_ind);
        data->results_table_ptr->at(row_ind).vals = rows[row_ind];
        data->results_table_ptr->at(row_ind).h_vals = string(results);


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

    std::vector<Row> final_table(file_rows.size());

    // The number of cores
    int n = ::get_nprocs();
    int k = 0;

    // Using vector to create the threadpool. Took this from class earlier
    std::vector<pthread_t> threads(n + 1);
    bool* released_flags = new bool[n + 1];

    for (int i = 0; i <= n; ++i) {
        released_flags[i] = false;
    }

    // This for loop creates all threads and make them wait
    for (int i = 1; i <= n; ++i) {
        ThreadsArg* args = new ThreadsArg{i, &k, released_flags, mode, &file_rows, &final_table};
        pthread_create(&threads[i], nullptr, ThreadRoutine, args);
    }

    // Sets up prompt for K with tty
    std::ifstream tty("/dev/tty");

    // Statement for buffering problem displaying the statement
    // Had to look this up on stack overflow
    std::cout << "Enter max threads (1 - " << n << "): " << std::flush;
    tty >> k;
    std::cout << std::endl;


    // If statements that release threads based on which mode it is
    if (mode == CLI_MODE_ALL) {

        for (int i = 1; i <= n; ++i) {
            released_flags[i] = true;
        }

    }

    else if (mode == CLI_MODE_RATE){

        for (int i = 1; i <= n; ++i) {
            released_flags[i] = true;
            if (i > k) {
                Timings_SleepMs(1); 
            }
        }

    }
    else if (mode == CLI_MODE_THREAD) {
        released_flags[1] = true;

        for (int i = k + 1; i <= n; ++i){
            released_flags[i] = true;
        }
    }



    // Waits for all of the threads
    for (int i = 1; i <= n; ++i) {
        pthread_join(threads[i], nullptr);
    }

    // Just to have the same setup for output as the example in the file
    std::cout << "Thread Start Encryption!" << std::endl;

    for (size_t i = 0; i < final_table.size(); ++i) {

        // Only prints the rows that are actually getting processed
        if (!final_table[i].ids.empty()) {

           std::cout << final_table[i].ids << "    "
                 << final_table[i].vals << "    "
                 << final_table[i].h_vals << std::endl;

        }
    }

    delete[] released_flags;

    return 0;
}