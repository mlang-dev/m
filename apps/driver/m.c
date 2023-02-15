/*
 * Copyright (C) 2023 Ligang Wang <ligangwangs@gmail.com>
 *
 * mlang driver, command line to run m compiler
 */
#include "clib/array.h"
#include "clib/object.h"
#include "clib/util.h"
#include "clib/string.h"
#include "wasm/mw.h"

#include <string.h>

extern char *optarg;
extern int optind, opterr, optopt;

void print_usage()
{
    printf("Usage as a compiler: m src file -o output file\n");
    printf("Usage as a repl: m\n");
    exit(2);
}

void compile_file(const char *filename)
{
    FILE *fp = fopen(filename, "r");    
    fseek(fp, 0, SEEK_END); 
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    const char *code = malloc(size);
    fread((void*)code, 1, size, fp);
    fclose(fp);
    u8* wasm = compile_code(code);
    char *basename = get_basename((char *)filename);
    char *target_name = strcat(basename, ".wasm");

    FILE *target_fp = fopen(target_name, "wb");
    fwrite(wasm, get_code_size(), 1, target_fp);
    fclose(target_fp);
    
    free((void*)code);
    free(wasm);
}

int main(int argc, char *argv[])
{
    // printf("from location: %s\n", get_exec_path());
    struct array src_files;
    array_init(&src_files, sizeof(char *));
    while (optind < argc) {
        array_push(&src_files, &argv[optind]);
        optind++;
    }
    for (size_t i = 0; i < array_size(&src_files); i++) {
        const char *fn = *(const char **)array_get(&src_files, i);
        if (access(fn, F_OK) == -1) {
            printf("file: %s does not exist\n", fn);
            exit(1);
        }
        compile_file(fn);
    }
    array_deinit(&src_files);
}
