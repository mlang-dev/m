/*
 * application data
 * 
 * Copyright (C) 2023 Ligang Wang <ligangwangs@gmail.com>
 *
 * global application data handling.
 */

#include "app/error.h"
#include "app/app.h"
#include "lexer/terminal.h"
#include "clib/util.h"
#include "clib/symbol.h"

struct app app;

void app_init()
{
    symbols_init();
    terminal_init();
    error_init(&app.error_reports);
}

void app_deinit()
{
    error_deinit(&app.error_reports);
    terminal_deinit();
    symbols_deinit();
}

void app_reset_error_reports()
{
    error_deinit(&app.error_reports);
    error_init(&app.error_reports);
}

struct app *app_get()
{
    return &app;
}
