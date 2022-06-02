
#include "ufud.h"
#include "ufud.p"

int ufud_poll(struct s_env *env,int fd,int inout,char *ipno) {

  int ok,pok,c,time_inout,time_retry;
  struct pollfd pfd;

  ok=FALSE;

  pfd.fd=fd;

  switch(inout) {
    case UFUD_POLLIN:
      pfd.events=POLLIN;
      time_inout=env->timein;
      time_retry=env->timein_retry;
      break;
    case UFUD_POLLOUT:
      pfd.events=POLLOUT;
      time_inout=env->timeout;
      time_retry=env->timeout_retry;
      break;
  }

  c=0;
  while(c<time_retry) {

    pok=poll(&pfd,1,time_inout);
    if(pok<0) {

      sprintf(env->msg,"[%s] Error in polling, quitting!",ipno);
      ufud_log(env);

      close(fd);
      exit(2);

    }
    else {
      if(pok==0) {

        sprintf(env->msg,"[%s] Timeout in polling! (>%dms)",ipno,time_inout);
        ufud_log(env);

        if((c+1)<time_retry) {

          sprintf(env->msg,"[%s] Retrying poll, left: %d",ipno,time_retry-c-1);
          ufud_log(env);

          c++;

        }
        else {

          sprintf(env->msg,"[%s] Timeout in polling, quitting!",ipno);
          ufud_log(env);

          close(fd);
          exit(3);

        }

      }
      else {

        switch(pfd.revents) {

          case POLLIN:
            ok=TRUE;
            c=time_retry;
            break;

          case POLLOUT:
            ok=TRUE;
            c=time_retry;
            break;

          case POLLPRI:
            ok=TRUE;
            c=time_retry;
            break;

          case POLLERR:
            sprintf(env->msg,"[%s] Poll: Error!",ipno);
            ufud_log(env);
            break;

          case POLLHUP:
            sprintf(env->msg,"[%s] Poll: Hang up!",ipno);
            ufud_log(env);
            break;

          case POLLNVAL:
            sprintf(env->msg,"[%s] Poll: Invalid request!",ipno);
            ufud_log(env);
            break;

        }

        if(!ok) {

          sprintf(env->msg,"[%s] Error in polling, quitting!",ipno);
          ufud_log(env);

          close(fd);
          exit(4);

        }

      }
    }
  }

  return(ok);

}

