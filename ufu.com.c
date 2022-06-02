
#include "ufu.h"
#include "ufu.p"

int ufu_com_init(struct s_env *env,struct s_remote *r) {

  int sockfd;  
  struct hostent *he;
  struct sockaddr_in their_addr;

  sprintf(env->msg,"Setting up a connection to %s on port %d.",r->hostname,r->port);
  ufu_log(env);

  // get the host info
  if((he=gethostbyname(r->hostname))==NULL) {
    sprintf(env->msg,"Unable to get address for %s!",r->hostname);
    ufu_log(env);
    return(FALSE);
  }

  if((sockfd=socket(PF_INET,SOCK_STREAM,0))==(-1)) {
    sprintf(env->msg,"Unable to open socket to %s on port %d!",r->hostname,r->port);
    ufu_log(env);
    return(FALSE);
  }

  their_addr.sin_family=AF_INET;                 // host byte order 
  their_addr.sin_port=htons(r->port);            // short, network byte order 
  their_addr.sin_addr=*((struct in_addr *)he->h_addr);
  memset(&(their_addr.sin_zero),'\0',8);         // zero the rest of the struct 

  if(connect(sockfd,(struct sockaddr *)&their_addr,sizeof(struct sockaddr))==(-1)) {
    sprintf(env->msg,"Unable to connect to %s on port %d!",r->hostname,r->port);
    ufu_log(env);
    return(FALSE);
  }

  sprintf(env->msg,"Connected to %s on port %d.",r->hostname,r->port);
  ufu_log(env);

  r->sockfd=sockfd;
  sprintf(env->msg,"Connection filedescriptor: %d",r->sockfd);
  ufu_log(env);

  return(TRUE);

}

int ufu_com_exit(struct s_env *env,struct s_remote *r) {

  if(r->sockfd>0) {

    close(r->sockfd);
    r->sockfd=0;

    sprintf(env->msg,"Disconnected from %s on port %d.",r->hostname,r->port);
    ufu_log(env);

  }

  return(TRUE);

}

int ufu_poll(struct s_env *env,struct s_remote *r,int inout) {

  int ok,pok,c,time_inout,time_retry;
  struct pollfd pfd;

  ok=FALSE;

  pfd.fd=r->sockfd;

  switch(inout) {
    case UFU_POLLIN:
      pfd.events=POLLIN;
      time_inout=env->timein;
      time_retry=env->timein_retry;
      break;
    case UFU_POLLOUT:
      pfd.events=POLLOUT;
      time_inout=env->timeout;
      time_retry=env->timeout_retry;
      break;
  }

  c=0;
  while(c<time_retry) {

    pok=poll(&pfd,1,time_inout);
    if(pok<0) {

      sprintf(env->msg,"Error in polling!");
      ufu_log(env);

      ufu_com_exit(env,r);

    }
    else {
      if(pok==0) {

        sprintf(env->msg,"Timeout in polling! (>%d ms)",time_inout);
        ufu_log(env);

        if((c+1)<time_retry) {

          sprintf(env->msg,"Retrying poll, left: %d",time_retry-c);
          ufu_log(env);

          c++;

        }
        else {

          sprintf(env->msg,"Used all the polling retries! (%d)",time_retry);
          ufu_log(env);

          ufu_com_exit(env,r);

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
            sprintf(env->msg,"Poll: Error!");
            ufu_log(env);
            break;

          case POLLHUP:
            sprintf(env->msg,"Poll: Hang up!");
            ufu_log(env);
            break;

          case POLLNVAL:
            sprintf(env->msg,"Poll: Invalid request!");
            ufu_log(env);
            break;

        }

        if(!ok) {
          ufu_com_exit(env,r);
        }

      }
    }
  }

  return(ok);

}

int ufu_send(struct s_env *env,struct s_remote *r,struct s_buffer *b) {

  int ok,len,e;

  ok=FALSE;

  if(ufu_poll(env,r,UFU_POLLOUT)) {

    len=send(r->sockfd,b,sizeof(struct s_buffer),0);
    e=errno;
    ok=(len>0);

    if(!ok) {

      ufu_errormsg(env,UFU_SEND,e);

    }

  }

  return(ok);

}

int ufu_recv(struct s_env *env,struct s_remote *r,struct s_buffer *b) {

  int ok,len,e,flag;

  // This flag tells us that 'recv' will wait until all data is received
  // from daemon. Without this flag there will be retries and this will
  // give us empty entries...
  flag=MSG_WAITALL;

  ok=FALSE;

  if(ufu_poll(env,r,UFU_POLLIN)) {

    len=recv(r->sockfd,b,sizeof(struct s_buffer),flag);
    e=errno;
    ok=(len>0);

    if(!ok) {

      ufu_errormsg(env,UFU_RECV,e);

    }

  }

  return(ok);

}

