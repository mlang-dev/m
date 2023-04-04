/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * mlang driver, command line to run as an intepreter or compiler options
 */
#include "clib/array.h"
#include "clib/object.h"
#include "clib/util.h"
#include "clib/string.h"
#include "compiler/compiler.h"
#include "compiler/ld.h"
#include "compiler/engine.h"
#include "compiler/repl.h"
#include "app/app.h"
#include <string.h>
#include <stdlib.h>

extern char *optarg;
extern int optind, opterr, optopt;

void print_usage()
{
    printf("m usage: m -o output file -f ir|bc|ob src file\n");
    exit(2);
}

const char * ld_exe_cmd = "ld.lld -pie -z relro --hash-style=gnu --build-id --eh-frame-hdr -m elf_x86_64 -dynamic-linker /lib64/ld-linux-x86-64.so.2 /lib/x86_64-linux-gnu/Scrt1.o /lib/x86_64-linux-gnu/crti.o /usr/bin/../lib/gcc/x86_64-linux-gnu/11/crtbeginS.o -L/usr/bin/../lib/gcc/x86_64-linux-gnu/11 -L/usr/bin/../lib/gcc/x86_64-linux-gnu/11/../../../../lib64 -L/lib/x86_64-linux-gnu -L/lib/../lib64 -L/usr/lib/x86_64-linux-gnu -L/usr/lib/../lib64 -L/usr/lib/llvm-14/bin/../lib -L/lib -L/usr/lib -lgcc --as-needed -lgcc_s --no-as-needed -lc -lgcc --as-needed -lgcc_s --no-as-needed /usr/bin/../lib/gcc/x86_64-linux-gnu/11/crtendS.o /lib/x86_64-linux-gnu/crtn.o";

int main(int argc, char *argv[])
{
    // printf("from location: %s\n", get_exec_path());
    int c;
    int fflag = 0;
    enum object_file_type file_type = FT_OBJECT;
    struct array src_files;
    array_init(&src_files, sizeof(char *));
    struct array obj_files;
    array_init(&obj_files, sizeof(char *));
    bool is_compiler_front_end = false;
    string link_cmd;
    string_init_chars(&link_cmd, ld_exe_cmd);
    char *output_filepath = 0;
#ifdef __APPLE__
    const char *ld_cmd = "ld64.lld.darwinnew";
    const char *finalization = "-lSystem";
#elif defined(_WIN32)
    const char *finalization = 0;
    char output[PATH_MAX];
    char *output_str = &output[0];
    strcpy_s(output, sizeof(output), "/OUT:");
    const char *ld_cmd = "link.exe";
    const char *entry_main = "/ENTRY:main";
    const char *libstdio = "legacy_stdio_definitions.lib";
    const char *libc = "ucrtd.lib";
#elif defined(__linux__)
#endif
    /**
     * ':' indicating this option has argument value: optarg
     * 
     */
    while ((c = getopt(argc, argv, "cf:o:")) != -1) {
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
            strcat_s(output, sizeof(output), optarg);
            array_push(&ld_options, &output_str);
#else
            string_add_chars(&link_cmd, " -o ");
            string_add_chars(&link_cmd, optarg);
#endif
            output_filepath = optarg;
            break;
        }
        case 'c':{
            is_compiler_front_end = true;
            break;
        }
        case '?':
        default:
            break;
        }
    }
    for(; optind < argc; optind++){     
        array_push_ptr(&src_files, argv[optind]);
    }
    int result = 0;
    if (!array_size(&src_files)) {
        printf("%s\n", engine_version());
        print_usage();
        //result = run_repl();
    } else {
        if (!file_type)
            file_type = FT_OBJECT;
        app_init();
        for (size_t i = 0; i < array_size(&src_files); i++) {
            const char *fn = (const char *)array_get_ptr(&src_files, i);
            if (access(fn, F_OK) == -1) {
                printf("file: %s does not exist\n", fn);
                exit(1);
            }
            printf("compiling %s -> %s\n", fn, output_filepath);
            result = compile(fn, file_type, output_filepath);
            char *basename = get_basename((char *)fn);
            char *obj_name = strcat(basename, ".o");
            string_add_chars(&link_cmd, " ");
            string_add_chars(&link_cmd, obj_name);
        }
        app_deinit();
    }
    // do linker
    if (file_type == FT_OBJECT && !is_compiler_front_end) {
        printf("linking %s\n", string_get(&link_cmd));
        result = system(string_get(&link_cmd));
    }
    array_deinit(&src_files);
    string_deinit(&link_cmd);
    return result;
}
