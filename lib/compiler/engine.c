/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * compiler engine connnecting frontend to backend
 */
#include "compiler/engine.h"

const char *_engine_version = "m - 0.0.45";

void engine_free(struct engine *engine)
{
    backend_deinit(engine->be);
    frontend_deinit(engine->fe);
    free(engine);
}

const char *engine_version(void)
{
    return _engine_version;
}