int ufu_send_action(struct s_env *env,struct s_remote *r,struct s_buffer_action *b) {

  int ok,len,e;

  ok=FALSE;

  if(ufu_poll(env,r,UFU_POLLOUT)) {

    len=send(r->sockfd,b,sizeof(struct s_buffer_action),0);
    e=errno;
    ok=(len>0);

    if(!ok) {

      ufu_errormsg(env,UFU_SEND,e);

    }

  }

  return(ok);

}

int ufu_recv_action(struct s_env *env,struct s_remote *r,struct s_buffer_action *b) {

  int ok,len,e,flag;

  // This flag tells us that 'recv' will wait until all data is received
  // from daemon. Without this flag there will be retries and this will
  // give us empty entries...
  flag=MSG_WAITALL;

  ok=FALSE;

  if(ufu_poll(env,r,UFU_POLLIN)) {

    len=recv(r->sockfd,b,sizeof(struct s_buffer_action),flag);
    e=errno;
    ok=(len>0);

    if(!ok) {

      ufu_errormsg(env,UFU_RECV,e);

    }

  }

  return(ok);

}

void ufu_errormsg(struct s_env *env,int sr,int errmsg) {

  switch(sr) {

    case UFU_SEND:

      switch(errmsg) {

        case EACCES:
          sprintf(env->msg,"[EACCES] No write permission!");
          break;

        case EAGAIN:
          sprintf(env->msg,"[EAGAIN] (Non-)blocking socket issue!");
          break;

        case EBADF:
          sprintf(env->msg,"[EBADF] Invalid descriptor!");
          break;

        case ECONNRESET:
          sprintf(env->msg,"[ECONNRESET] Connection reset by pear!");
          break;

        case EDESTADDRREQ:
          sprintf(env->msg,"[EDESTADDRREQ] No connection!");
          break;

        case EFAULT:
          sprintf(env->msg,"[EFAULT] Invalid user space address!");
          break;

        case EINTR:
          sprintf(env->msg,"[EINTR] Signalled before data transmission!");
          break;

        case EINVAL:
          sprintf(env->msg,"[EINVAL] Invalid argument!");
          break;

        case EMSGSIZE:
          sprintf(env->msg,"[EMSGSIZE] Invalid size!");
          break;

        case EISCONN:
          sprintf(env->msg,"[EISCONN] Already connected!");
          break;

        case ENOBUFS:
          sprintf(env->msg,"[ENOBUFS] Output queue is full!");
          break;

        case ENOMEM:
          sprintf(env->msg,"[ENOMEM] Out of memory!");
          break;

        case ENOTCONN:
          sprintf(env->msg,"[ENOTCONN] Not connected!");
          break;

        case ENOTSOCK:
          sprintf(env->msg,"[ENOTSOCK] Argument is not a socket!");
          break;

        case EOPNOTSUPP:
          sprintf(env->msg,"[EOPNOTSUPP] Inappropriate flags!");
          break;

        case EPIPE:
          sprintf(env->msg,"[EPIPE] End socket closed!");
          break;

        default:
          sprintf(env->msg,"Errno %d: Unknow error!",errmsg);
          break;

      }

      break;

    case UFU_RECV:

      switch(errmsg) {

        case EAGAIN:
          sprintf(env->msg,"[EAGAIN] (Non-)blocking socket issue!");
          break;

        case EBADF:
          sprintf(env->msg,"[EBADF] Invalid descriptor!");
          break;

        case ECONNREFUSED:
          sprintf(env->msg,"[ECONNREFUSED] Connection refused!");
          break;

        case EFAULT:
          sprintf(env->msg,"[EFAULT] Invalid user space address!");
          break;

        case EINTR:
          sprintf(env->msg,"[EINTR] Receive interupted!");
          break;

        case EINVAL:
          sprintf(env->msg,"[EINVAL] Invalid argument!");
          break;

        case ENOMEM:
          sprintf(env->msg,"[ENOMEM] Out of memory!");
          break;

        case ENOTCONN:
          sprintf(env->msg,"[ENOTCONN] Socket not connected!");
          break;

        case ENOTSOCK:
          sprintf(env->msg,"[ENOTSOCK] Argument is not a socket!");
          break;

        default:
          sprintf(env->msg,"Errno %d: Unknow error!",errmsg);
          break;

      }
      break;

  }

  sprintf(env->msg,"%s",env->msg);
  ufu_log(env);

}

