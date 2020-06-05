/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * mlang driver, command line to run as an intepreter or compiler options
 */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

#include "clib/array.h"
#include "clib/object.h"
#include "clib/util.h"
#include "cmodule.h"
#include "astdump.h"

extern char* optarg;
extern int optind, opterr, optopt;

int main(int argc, char* argv[])
{
    int option;
    printf("c2m - 0.0.16\n");
    struct array src_files;
    array_init(&src_files, sizeof(char*));
    char* head_folder = 0;
    while (optind < argc) {
        if ((option = getopt(argc, argv, "i:")) != -1) {
            switch (option) {
            case 'i':
                head_folder = optarg;
                break;
            }
        }else{
            array_push(&src_files, &argv[optind]);
            optind++;
        }
    }
    int result = 0;
    char file_path[PATH_MAX];
    char cwd[PATH_MAX];
    char dest_path[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    if (array_size(&src_files)) {
        for (size_t i = 0; i < array_size(&src_files); i++) {
            const char* fn = *(const char**)array_get(&src_files, i);
            join_path(file_path, head_folder, fn);
            if (access(file_path, F_OK) == -1) {
                printf("file: %s does not exist\n", file_path);
                exit(1);
            }
            string dest_file;
            string src_file;
            string_init_chars(&src_file, fn);
            string_copy(&dest_file, string_substr(&src_file, '.'));
            string_add_chars(&dest_file, ".m");
            join_path(dest_path, cwd, string_get(&dest_file));
            printf("transpile from %s to %s\n", file_path, dest_path);
            transpile_2_m(file_path, dest_path);
        }
    }
    array_deinit(&src_files);
    return result;
}
