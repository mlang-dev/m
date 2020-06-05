/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * mlang driver, command line to run as an intepreter or compiler options
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "clib/array.h"
#include "clib/object.h"
#include "compiler.h"
#include "repl.h"

extern char* optarg;
extern int optind, opterr, optopt;

void print_usage()
{
    printf("Usage as a compiler: m -f ir|bc|ob src file\n");
    printf("Usage as a repl: m\n");
    exit(2);
}

int main(int argc, char* argv[])
{
    printf("m - 0.0.16\n");
    int option;
    int fflag = 0;
    enum object_file_type file_type = FT_OBJECT;
    struct array src_files;
    array_init(&src_files, sizeof(char*));
    while (optind < argc) {
        if ((option = getopt(argc, argv, "f:")) != -1) {
            switch (option) {
            case 'f':
                if (strcmp(optarg, "bc") == 0)
                    file_type = FT_BITCODE;
                else if (strcmp(optarg, "ob") == 0)
                    file_type = FT_OBJECT;
                else if (strcmp(optarg, "ir") == 0)
                    file_type = FT_IR;
                if (fflag || !file_type) {
                    print_usage();
                }
                fflag = 1;
                break;
            case '?':
            default:
                break;
            }
        } else {
            array_push(&src_files, &argv[optind]);
            optind++;
        }
    }
    int result = 0;
    if (!array_size(&src_files)) {
        result = run_repl();
    } else {
        if (!file_type)
            file_type = FT_OBJECT;
        for (size_t i = 0; i < array_size(&src_files); i++) {
            const char* fn = *(const char**)array_get(&src_files, i);
            if (access(fn, F_OK) == -1) {
                printf("file: %s does not exist\n", fn);
                exit(1);
            }

            result = compile(fn, file_type);
            break;
        }
    }
    array_deinit(&src_files);
    return result;
}
