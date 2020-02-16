#include <iostream>
#include "compiler.h"
#include "interpreter.h"

using namespace std;

int main(int argc, const char *argv[]) {
  cout << "m - 0.0.6";
  cout << endl;
  if (argc == 1)
    return run_interactive();
  else
    return compile(argv[1]);
}
