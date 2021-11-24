/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * mlang driver, command line to run as an intepreter or compiler options
 */
#include "clib/array.h"
#include "clib/object.h"
#include "clib/symbol.h"
#include "clib/util.h"
#include <limits.h>
#include <stdio.h>

#include "parser/astdump.h"
#include "tool/cmodule.h"

extern char *optarg;
extern int optind, opterr, optopt;

int main(int argc, char *argv[])
{
    int option;
    printf("c2m - 0.0.16\n");
    symbols_init();
    struct array src_files;
    array_init(&src_files, sizeof(char *));
    char *input_folder = 0;
    char *output_folder = 0;
    while (optind < argc) {
        if ((option = getopt(argc, argv, "i:o:")) != -1) {
            switch (option) {
            case 'i':
                input_folder = optarg;
                break;
            case 'o':
                output_folder = optarg;
                break;
            }
        } else {
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
            const char *fn = *(const char **)array_get(&src_files, i);
            join_path(file_path, sizeof(file_path), input_folder, fn);
            if (access(file_path, F_OK) == -1) {
                printf("file: %s does not exist\n", file_path);
                exit(1);
            }
            string dest_file;
            string src_file;
            string_init_chars(&src_file, fn);
            string_copy(&dest_file, string_substr(&src_file, '.'));
            string_add_chars(&dest_file, ".m");
            if (output_folder)
                join_path(dest_path, sizeof(dest_path), output_folder, string_get(&dest_file));
            else
                join_path(dest_path, sizeof(dest_path), cwd, string_get(&dest_file));
            printf("transpile from %s to %s\n", file_path, dest_path);
            transpile_2_m(file_path, dest_path);
        }
    }
    array_deinit(&src_files);
    symbols_deinit();
    return result;
}
