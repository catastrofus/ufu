
#include "ufud.h"
#include "ufud.p"

int ufud_get_version(struct s_env *env,struct s_buffer *b,int fd,char *ipno) {

  switch(env->os) {
    case UFUD_OS_UNKNOWN:
      sprintf(b->errmsg,"%s (Unknown)",UFUD_VERSION);
      break;
    case UFUD_OS_LINUX:
      sprintf(b->errmsg,"%s (Linux)",UFUD_VERSION);
      break;
    case UFUD_OS_HPUX:
      sprintf(b->errmsg,"%s (HPUX)",UFUD_VERSION);
      break;
  }

  b->last=TRUE;
  b->error=FALSE;

  ufud_send(env,b,fd,ipno);

  sprintf(env->msg,"[%s] Send data \"%s\".",ipno,UFUD_VERSION);
  ufud_log(env);

  return(TRUE);

}
