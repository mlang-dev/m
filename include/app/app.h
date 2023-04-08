/*
 * app.h
 * 
 * Copyright (C) 2023 Ligang Wang <ligangwangs@gmail.com>
 *
 * header file to define global application data
 */
 
#ifndef __MLANG_APP_H__
#define __MLANG_APP_H__

#include "clib/hashtable.h"
#include "clib/typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

struct app{
    struct hashtable error_reports;
};

struct app *app_get();
void app_init();
void app_deinit();
void app_reset_error_reports();

#ifdef __cplusplus
}
#endif

#endif
