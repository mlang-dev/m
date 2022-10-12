/*
 * error handling
 * 
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * error handling for mlang compiler, it is not thread safe, but as long as handle is unique per thread,
 * the chance of having race condition is pretty low.
 */

#include "error/error.h"
#include "clib/hashtable.h"
#include "clib/array.h"
#include <assert.h>


const char *err_messages[ALL_ERROR] = {
    "",
    "unrecognized character.",
    "unrecognized token.",

    "missing end quote for char literal.",
    "character literal is found to have more than 1 character.",
    "missing end quote for string literal.",
    "inconsistent indent level found.",

    "The left side of the dot is expected to be a struct type.",
    "field does not exist.",
    "variable type not matched with literal constant.",
    "types do not match.",
    "function is not defined."
};

struct hashtable g_error_reports;

void _free_fun(void *arr)
{
    array_deinit(arr);
}

void error_init()
{
    hashtable_init_with_value_size(&g_error_reports, sizeof(struct array), _free_fun);
}

void error_deinit()
{
    hashtable_deinit(&g_error_reports);
}

struct error_reports get_error_reports(ErrorHandle handle)
{
    struct error_reports reports;
    struct array *arr = hashtable_get_p(&g_error_reports, handle);
    if(!arr||!array_size(arr)){
        reports.num_errors = 0;
        reports.reports = 0;
    }
    reports.num_errors = array_size(arr);
    reports.reports = array_front(arr);
    return reports;
}

struct error_report *get_last_error_report(ErrorHandle handle)
{
    struct array *arr = hashtable_get_p(&g_error_reports, handle);
    if(!arr||!array_size(arr))
        return 0;
    return array_back(arr);
}

void report_error(ErrorHandle handle, enum error_code error_code, struct source_location loc)
{
    struct array *arr = hashtable_get_p(&g_error_reports, handle);
    if(!arr){
        struct array new_array;
        array_init(&new_array, sizeof(struct error_report));
        hashtable_set_p(&g_error_reports, handle, &new_array);
        arr = hashtable_get_p(&g_error_reports, handle);
    }
    assert(arr);
    struct error_report report;
    report.error_code = error_code;
    report.error_msg = err_messages[error_code];
    report.loc = loc;
    array_push(arr, &report);
}
