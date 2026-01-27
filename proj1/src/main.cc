// Copyright Jerard Austin 2026


#include <iostream>
#include <proj1/include/main.h> // Used to 
#include <pthread.h>  // For my use of threads
#include <vector>
#include <proj1/lib/timings.h>
#include <string>
#include <fstream>  // For handling my files

using namespace std;

// Structure for my threads
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



