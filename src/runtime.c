/*
 * Copyright (C) 2020 Ligang Wang <ligangwangs@gmail.com>
 *
 * m language runtime
 */
#include <stdio.h>
#include "runtime.h"

double print(double x) {
  printf("%f\n", x);
  return x;
}
