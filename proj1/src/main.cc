// Copyright Jerard Austin 2026


#include <iostream>
#include <pthread.h>  // For my use of threads
#include <vector>
#include <string>
#include <fstream>  // For handling my files

// For the provided libraries
#include <proj1/lib/timings.h> // Used for 
#include <proj1/lib/cli_parser.h>
#include <proj1/lib/thread_log.h>
#include <proj1/lib/sha256.h>


using namespace std;

// Structure for the data of my threads
struct ThreadsArg {
    int index;
    int k;
    bool* released_flags;

    std::string plan;
    const std::vector<std::string>* file_data;

};


void ThreadRoutine (void* arg) {
    // Statement for processing my rows
    return nullptr;
}


int main (int argc, char* argv[]) {

    //
    ThreadsArg* data = static_cast<ThreadsArg*>(arg)

    // Statement that makes 
    while (!data->released_flags[data->index]) {
        Timings_SleepMs(1);
    }

    // If the index is higher than k it exits immediately
    if (data->index > data->k) {
        return nullptr;
    }

    


}
