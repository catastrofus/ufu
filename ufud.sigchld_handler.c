
#include "ufud.h"
#include "ufud.p"

void sigchld_handler(int i) {

  while(wait(NULL) > 0);

}

