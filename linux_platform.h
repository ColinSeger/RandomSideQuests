#pragma once
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "platform.h"

static inline struct Timer profile_start(){
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    struct Timer result = {};
    result.ns = (uint64_t)start.tv_sec * 1000000000ULL + (uint64_t)start.tv_nsec;
    return result;
}

static inline void profile_end(struct Timer start){
    struct timespec end;

    clock_gettime(CLOCK_MONOTONIC, &end);

    uint64_t elapsed_nano = (uint64_t)end.tv_sec * 1000000000ULL + (uint64_t)end.tv_nsec;
    elapsed_nano -= start.ns;
    double elapsed_sec = (double)elapsed_nano / 1e9;

    printf("Elapsed time: %llu nano sec\n", (unsigned long long)elapsed_nano);
    printf("Elapsed time: %f seconds\n", elapsed_sec);
}
