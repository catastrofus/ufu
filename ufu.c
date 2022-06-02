
#include "ufu.h"
#include "ufu.p"

int main(int argc,char **argv) {

  struct s_env *env;

  // Initialize environment.
  env=ufu_alloc_env(argc,argv);

  strcpy(env->fun,"main");
  ufu_wai(env);

  // Initialize character based windows.
  ufu_init_curses(env);

  // Display logo.
  ufu_logo(env);

  // Main.
  ufu_main(env);

  // Deinitialize character based windows.
  ufu_deinit_curses(env);

  // Deinitialize environment.
  ufu_free_env(env);

  return(0);

}

