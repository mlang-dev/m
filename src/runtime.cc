#include <stdio.h>
#include "runtime.h"

double putchard(double x) {
  putchar((char)x);
  return x;
}
