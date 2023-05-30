#include "profile.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

// cs61

static struct timeval tv_begin;

void profile_begin(void) {
    int r = gettimeofday(&tv_begin, 0);
    assert(r >= 0);
}

void profile_end(void) {
    struct timeval tv_end;
    struct rusage usage, cusage;

    int r = gettimeofday(&tv_end, 0);
    assert(r >= 0);
    r = getrusage(RUSAGE_SELF, &usage);
    assert(r >= 0);
    r = getrusage(RUSAGE_CHILDREN, &cusage);
    assert(r >= 0);

    timersub(&tv_end, &tv_begin, &tv_end);
    timeradd(&usage.ru_utime, &cusage.ru_utime, &usage.ru_utime);
    timeradd(&usage.ru_stime, &cusage.ru_stime, &usage.ru_stime);

    char buf[1000];
    int len = sprintf(buf, "{\"time\":%ld.%06ld, \"utime\":%ld.%06ld, \"stime\":%ld.%06ld, \"maxrss\":%ld}\n",
                      tv_end.tv_sec, (long) tv_end.tv_usec,
                      usage.ru_utime.tv_sec, (long) usage.ru_utime.tv_usec,
                      usage.ru_stime.tv_sec, (long) usage.ru_stime.tv_usec,
                      usage.ru_maxrss + cusage.ru_maxrss);

    // Print the report to file descriptor REPORT_FILENO if it's available.
    off_t off = lseek(REPORT_FILENO, 0, SEEK_CUR);
    int fd = (off != (off_t) -1 || errno == ESPIPE ? REPORT_FILENO : STDERR_FILENO);
    if (fd == STDERR_FILENO) {
        fflush(stderr);
    }
    ssize_t nwritten = write(fd, buf, len);
    assert(nwritten == len);
}


arguments parse_arguments(int argc, char* argv[], const char* opts) {
    arguments args;
    args.input_size = -1;
    args.block_size = 0;
    args.stride = 1024;
    args.output_file = args.input_file = NULL;
    args.input_files = NULL;

    int arg;
    char* endptr;
    while ((arg = getopt(argc, argv, opts)) != -1) {
        switch (arg) {
        case 's':
            args.input_size = (size_t) strtoul(optarg, &endptr, 0);
            if (endptr == optarg || *endptr) {
                goto usage;
            }
            break;
        case 'b':
            args.block_size = (size_t) strtoul(optarg, &endptr, 0);
            if (args.block_size == 0 || endptr == optarg || *endptr) {
                goto usage;
            }
            break;
        case 't':
            args.stride = (size_t) strtoul(optarg, &endptr, 0);
            if (args.stride == 0 || endptr == optarg || *endptr) {
                goto usage;
            }
            break;
        case 'r': {
            unsigned long seed = strtoul(optarg, &endptr, 0);
            if (endptr == optarg || *endptr) {
                goto usage;
            }
            srandom(seed);
            break;
        }
        case 'o':
            args.output_file = optarg;
            break;
        case '#':
            break;
        default:
            goto usage;
        }
    }

    if (optind + 1 < argc && !strchr(opts, '#')) {
        goto usage;
    }
    args.input_files = (const char**) &argv[optind];
    if (optind < argc) {
        args.input_file = argv[optind];
        args.n_input_files = argc - optind;
    } else {
        args.n_input_files = 1;
    }
    return args;

 usage:
    fprintf(stderr, "Usage: %s", argv[0]);
    if (strchr(opts, 's')) {
        fprintf(stderr, " [-s SIZE]");
    }
    if (strchr(opts, 'b')) {
        fprintf(stderr, " [-b BLOCKSIZE]");
    }
    if (strchr(opts, 't')) {
        fprintf(stderr, " [-t STRIDE]");
    }
    if (strchr(opts, 'o')) {
        fprintf(stderr, " [-o OUTFILE]");
    }
    if (strchr(opts, '#')) {
        fprintf(stderr, " [FILE...]\n");
    } else {
        fprintf(stderr, " [FILE]\n");
    }
    exit(1);
}
