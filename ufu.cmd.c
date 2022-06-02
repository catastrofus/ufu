
#include "ufu.h"
#include "ufu.p"

void ufu_cmd_view(struct s_env *env,char *dname,char *fname) {

  char *s;

  strcpy(env->fun,"ufu_cmd_view");
  ufu_wai(env);

  if(strlen(dname)>0) {

    s=ufu_alloc_string(env,strlen(env->viewer)+strlen(dname)+2);
    strcpy(s,env->viewer);
    strcat(s," ");
    strcat(s,dname);
    if(fname!=NULL) {
      strcat(s,"/");
      strcat(s,fname);
    }

    sprintf(env->msg,"View file using command \"%s\".",s);
    ufu_log(env);

    ufu_deinit_curses(env);
    system(s);
    ufu_init_curses(env);

    ufu_free_string(env,s);

  }

}

void ufu_cmd_edit(struct s_env *env,char *dname,char *fname) {

  char *s;

  strcpy(env->fun,"ufu_cmd_edit");
  ufu_wai(env);

  if(strlen(dname)>0) {

    s=ufu_alloc_string(env,strlen(env->viewer)+strlen(dname)+2);
    strcpy(s,env->editor);
    strcat(s," ");
    strcat(s,dname);
    if(fname!=NULL) {
      strcat(s,"/");
      strcat(s,fname);
    }

    sprintf(env->msg,"Edit file using command \"%s\".",s);
    ufu_log(env);

    ufu_deinit_curses(env);
    system(s);
    ufu_init_curses(env);

    ufu_free_string(env,s);

  }

}

void ufu_cmd_exec(struct s_env *env,int panel,char *cmd) {

  int c;
  char *s,*t;
  FILE *fp;

  strcpy(env->fun,"ufu_cmd_exec");
  ufu_wai(env);

  s=ufu_alloc_string(env,80);
  //strcpy(s,"read -n 1 -s -r -p \"Press any key to continue\"");
  strcpy(s,"read -n 1 -s -r");

  ufu_exec_ext(env,panel,cmd,NULL,NULL);

  ufu_free_string(env,s);

}

void ufu_cmd_exec_main(struct s_env *env,int panel,char key) {

  int found;
  char *s;
  struct s_ucmd *uc;

  strcpy(env->fun,"ufu_exec_main");
  ufu_wai(env);

  s=ufu_alloc_string(env,512);
  found=FALSE;
  uc=env->ucfirst;
  while((uc!=NULL)&&(!found)) {
    if(uc->key==key) {
      found=TRUE;
      sprintf(s,"Key: %c",key);
      ufu_cmd_exec(env,panel,uc->exec);
    }
    uc=uc->next;
  }
  ufu_free_string(env,s);

}