void ufud_errormsg(struct s_env *env,int sr,int errmsg,char *ipno) {

  switch(sr) {

    case UFUD_SEND:

      switch(errmsg) {

        case EACCES:
          sprintf(env->msg,"[%s] [EACCES] No write permission!",ipno);
          break;

        case EAGAIN:
          sprintf(env->msg,"[%s] [EAGAIN] (Non-)blocking socket issue!",ipno);
          break;

        case EBADF:
          sprintf(env->msg,"[%s] [EBADF] Invalid descriptor!",ipno);
          break;

        case ECONNRESET:
          sprintf(env->msg,"[%s] [ECONNRESET] Connection reset by pear!",ipno);
          break;

        case EDESTADDRREQ:
          sprintf(env->msg,"[%s] [EDESTADDRREQ] No connection!",ipno);
          break;

        case EFAULT:
          sprintf(env->msg,"[%s] [EFAULT] Invalid user space address!",ipno);
          break;

        case EINTR:
          sprintf(env->msg,"[%s] [EINTR] Signalled before data transmission!",ipno);
          break;

        case EINVAL:
          sprintf(env->msg,"[%s] [EINVAL] Invalid argument!",ipno);
          break;

        case EMSGSIZE:
          sprintf(env->msg,"[%s] [EMSGSIZE] Invalid size!",ipno);
          break;

        case EISCONN:
          sprintf(env->msg,"[%s] [EISCONN] Already connected!",ipno);
          break;

        case ENOBUFS:
          sprintf(env->msg,"[%s] [ENOBUFS] Output queue is full!",ipno);
          break;

        case ENOMEM:
          sprintf(env->msg,"[%s] [ENOMEM] Out of memory!",ipno);
          break;

        case ENOTCONN:
          sprintf(env->msg,"[%s] [ENOTCONN] Not connected!",ipno);
          break;

        case ENOTSOCK:
          sprintf(env->msg,"[%s] [ENOTSOCK] Argument is not a socket!",ipno);
          break;

        case EOPNOTSUPP:
          sprintf(env->msg,"[%s] [EOPNOTSUPP] Inappropriate flags!",ipno);
          break;

        case EPIPE:
          sprintf(env->msg,"[%s] [EPIPE] End socket closed!",ipno);
          break;

        default:
          sprintf(env->msg,"[%s] Errno %d: Unknow error!",ipno,errmsg);
          break;

      }

      break;

    case UFUD_RECV:

      switch(errmsg) {

        case EAGAIN:
          sprintf(env->msg,"[%s] [EAGAIN] (Non-)blocking socket issue!",ipno);
          break;

        case EBADF:
          sprintf(env->msg,"[%s] [EBADF] Invalid descriptor!",ipno);
          break;

        case ECONNREFUSED:
          sprintf(env->msg,"[%s] [ECONNREFUSED] Connection refused!",ipno);
          break;

        case EFAULT:
          sprintf(env->msg,"[%s] [EFAULT] Invalid user space address!",ipno);
          break;

        case EINTR:
          sprintf(env->msg,"[%s] [EINTR] Receive interupted!",ipno);
          break;

        case EINVAL:
          sprintf(env->msg,"[%s] [EINVAL] Invalid argument!",ipno);
          break;

        case ENOMEM:
          sprintf(env->msg,"[%s] [ENOMEM] Out of memory!",ipno);
          break;

        case ENOTCONN:
          sprintf(env->msg,"[%s] [ENOTCONN] Socket not connected!",ipno);
          break;

        case ENOTSOCK:
          sprintf(env->msg,"[%s] [ENOTSOCK] Argument is not a socket!",ipno);
          break;

        default:
          sprintf(env->msg,"[%s] Errno %d: Unknow error!",ipno,errmsg);
          break;

      }
      break;

  }

  sprintf(env->msg,"%s",env->msg);
  ufud_log(env);

}

int ufud_send(struct s_env *env,struct s_buffer *b,unsigned int fd,char *ipno) {

  int ok,len,e;
  struct pollfpfd;

  ok=FALSE;

  if(ufud_poll(env,fd,UFUD_POLLOUT,ipno)) {

    len=send(fd,b,sizeof(struct s_buffer),0);
    e=errno;
    ok=(len>0);

    if(!ok) {

      ufud_errormsg(env,UFUD_SEND,e,ipno);

    }

  }

  return(ok);

}

