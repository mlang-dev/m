/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m language runtime
 */
#include "runtime.h"
#include <stdio.h>

int print(int x)
{
    printf("%d\n", x);
    return x;
}
