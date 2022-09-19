/*
 * Copyright (C) 2022 Ligang Wang <ligangwangs@gmail.com>
 *
 * compiler engine connnecting frontend to backend
 */
#include "compiler/engine.h"

void engine_free(struct engine *engine)
{
    backend_deinit(engine->be);
    frontend_deinit(engine->fe);
}
