
#include "ufu.h"
#include "ufu.p"

void ufu_crypt_passwd(struct s_env *env,struct s_buffer *b,struct s_remote *r) {

  int i;
  char c;

  strcpy(env->fun,"ufu_crypt_passwd");
  ufu_wai(env);

  strcpy(b->username,r->username);
  strcpy(b->password,r->password);

  sprintf(env->msg,"Scrambling password.");
  ufu_log(env);

  for(i=0;i<strlen(b->password);i++) {

    c=b->password[i];

    if((i%2)>0) {
      c=c+128-strlen(b->username);
    }
    else {
      c=256-c+strlen(b->username);
    }

    b->password[i]=c;

  }

}

int ufu_com_read_dir(struct s_env *env,char *dirname,int panel) {

  int ok,last,p;
  long int size;
  struct s_entry *e1,*e2;
  struct s_buffer *b;

  strcpy(env->fun,"ufu_com_read_dir");
  ufu_wai(env);

  ok=FALSE;

  return(ok);

}

int ufu_com_read_entry(struct s_env *env,char *dirname,int panel,struct s_entry *e) {

  int ok,p;
  struct s_buffer *b;

  strcpy(env->fun,"ufu_com_read_entry");
  ufu_wai(env);

  ok=FALSE;

  return(ok);

}

int ufu_com_get_txtfile(struct s_env *env,char *dname,char *fname,int panel,int ve) {

  int ok,last,i,must_write;
  char c,*f;
  struct s_buffer *b;
  FILE *fp;

  strcpy(env->fun,"ufu_com_get_txtfile");
  ufu_wai(env);

  ok=FALSE;

  return(ok);

}

int ufu_com_get_version(struct s_env *env,struct s_remote *r) {

  int ok;
  struct s_buffer *b;

  strcpy(env->fun,"ufu_com_get_version");
  ufu_wai(env);

  ok=FALSE;

  return(ok);

}

int ufu_com_get_binfile(struct s_env *env,char *dname,char *fname,int panel,int ve) {

  int ok,last,i;
  char *f;
  struct s_buffer *b;
  struct s_buffer_action *ba;
  FILE *fp;

  strcpy(env->fun,"ufu_com_get_binfile");
  ufu_wai(env);

  ok=FALSE;

  return(ok);

}

int ufu_com_put_binfile(struct s_env *env,char *dname,char *fname,int panel,int ve) {

  int ok,last,i;
  char *f;
  struct s_buffer *b;
  struct s_buffer_action *ba;
  FILE *fp;

  strcpy(env->fun,"ufu_com_put_binfile");
  ufu_wai(env);

  ok=FALSE;

  return(ok);

}

