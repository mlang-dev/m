/*
 * source_location.h
 * 
 * Copyright (C) 2021 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file to define error handling routines
 */

#ifndef __MLANG_ERROR_H__
#define __MLANG_ERROR_H__

#include "lexer/source_location.h"
#include "clib/typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

enum error_code {
    EC_SUCCESS = 0,

    EC_UNRECOGNIZED_CHAR,
    EC_UNRECOGNIZED_TOKEN,
    
    EC_CHAR_MISS_END_QUOTE,
    EC_CHAR_LEN_TOO_LONG,
    EC_STR_MISS_END_QUOTE,
    EC_INCONSISTENT_INDENT_LEVEL,

    ALL_ERROR
};

struct error_report {
    enum error_code error_code;
    const char *error_msg;
    struct source_location loc;
};


struct error_reports {
    u32 num_errors;
    struct error_reports *reports;
};

typedef void *ErrorHandle;

void error_init();
void error_deinit();
struct error_reports get_error_reports(ErrorHandle handle);
struct error_report *get_last_error_report(ErrorHandle handle);
void report_error(ErrorHandle handle, enum error_code error_code, struct source_location loc);

#ifdef __cplusplus
}
#endif

#endif