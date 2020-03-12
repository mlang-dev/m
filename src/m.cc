#include <iostream>
#include "compiler.h"
#include "repl.h"

using namespace std;

int main(int argc, const char *argv[]) {
  cout << "m - 0.0.11";
  cout << endl;
  if (argc == 1)
    return run_repl();
  else
    return compile(argv[1]);
}
