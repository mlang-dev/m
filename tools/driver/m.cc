/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * mlang driver, command line to run as an intepreter or compiler options
 */
#include "clib/array.h"
#include "clib/object.h"
#include "compiler/compiler.h"
#include "compiler/ld.h"
#include "compiler/repl.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

extern char *optarg;
extern int optind, opterr, optopt;

void print_usage()
{
    printf("Usage as a compiler: m -o output file -f ir|bc|ob src file\n");
    printf("Usage as a repl: m\n");
    exit(2);
}

int main(int argc, char *argv[])
{
    // printf("from location: %s\n", get_exec_path());
    int c;
    int fflag = 0;
    enum object_file_type file_type = FT_OBJECT;
    struct array src_files;
    array_init(&src_files, sizeof(char *));
    struct array ld_options;
    array_init(&ld_options, sizeof(char *));
    bool use_ld = false;
#ifdef __APPLE__
    const char *ld_cmd = "ld64.lld.darwinnew";
    const char *finalization = "-lSystem";
    array_push(&ld_options, &ld_cmd);
#elif defined(_WIN32)
    char output[PATH_MAX];
    char *output_str = &output[0];
    strcpy(output, "/OUT:");
    const char *ld_cmd = "link.exe";
    const char *entry_main = "/ENTRY:main";
    const char *libstdio = "legacy_stdio_definitions.lib";
    const char *libc = "ucrtd.lib";
    const char *finalization = NULL;
    array_push(&ld_options, &ld_cmd);
    array_push(&ld_options, &entry_main);
    array_push(&ld_options, &libstdio);
    array_push(&ld_options, &libc);
#elif defined(__linux__)
    const char *ld_cmd = "ld.lld";
    const char *libcpath = "-L/usr/lib/x86_64-linux-gnu";
    const char *libc = "-lc";
    const char *dynamic_link = "-dynamic-linker"; // only elf
    const char *libld = "/lib64/ld-linux-x86-64.so.2";
    const char *start_entry = "/usr/lib/x86_64-linux-gnu/crt1.o";
    const char *initialization = "/usr/lib/x86_64-linux-gnu/crti.o";
    const char *finalization = "/usr/lib/x86_64-linux-gnu/crtn.o";
    array_push(&ld_options, &ld_cmd);
    array_push(&ld_options, &libcpath);
    array_push(&ld_options, &libc);
    array_push(&ld_options, &dynamic_link);
    array_push(&ld_options, &libld);
    array_push(&ld_options, &start_entry);
    array_push(&ld_options, &initialization);
#endif
    while (optind < argc) {
        if ((c = getopt(argc, argv, "f:o:")) != -1) {
            switch (c) {
            case 'f': {
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
            }
            case 'o': {
#ifdef _WIN32
                strcat(output, optarg);
                array_push(&ld_options, &output_str);
#else
                const char *output = "-o";
                array_push(&ld_options, &output);
                array_push(&ld_options, &optarg);
#endif
                use_ld = true;
                break;
            }
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
        printf("m - 0.0.17\n");
        result = run_repl();
    } else {
        if (!file_type)
            file_type = FT_OBJECT;
        for (size_t i = 0; i < array_size(&src_files); i++) {
            const char *fn = *(const char **)array_get(&src_files, i);
            if (access(fn, F_OK) == -1) {
                printf("file: %s does not exist\n", fn);
                exit(1);
            }
            result = compile(fn, file_type);
            char *basename = get_basename((char *)fn);
            char *obj_name = strcat(basename, ".o");
            array_push(&ld_options, &obj_name);
            break;
        }
    }
    // do linker
    if (file_type == FT_OBJECT && use_ld) {
        if (finalization)
            array_push(&ld_options, &finalization);
        int ld_argc = array_size(&ld_options);
        const char **ld_argv = (const char **)array_get(&ld_options, 0);
        result = ld(ld_argc, ld_argv);
    }
    array_deinit(&src_files);
    array_deinit(&ld_options);
    return result;
}
