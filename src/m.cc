/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * mlang driver, command line to run as an intepreter or compiler options
 */
#include "compiler.h"
#include "repl.h"
#include <unistd.h>

using namespace std;

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
    printf("m - 0.0.13\n");
    int option;
    int fflag = 0;
    char* fopt = 0;
    object_file_type file_type = FT_OBJECT;
    vector<char*> src_files;
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
                fopt = optarg;
                fflag = 1;
                break;
            case '?':
            default:
                break;
            }
        } else {
            src_files.push_back(argv[optind++]);
        }
    }
    if (src_files.empty())
        return run_repl();
    else {
        if (!file_type)
            file_type = FT_OBJECT;
        for (auto src_file : src_files)
            return compile(src_file, file_type);
    }
}
