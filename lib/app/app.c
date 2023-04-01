/*
 * application data
 * 
 * Copyright (C) 2023 Ligang Wang <ligangwangs@gmail.com>
 *
 * global application data handling.
 */

#include "app/error.h"
#include "app/app.h"
#include "clib/util.h"

struct app app;

void app_init()
{
    error_init(&app.error_reports);
}

void app_deinit()
{
    error_deinit(&app.error_reports);
}

struct app *app_get()
{
    return &app;
}
