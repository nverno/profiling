#pragma once

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

// Try to print report to file descriptor REPORT_FILENO if it's available
// Otherwise, print to STDERR_FILENO
#ifndef REPORT_FILENO
#define REPORT_FILENO 100
#endif /* OUTFD */

void profile_begin(void);
void profile_end(void);

typedef struct {
    size_t input_size;          // `-s` option: input size. Defaults to SIZE_MAX
    size_t block_size;          // `-b` option: block size. Defaults to 0
    size_t stride;              // `-t` option: stride. Defaults to 1
    const char* output_file;    // `-o` option: output file. Defaults to NULL
    const char* input_file;     // input file. Defaults to NULL
    int n_input_files;          // number of input files; at least 1
    const char** input_files;   // all input files; NULL-terminated array
} arguments;

arguments parse_arguments(int argc, char* argv[], const char* opts);
