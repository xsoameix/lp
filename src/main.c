#include <stdio.h>
#include "scan.h"

int
main(int argc, char ** argv) {
  return argc == 2 ? exec(argv[1]) : 1;
}