int ufud_recv(struct s_env *env,struct s_buffer *b,unsigned int fd,char *ipno) {

  int ok,len,e,flag;
  char *act;

  act=ufud_alloc_string(env,16);

  flag=MSG_WAITALL;
  ok=FALSE;

  if(ufud_poll(env,fd,UFUD_POLLIN,ipno)) {

    len=recv(fd,b,sizeof(struct s_buffer),0);
    e=errno;
    ok=((len==sizeof(struct s_buffer))&&(b->action>=UFUD_B_MIN)&&(b->action<=UFUD_B_MAX));

    if(ok) {

      sprintf(env->msg,"[%s] Received length  : %d",ipno,len);
      ufud_log(env);

      switch(b->action) {
        case UFUD_B_NONE:
          strcpy(act,"none");
          break;
        case UFUD_B_GET_VERSION:
          strcpy(act,"get version");
          break;
        case UFUD_B_READ_DIR:
          strcpy(act,"read directory");
          break;
        case UFUD_B_GET_TXTFILE:
          strcpy(act,"get textfile");
          break;
        case UFUD_B_PUT_TXTFILE:
          strcpy(act,"put textfile");
          break;
        case UFUD_B_GET_BINFILE:
          strcpy(act,"get binary file");
          break;
        case UFUD_B_PUT_BINFILE:
          strcpy(act,"put binary file");
          break;
        case UFUD_B_UNLINK_ENTRY:
          strcpy(act,"unlink entry");
          break;
        default:
          strcpy(act,"unknown");
          break;
      }

      sprintf(env->msg,"[%s] Received action  : %d (%s)",ipno,b->action,act);
      ufud_log(env);

      sprintf(env->msg,"[%s] Received username: %s",ipno,b->username);
      ufud_log(env);

      switch(b->action) {

        case UFUD_B_READ_DIR:
        case UFUD_B_READ_ENTRY:

          sprintf(env->msg,"[%s] Received dirname : %s",ipno,b->dirname);
          ufud_log(env);

          break;

        case UFUD_B_GET_TXTFILE:
        case UFUD_B_PUT_TXTFILE:
        case UFUD_B_GET_BINFILE:
        case UFUD_B_PUT_BINFILE:
        case UFUD_B_UNLINK_ENTRY:

          sprintf(env->msg,"[%s] Received dirname : %s",ipno,b->dirname);
          ufud_log(env);

          sprintf(env->msg,"[%s] Received filename: %s",ipno,b->filename);
          ufud_log(env);

          break;

      }

    }
    else {

      ufud_errormsg(env,UFUD_RECV,e,ipno);

    }

  }

  ufud_free_string(env,act);

  return(ok);

}

int ufud_send_action(struct s_env *env,struct s_buffer_action *b,unsigned int fd,char *ipno) {

  int ok,len,e;
  struct pollfpfd;

  ok=FALSE;

  if(ufud_poll(env,fd,UFUD_POLLOUT,ipno)) {

    len=send(fd,b,sizeof(struct s_buffer_action),0);
    e=errno;
    ok=(len>0);

    if(!ok) {

      ufud_errormsg(env,UFUD_SEND,e,ipno);

    }

  }

  return(ok);

}

int ufud_recv_action(struct s_env *env,struct s_buffer_action *b,unsigned int fd,char *ipno) {

  int ok,len,e,flag;

  flag=MSG_WAITALL;
  ok=FALSE;

  if(ufud_poll(env,fd,UFUD_POLLIN,ipno)) {

    len=recv(fd,b,sizeof(struct s_buffer_action),0);
    e=errno;
    //ok=((len>0)&&(b->action>=UFUD_B_MIN)&&(b->action<=UFUD_B_MAX));
    ok=((len==sizeof(struct s_buffer))&&(b->action>=UFUD_B_MIN)&&(b->action<=UFUD_B_MAX));

    if(ok) {

      sprintf(env->msg,"[%s] Received length  : %d",ipno,len);
      ufud_log(env);

      sprintf(env->msg,"[%s] Received action  : %d",ipno,b->action);
      ufud_log(env);

      sprintf(env->msg,"[%s] Received username: %s",ipno,b->username);
      ufud_log(env);

      switch(b->action) {

        case UFUD_B_READ_DIR:
        case UFUD_B_READ_ENTRY:

          sprintf(env->msg,"[%s] Received dirname : %s",ipno,b->dirname);
          ufud_log(env);

          break;

        case UFUD_B_GET_TXTFILE:
        case UFUD_B_PUT_TXTFILE:
        case UFUD_B_GET_BINFILE:
        case UFUD_B_PUT_BINFILE:
        case UFUD_B_UNLINK_ENTRY:

          sprintf(env->msg,"[%s] Received dirname : %s",ipno,b->dirname);
          ufud_log(env);

          sprintf(env->msg,"[%s] Received filename: %s",ipno,b->filename);
          ufud_log(env);

          break;

      }

    }
    else {

      ufud_errormsg(env,UFUD_RECV,e,ipno);

    }

  }

  return(ok);

}

