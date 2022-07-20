
#include "ufu.h"
#include "ufu.p"

int ufu_com_init(struct s_env *env,struct s_remote *r) {

  int sockfd;  
  struct hostent *he;
  struct sockaddr_in their_addr;

  return(TRUE);

}

int ufu_com_exit(struct s_env *env,struct s_remote *r) {

  return(TRUE);

}

int ufu_poll(struct s_env *env,struct s_remote *r,int inout) {

  int ok,pok,c,time_inout,time_retry;
  struct pollfd pfd;

  ok=FALSE;

  return(ok);

}

int ufu_send(struct s_env *env,struct s_remote *r,struct s_buffer *b) {

  int ok,len,e;

  ok=FALSE;

  return(ok);

}

int ufu_recv(struct s_env *env,struct s_remote *r,struct s_buffer *b) {

  int ok,len,e,flag;

  ok=FALSE;

  return(ok);

}

int ufu_send_action(struct s_env *env,struct s_remote *r,struct s_buffer_action *b) {

  int ok,len,e;

  ok=FALSE;

  return(ok);

}

int ufu_recv_action(struct s_env *env,struct s_remote *r,struct s_buffer_action *b) {

  int ok,len,e,flag;

  ok=FALSE;

  return(ok);

}

void ufu_errormsg(struct s_env *env,int sr,int errmsg) {

}

