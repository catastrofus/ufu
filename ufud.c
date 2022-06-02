
#include "ufud.h"
#include "ufud.p"

// int main(int argc,char **argv) {
int main(void) {

  int result,ok_uid,ok_gid;
  char *ipno,*ipname,*s;
  // listen on sock_fd, new connection on fd
  unsigned int sockfd,fd,sin_size,yes;
  struct sockaddr_in my_addr,their_addr;
  struct hostent* hp;
  struct sigaction sa;
  struct s_env *env;
  struct s_buffer *b;
  pid_t pid,ppid;
  // struct utsname u;

  yes=1;

  env=ufud_alloc_env();
  ufud_read_config(env);

  ipno=ufud_alloc_string(env,UFUD_LEN_IPNO);
  ipname=ufud_alloc_string(env,UFUD_LEN_IPNAME);

  sprintf(env->msg,"%s",UFUD_BUILDDATE);
  ufud_log(env);

  if((sockfd=socket(AF_INET,SOCK_STREAM,0))==(-1)) {
    sprintf(env->msg,"Unable to open socket!");
    ufud_log(env);
    exit(1);
  }

  sprintf(env->msg,"Socket opened.");
  ufud_log(env);

  if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==(-1)) {
    sprintf(env->msg,"Unable to set socket options!");
    ufud_log(env);
    exit(1);
  }

  sprintf(env->msg,"Socket options set.");
  ufud_log(env);

  my_addr.sin_family=AF_INET;          // host byte order
  my_addr.sin_port=htons(env->port);   // short, network byte order
  my_addr.sin_addr.s_addr=INADDR_ANY;  // automatically fill with my IP
  memset(&(my_addr.sin_zero),'\0',8);  // zero the rest of the struct

  if(bind(sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))==(-1)) {
    sprintf(env->msg,"Unable to bind to port %d!",env->port);
    ufud_log(env);
    exit(1);
  }

  sprintf(env->msg,"Binded to port %d.",env->port);
  ufud_log(env);

  if (listen(sockfd,env->backlog)==(-1)) {
    sprintf(env->msg,"Unable to listen to socket!");
    ufud_log(env);
    exit(1);
  }

  sprintf(env->msg,"Backlog size is %d requests.",env->backlog);
  ufud_log(env);
  sprintf(env->msg,"Listening to socket.");
  ufud_log(env);

  sa.sa_handler=sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags=SA_RESTART;
  if (sigaction(SIGCHLD,&sa,NULL)==(-1)) {
    sprintf(env->msg,"Unable to set signal functions!");
    ufud_log(env);
    exit(1);
  }

  sprintf(env->msg,"Signal functions set.");
  ufud_log(env);

  sprintf(env->msg,UFUD_VERSION);
  ufud_log(env);

  sprintf(env->msg,UFUD_BUILDDATE);
  ufud_log(env);

  sprintf(env->msg,"Server up and running.");
  ufud_log(env);

  s=ufud_alloc_string(env,strlen(UFUD_VERSION));
  strcpy(s,UFUD_VERSION);
  ufud_lower(env,s);
  printf("%s\n",s);
  ufud_free_string(env,s);

  s=ufud_alloc_string(env,strlen(UFUD_BUILDDATE));
  strcpy(s,UFUD_BUILDDATE);
  ufud_lower(env,s);
  printf("%s\n",s);
  ufud_free_string(env,s);

  printf("server up and running.\n");

  while(TRUE) {  // main accept() loop

    sin_size=sizeof(struct sockaddr_in);

    if((fd=accept(sockfd,(struct sockaddr *)&their_addr,&sin_size))<0) {

      sprintf(env->msg,"Unable to accept connection!");
      ufud_log(env);
      sprintf(env->msg,"Resuming application.");
      ufud_log(env);

    }
    else {

      sprintf(env->msg,"[accept] Creating structure for client.");
      ufud_log(env);
      hp=gethostbyaddr((char*)&(their_addr.sin_addr),sizeof(their_addr),AF_INET);

      sprintf(env->msg,"[accept] Converting structure to numbers-and-dots notation.");
      ufud_log(env);
      strcpy(ipno,inet_ntoa(their_addr.sin_addr));

      sprintf(env->msg,"[accept] Converting structure to hostname.");
      ufud_log(env);
      // if(strlen(hp->h_name)>0) {
      if(hp->h_name!=NULL) {
        strcpy(ipname,hp->h_name);
        while(*(hp->h_aliases)){
          sprintf(env->msg,"[accept]   Alias: %s",*(hp->h_aliases)++);
          ufud_log(env);
        }
      }
      else {
        strcpy(ipname,"NoRev:");
        strcat(ipname,ipno);
      }

      sprintf(env->msg,"[accept] Connection from %s (%s).",ipno,ipname);
      ufud_log(env);

      if(!fork()) { // this is the child process

        close(sockfd); // child doesn't need the listener

        pid=getpid();
        ppid=getppid();

        env->ppid=ppid;
        env->pid=pid;

        sprintf(env->msg,"[%s] Forking new pid %d from parent %d.",ipno,pid,ppid);
        ufud_log(env);

        if((!env->auth_ip)||(ufud_auth_ip(env,ipno))) {

          sprintf(env->msg,"[%s] %s allowed!",ipno,ipname);
          ufud_log(env);

          b=ufud_alloc_buffer(env,ipno);

          result=ufud_recv(env,b,fd,ipno);

          if(result) {

            // Password check.
            if(ufud_check_user(env,b,ipno)) {

              ok_uid=ufud_setuid(env,env->uid,ipno);
              ok_gid=TRUE;
              //ok_gid=ufud_setgid(env,env->gid,ipno);

              if((ok_uid)&&(ok_gid)) {

                sprintf(env->msg,"[%s] Switched to user %s!",ipno,ufud_search_user(env,env->uid,ipno));
                ufud_log(env);

                setegid(env->gid);

                switch(b->action) {

                  case UFUD_B_GET_VERSION:
                    // Read directory.
                    ufud_get_version(env,b,fd,ipno);
                    break;

                  case UFUD_B_READ_DIR:
                    // Read directory.
                    ufud_read_entries(env,b,fd,ipno);
                    break;

                  case UFUD_B_READ_ENTRY:
                    // Read directory.
                    ufud_read_entry(env,b,fd,ipno);
                    break;

                  case UFUD_B_GET_TXTFILE:
                    // Get textfile.
                    ufud_get_txtfile(env,b,fd,ipno);
                    break;

                  case UFUD_B_PUT_TXTFILE:
                    // Get textfile.
                    ufud_put_txtfile(env,b,fd,ipno);
                    break;

                  case UFUD_B_UNLINK_ENTRY:
                    // Unlink entry.
                    ufud_unlink_entry(env,b,fd,ipno);
                    break;

                  case UFUD_B_GET_BINFILE:
                    // Get textfile.
                    ufud_get_binfile(env,b,fd,ipno);
                    break;

                  case UFUD_B_PUT_BINFILE:
                    // Get textfile.
                    ufud_put_binfile(env,b,fd,ipno);
                    break;

                  default:
                    // Unknown command.
                    sprintf(env->msg,"[%s] Unknown command: %c?",ipno,b->action);
                    ufud_log(env);
                    break;

                }

              }
              else {

                if(!ok_uid) {
                  sprintf(env->msg,"[%s] 'seteuid' failed for uid %d!",ipno,env->uid);
                  ufud_log(env);
                }
                if(!ok_gid) {
                  sprintf(env->msg,"[%s] 'setegid' failed for gid %d!",ipno,env->gid);
                  ufud_log(env);
                }

              }

            }
            else {

              // No authorization based on username!
              b->error=TRUE;
              b->last=TRUE;
              // b->errmsg is set in "ufud_check_user".

              sprintf(env->msg,"[%s] Sending back errormessage.",ipno);
              ufud_log(env);

              ufud_send(env,b,fd,ipno);

            }

          }

          ufud_free_buffer(env,b,ipno);

          sprintf(env->msg,"[%s] Terminating pid %d.",ipno,pid);
          ufud_log(env);
          sprintf(env->msg,"[%s] Normal disconnection.",ipno);
          ufud_log(env);
          sprintf(env->msg,"[%s] Host %s has left the building!",ipno,ipname);
          ufud_log(env);

          close(fd);

          exit(0);

        }
        else {

          sprintf(env->msg,"[%s] %s is not allowed to connect!",ipno,ipname);
          ufud_log(env);

          sprintf(env->msg,"[%s] Allocating buffer (%d bytes) for error transfer",ipno,sizeof(struct s_buffer));
          ufud_log(env);

          b=ufud_alloc_buffer(env,ipno);

          sprintf(env->msg,"[%s] Sending back errormessage.",ipno);
          ufud_log(env);

          // No authorization based on ipno!
          b->error=TRUE;
          b->last=TRUE;
          sprintf(b->errmsg,"%s (%s) not allowed to connect!",ipno,ipname);

          ufud_send(env,b,fd,ipno);

          ufud_free_buffer(env,b,ipno);

          exit(1);

        }

      }

      close(fd);  // parent doesn't need this

    }

  }

  ufud_free_users(env);
  ufud_free_groups(env);
  ufud_free_ips(env);

  ufud_free_string(env,ipno);
  ufud_free_string(env,ipname);

  ufud_free_env(env);

  return(0);

}

