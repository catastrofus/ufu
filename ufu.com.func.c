
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

  if(ufu_com_init(env,env->panel[panel]->rem)) {

    b=ufu_alloc_buffer(env,UFU_B_READ_DIR);

    ufu_crypt_passwd(env,b,env->panel[panel]->rem);

    env->key_add_panel++;

    p=panel;

    sprintf(env->msg,"Remote dir to read: %s",dirname);
    ufu_log(env);

    strcpy(env->panel[p]->dirname,dirname);
    env->panel[p]->header=UFU_HDR_STD;
    env->panel[p]->sortc=UFU_SORT_FNAME;
    env->panel[p]->sorto=UFU_SORT_ASC;

    env->panel[p]->size=(long) 0;
    size=(long) 0;

    b->action=UFU_B_READ_DIR;
    b->last=TRUE;
    b->error=FALSE;
    strcpy(b->dirname,dirname);

    // Send to UFUd daemon on remote host.
    ok=ufu_send(env,env->panel[panel]->rem,b);
    if(ok) {

      env->panel[p]->remote=TRUE;
      env->panel[p]->local=FALSE;

      strcpy(env->panel[p]->nodename,env->panel[panel]->rem->hostname);

      env->panel[p]->first=NULL;
      env->panel[p]->last=NULL;

      env->panel[p]->tos=NULL;
      env->panel[p]->cos=NULL;

      env->panel[p]->ndirs=0;
      env->panel[p]->nfiles=0;
      env->panel[p]->nlinks=0;
      env->panel[p]->ntotal=0;

      e1=NULL;
      e2=NULL;

      last=FALSE;

      // Receive from UFUd daemon.
      while(!last) {

        ufu_clear_string(env,b->username,UFU_LEN_UNAME);
        ufu_clear_string(env,b->password,UFU_LEN_PWORD);
        ufu_clear_string(env,b->dirname,UFU_LEN_NAME);
        ufu_clear_string(env,b->filename,UFU_LEN_NAME);
        ufu_clear_string(env,b->errmsg,UFU_LEN_ERROR);

        ufu_clear_string(env,b->e_fusername,UFU_LEN_UNAME);
        ufu_clear_string(env,b->e_fgroupname,UFU_LEN_GROUP);
        ufu_clear_string(env,b->e_lusername,UFU_LEN_UNAME);
        ufu_clear_string(env,b->e_lgroupname,UFU_LEN_GROUP);

        ufu_clear_string(env,b->e_fname,UFU_LEN_NAME);
        ufu_clear_string(env,b->e_lname,UFU_LEN_NAME);
        ufu_clear_string(env,b->e_fperm,UFU_LEN_PERM);
        ufu_clear_string(env,b->e_lperm,UFU_LEN_PERM);

        if(ufu_recv(env,env->panel[panel]->rem,b)) {

          if(!b->error) {

            // Do something with the data....

            e2=e1;
            e1=ufu_alloc_entry(env);

            e1->seqno=b->e_seqno;

            e1->fatime=b->e_fatime;
            e1->fmtime=b->e_fmtime;
            e1->fctime=b->e_fctime;

            e1->latime=b->e_latime;
            e1->lmtime=b->e_lmtime;
            e1->lctime=b->e_lctime;

            e1->u_read=b->e_u_read;
            e1->u_write=b->e_u_write;
            e1->u_exec=b->e_u_exec;
            e1->g_read=b->e_g_read;
            e1->g_write=b->e_g_write;
            e1->g_exec=b->e_g_exec;
            e1->o_read=b->e_o_read;
            e1->o_write=b->e_o_write;
            e1->o_exec=b->e_o_exec;

            e1->size=b->e_size;

            strcpy(e1->fname,b->e_fname);
            strcpy(e1->fperm,b->e_fperm);
            strcpy(e1->lname,b->e_lname);
            strcpy(e1->lperm,b->e_lperm);

            e1->is_dir=b->e_is_dir;
            e1->is_lnk=b->e_is_lnk;

            strcpy(e1->fusrname,b->e_fusername);
            strcpy(e1->fgrpname,b->e_fgroupname);

            if(!(e1->is_dir)) {
              size+=(long) e1->size;
            }
 
            if(e1->is_lnk) {
              strcpy(e1->lusrname,b->e_lusername);
              strcpy(e1->lgrpname,b->e_lgroupname);
            }

            e1->blocks=b->e_blocks;

            e1->fmode=b->e_fmode;
            e1->lmode=b->e_lmode;

            e1->fino=b->e_fino;
            e1->lino=b->e_lino;

            sprintf(env->msg,"Receiving entry #%d \"%s\".",e1->seqno,e1->fname);
            ufu_log(env);

            last=b->last;
            if(last) {

              env->panel[p]->ndirs=b->e_ndirs;
              env->panel[p]->nfiles=b->e_nfiles;
              env->panel[p]->nlinks=b->e_nlinks;
              env->panel[p]->ntotal=b->e_ntotal;

              env->panel[p]->size=(long) size;

              sprintf(env->msg,"Bytes received from daemon: %ld",env->panel[p]->size);
              ufu_log(env);

            }

            if(env->panel[p]->first!=NULL) {
              env->panel[p]->last=e1;
              e1->next=NULL;
              e1->prev=e2;
              if(e2!=NULL) {
                e2->next=e1;
              }
            }
            else {
              env->panel[p]->first=e1;
              env->panel[p]->last=e1;
              env->panel[p]->tos=NULL;
              env->panel[p]->cos=NULL;
              e1->next=NULL;
              e1->prev=NULL;
            }

          }
          else {

            // Error received from daemon,
            // so let's close the connection.
            last=TRUE;

            // Send failed, clearing up panel p.
            ufu_clear_string(env,env->panel[p]->dirname,UFU_LEN_NAME);

            strcpy(env->msg,b->errmsg);
            ufu_alert(env);

          }

        }
        else {

          // Something wrong with transfer,
          // so let's close the connection.
          last=TRUE;

          sprintf(env->msg,"Something wrong with transfer!");
          ufu_alert(env);

        }

      }

      ufu_com_exit(env,env->panel[panel]->rem);

      // Sort the received entries.
      ufu_sort(env,p);

    }
    else {
      // Send failed, clearing up panel p.
      ufu_com_exit(env,env->panel[panel]->rem);
      ufu_clear_string(env,env->panel[p]->dirname,UFU_LEN_NAME);

      sprintf(env->msg,"Send failed!");
      ufu_alert(env);

    }

    ufu_free_buffer(env,b,UFU_B_READ_DIR);

  }
  else {
    sprintf(env->msg,"Unable to setup connection!");
    ufu_log(env);
    sprintf(env->msg,"Host %s is not reponding!",env->panel[panel]->rem->hostname);
    ufu_alert(env);
  }

 
  return(ok);

}

int ufu_com_read_entry(struct s_env *env,char *dirname,int panel,struct s_entry *e) {

  int ok,p;
  struct s_buffer *b;

  strcpy(env->fun,"ufu_com_read_entry");
  ufu_wai(env);

  ok=FALSE;

  if(ufu_com_init(env,env->panel[panel]->rem)) {

    b=ufu_alloc_buffer(env,UFU_B_READ_DIR);

    ufu_crypt_passwd(env,b,env->panel[panel]->rem);

    p=panel;

    sprintf(env->msg,"Remote file to read: %s",dirname);
    ufu_log(env);

    strcpy(env->panel[p]->dirname,dirname);

    b->action=UFU_B_READ_ENTRY;
    b->last=TRUE;
    b->error=FALSE;
    strcpy(b->dirname,dirname);

    // Send to UFUd daemon on remote host.
    ok=ufu_send(env,env->panel[panel]->rem,b);
    if(ok) {

      // Receive from UFUd daemon.

      ufu_clear_string(env,b->username,UFU_LEN_UNAME);
      ufu_clear_string(env,b->password,UFU_LEN_PWORD);
      ufu_clear_string(env,b->dirname,UFU_LEN_NAME);
      ufu_clear_string(env,b->filename,UFU_LEN_NAME);
      ufu_clear_string(env,b->errmsg,UFU_LEN_ERROR);

      ufu_clear_string(env,b->e_fusername,UFU_LEN_UNAME);
      ufu_clear_string(env,b->e_fgroupname,UFU_LEN_GROUP);
      ufu_clear_string(env,b->e_lusername,UFU_LEN_UNAME);
      ufu_clear_string(env,b->e_lgroupname,UFU_LEN_GROUP);

      ufu_clear_string(env,b->e_fname,UFU_LEN_NAME);
      ufu_clear_string(env,b->e_lname,UFU_LEN_NAME);
      ufu_clear_string(env,b->e_fperm,UFU_LEN_PERM);
      ufu_clear_string(env,b->e_lperm,UFU_LEN_PERM);

      if(ufu_recv(env,env->panel[panel]->rem,b)) {

        if(!b->error) {

          // Do something with the data....

          e->seqno=b->e_seqno;

          e->fatime=b->e_fatime;
          e->fmtime=b->e_fmtime;
          e->fctime=b->e_fctime;

          e->latime=b->e_latime;
          e->lmtime=b->e_lmtime;
          e->lctime=b->e_lctime;

          e->u_read=b->e_u_read;
          e->u_write=b->e_u_write;
          e->u_exec=b->e_u_exec;
          e->g_read=b->e_g_read;
          e->g_write=b->e_g_write;
          e->g_exec=b->e_g_exec;
          e->o_read=b->e_o_read;
          e->o_write=b->e_o_write;
          e->o_exec=b->e_o_exec;

          e->size=(long) b->e_size;

          strcpy(e->fname,b->e_fname);
          strcpy(e->fperm,b->e_fperm);
          strcpy(e->lname,b->e_lname);
          strcpy(e->lperm,b->e_lperm);

          e->is_dir=b->e_is_dir;
          e->is_lnk=b->e_is_lnk;

          strcpy(e->fusrname,b->e_fusername);
          strcpy(e->fgrpname,b->e_fgroupname);

          if(e->is_lnk) {
            strcpy(e->lusrname,b->e_lusername);
            strcpy(e->lgrpname,b->e_lgroupname);
          }

          e->blocks=b->e_blocks;

          e->fmode=b->e_fmode;
          e->lmode=b->e_lmode;

          e->fino=b->e_fino;
          e->lino=b->e_lino;

          if(S_ISREG(e->fmode)) {
            env->panel[p]->size+=(long) b->e_size;
          }
          sprintf(env->msg,"Receiving entry \"%s\".",e->fname);
          ufu_log(env);

        }
        else {

          // Send failed, clearing up panel p.

          strcpy(env->msg,b->errmsg);
          ufu_alert(env);

        }

      }
      else {

        // Something wrong with transfer,
        // so let's close the connection.

        sprintf(env->msg,"Something wrong with transfer!");
        ufu_alert(env);

      }

      ufu_com_exit(env,env->panel[panel]->rem);

    }
    else {
      // Send failed, clearing up panel p.
      ufu_com_exit(env,env->panel[panel]->rem);

      sprintf(env->msg,"Send failed!");
      ufu_alert(env);

    }

    ufu_free_buffer(env,b,UFU_B_READ_DIR);

  }
  else {
    sprintf(env->msg,"Unable to setup connection!");
    ufu_log(env);
    sprintf(env->msg,"Host %s is not reponding!",env->panel[panel]->rem->hostname);
    ufu_alert(env);
  }

 
  return(ok);

}

int ufu_com_get_txtfile(struct s_env *env,char *dname,char *fname,int panel,int ve) {

  int ok,last,i,must_write;
  char c,*f;
  struct s_buffer *b;
  FILE *fp;

  strcpy(env->fun,"ufu_com_get_txtfile");
  ufu_wai(env);

  f=ufu_alloc_string(env,UFU_LEN_NAME);
  sprintf(f,"/tmp/ufu.%d.XXXXXX",env->pid);

  ok=FALSE;

  if(ufu_com_init(env,env->panel[panel]->rem)) {

    b=ufu_alloc_buffer(env,UFU_B_GET_TXTFILE);

    ufu_crypt_passwd(env,b,env->panel[panel]->rem);

    sprintf(env->msg,"Remote txtfile to get: %s",fname);
    ufu_log(env);

    b->action=UFU_B_GET_TXTFILE;
    b->last=TRUE;
    b->error=FALSE;
    strcpy(b->dirname,dname);
    strcpy(b->filename,fname);

    // Send to UFUd daemon on remote host.
    ok=ufu_send(env,env->panel[panel]->rem,b);
    if(ok) {

      last=FALSE;
      i=0;

      // Create a temporary filename.
      if(!(mkstemp(f)<0)) {

        fp=fopen(f,"w");
        if(fp!=NULL) {

          ok=TRUE;

          // Receive from UFUd daemon.
          while(!last) {

            ufu_clear_string(env,b->username,UFU_LEN_UNAME);
            ufu_clear_string(env,b->password,UFU_LEN_PWORD);
            ufu_clear_string(env,b->dirname,UFU_LEN_NAME);
            ufu_clear_string(env,b->filename,UFU_LEN_NAME);
            ufu_clear_string(env,b->errmsg,UFU_LEN_ERROR);

            if(ufu_recv(env,env->panel[panel]->rem,b)) {

              if(!b->error) {

                // Do something with the data....

                last=b->last;
                if(!last) {

                  fputs(b->line,fp);

                  i++;

                }

              }
              else {

                // Error received from daemon,
                // so let's close the connection.
                last=TRUE;
                ok=FALSE;

                strcpy(env->msg,b->errmsg);
                ufu_alert(env);

              }

            }
            else {

              // Something wrong with transfer,
              // so let's close the connection.
              last=TRUE;
              ok=FALSE;

              sprintf(env->msg,"Something wrong with transfer!");
              ufu_alert(env);

            }

          }

          sprintf(env->msg,"%d lines received from daemon.",i);
          ufu_log(env);

          fclose(fp);

          if(ok) {

            must_write=FALSE;

            switch(ve) {
              case UFU_VIEW:
                // View file.
                ufu_cmd_view(env,f,NULL);
                break;
              case UFU_EDIT:
                // Edit file.
                ufu_cmd_edit(env,f,NULL);
                if(env->confirmaction) {
                  sprintf(env->msg,"%s, shall I try to write this humble file? ",env->master);
                  c=ufu_get_yn(env);
                  must_write=(c=='y');
                }
                else {
                  must_write=TRUE;
                }
                break;
            }

            if(must_write) {
  
              ufu_com_exit(env,env->panel[panel]->rem);

              if(ufu_com_init(env,env->panel[panel]->rem)) {

                ufu_free_buffer(env,b,UFU_B_GET_TXTFILE);
                b=ufu_alloc_buffer(env,UFU_B_PUT_TXTFILE);

                ufu_crypt_passwd(env,b,env->panel[panel]->rem);

                sprintf(env->msg,"Remote txtfile to put: %s",fname);
                ufu_log(env);

                fp=fopen(f,"r");
                if(fp!=NULL) {

                  b->action=UFU_B_PUT_TXTFILE;
                  b->last=FALSE;
                  b->error=FALSE;
                  strcpy(b->dirname,dname);
                  strcpy(b->filename,fname);

                  // Send to UFUd daemon on remote host.
                  ok=ufu_send(env,env->panel[panel]->rem,b);
                  if(ok) {

                    if(ufu_recv(env,env->panel[panel]->rem,b)) {

                      if(!b->error) {

                        sprintf(env->msg,"Reading from file \"%s\".",f);
                        ufu_log(env);

                        i=0;

                        ufu_clear_string(env,b->line,UFU_LEN_MSG);
                        while(fgets(b->line,UFU_LEN_MSG,fp)!=NULL) {

                          ufu_send(env,env->panel[panel]->rem,b);

                          ufu_clear_string(env,b->line,UFU_LEN_MSG);

                          i++;

                        }

                        b->action=UFU_B_PUT_TXTFILE;
                        b->last=TRUE;
                        b->error=FALSE;

                        ufu_send(env,env->panel[panel]->rem,b);

                        sprintf(env->msg,"%d lines send to daemon.",i);
                        ufu_log(env);

                        fclose(fp);

                      }
                      else {

                        strcpy(env->msg,b->errmsg);
                        ufu_alert(env);

                      }

                    }
                    else {

                      sprintf(env->msg,"Unable to send!");
                      ufu_alert(env);

                    }

                  }
                  else {

                    // Send failed, clearing up panel p.
                    ufu_com_exit(env,env->panel[panel]->rem);

                    sprintf(env->msg,"Send failed!");
                    ufu_alert(env);

                  }

                }
                else {

                  sprintf(env->msg,"(%s) Unable to open temporary file for reading!",fname);
                  ufu_alert(env);

                }

              }
              else {

                sprintf(env->msg,"Unable to setup connection!");
                ufu_log(env);

                sprintf(env->msg,"Host %s is not reponding!",env->panel[panel]->rem->hostname);
                ufu_alert(env);

              }

            }

          }

          // Remove file.
          unlink(f);

        }
        else {

          sprintf(env->msg,"(%s) Unable to open temporary file for writing!",fname);
          ufu_alert(env);

        }

      }
      else {

        sprintf(env->msg,"(%s) Unable to create temporary file!",fname);
        ufu_alert(env);

      }

      ufu_com_exit(env,env->panel[panel]->rem);

    }
    else {

      // Send failed, clearing up panel p.
      ufu_com_exit(env,env->panel[panel]->rem);

      sprintf(env->msg,"Send failed!");
      ufu_alert(env);

    }

    ufu_free_buffer(env,b,UFU_B_GET_TXTFILE);

  }
  else {

    sprintf(env->msg,"Unable to setup connection!");
    ufu_log(env);

    sprintf(env->msg,"Host %s is not reponding!",env->panel[panel]->rem->hostname);
    ufu_alert(env);

  }

  ufu_free_string(env,f);
 
  return(ok);

}

int ufu_com_get_version(struct s_env *env,struct s_remote *r) {

  int ok;
  struct s_buffer *b;

  strcpy(env->fun,"ufu_com_get_version");
  ufu_wai(env);

  ok=FALSE;

  if(ufu_com_init(env,r)) {

    b=ufu_alloc_buffer(env,UFU_B_GET_VERSION);

    ufu_crypt_passwd(env,b,r);

    b->action=UFU_B_GET_VERSION;
    b->last=TRUE;
    b->error=FALSE;

    // Send to UFUd daemon on remote host.
    ok=ufu_send(env,r,b);
    if(ok) {

      // Receive from UFUd daemon.
      if(ufu_recv(env,r,b)) {

        if(!b->error) {

          // Do something with the data....

          strcpy(r->dversion,b->errmsg);

          sprintf(env->msg,"Receiving daemon version \"%s\".",r->dversion);
          ufu_log(env);

          ok=(strncmp(UFU_VERSION,r->dversion,strlen(UFU_VERSION))==0);

          if(ok) {

            strcpy(env->msg,"Daemon version seems ok.");
            ufu_log(env);

          }
          else {

            strcpy(env->msg,"Difference in client/daemon versions!");
            ufu_log(env);

            sprintf(env->msg,"  Client: %s",UFU_VERSION);
            ufu_log(env);

            sprintf(env->msg,"  Daemon: %s",r->dversion);
            ufu_log(env);

            strcpy(env->msg,"Daemon version differs from client version!");
            ufu_alert(env);

          }

        }
        else {

          // Send failed, clearing up panel p.

          strcpy(env->msg,b->errmsg);
          ufu_alert(env);

        }

      }
      else {

        // Something wrong with transfer,
        // so let's close the connection.

        sprintf(env->msg,"Something wrong with transfer!");
        ufu_alert(env);

      }

      ufu_com_exit(env,r);

    }
    else {
      // Send failed, clearing up panel p.
      ufu_com_exit(env,r);

      sprintf(env->msg,"Send failed!");
      ufu_alert(env);

    }

    ufu_free_buffer(env,b,UFU_B_GET_VERSION);

  }
  else {

    sprintf(env->msg,"Unable to setup connection!");
    ufu_log(env);

    sprintf(env->msg,"Host %s is not reponding!",r->hostname);
    ufu_alert(env);

  }

 
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

  f=ufu_alloc_string(env,UFU_LEN_NAME);
  sprintf(f,"/tmp/ufu.%d.XXXXXX",env->pid);

  ok=FALSE;

  if(ufu_com_init(env,env->panel[panel]->rem)) {

    b=ufu_alloc_buffer(env,UFU_B_GET_BINFILE);
    ba=ufu_alloc_buffer_action(env,UFU_B_GET_BINFILE);

    ufu_crypt_passwd(env,b,env->panel[panel]->rem);

    sprintf(env->msg,"Remote binfile to get: %s",fname);
    ufu_log(env);

    b->action=UFU_B_GET_BINFILE;
    b->last=TRUE;
    b->error=FALSE;
    strcpy(b->dirname,dname);
    strcpy(b->filename,fname);

    // Send to UFUd daemon on remote host.
    ok=ufu_send(env,env->panel[panel]->rem,b);
    if(ok) {

      last=FALSE;
      i=0;

      // Create a temporary filename.
      if(!(mkstemp(f)<0)) {

        fp=fopen(f,"w");
        if(fp!=NULL) {

          ok=TRUE;

          // Receive from UFUd daemon.
          while(!last) {

            ufu_clear_string(env,ba->username,UFU_LEN_UNAME);
            ufu_clear_string(env,ba->password,UFU_LEN_PWORD);
            ufu_clear_string(env,ba->dirname,UFU_LEN_NAME);
            ufu_clear_string(env,ba->filename,UFU_LEN_NAME);
            ufu_clear_string(env,ba->errmsg,UFU_LEN_ERROR);

            if(ufu_recv_action(env,env->panel[panel]->rem,ba)) {

              if(!ba->error) {

                // Do something with the data....

                last=ba->last;
                if(!last) {

                  fputs(ba->line,fp);

                  i++;

                }

              }
              else {

                // Error received from daemon,
                // so let's close the connection.
                last=TRUE;
                ok=FALSE;

                strcpy(env->msg,ba->errmsg);
                ufu_alert(env);

              }

            }
            else {

              // Something wrong with transfer,
              // so let's close the connection.
              last=TRUE;
              ok=FALSE;

              sprintf(env->msg,"Something wrong with transfer!");
              ufu_alert(env);

            }

          }

          sprintf(env->msg,"%ld bytes received from daemon.",(long)i*UFU_ACTION_BUFFER);
          ufu_log(env);

          fclose(fp);

        }
        else {

          sprintf(env->msg,"(%s) Unable to open temporary file for writing!",fname);
          ufu_alert(env);

        }

      }
      else {

        sprintf(env->msg,"(%s) Unable to create temporary file!",fname);
        ufu_alert(env);

      }

      ufu_com_exit(env,env->panel[panel]->rem);

    }
    else {

      // Send failed, clearing up panel p.
      ufu_com_exit(env,env->panel[panel]->rem);

      sprintf(env->msg,"Send failed!");
      ufu_alert(env);

    }

    ufu_free_buffer(env,b,UFU_B_GET_TXTFILE);

  }
  else {

    sprintf(env->msg,"Unable to setup connection!");
    ufu_log(env);

    sprintf(env->msg,"Host %s is not reponding!",env->panel[panel]->rem->hostname);
    ufu_alert(env);

  }

  ufu_free_string(env,f);
 
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

  f=ufu_alloc_string(env,UFU_LEN_NAME);
  sprintf(f,"/tmp/ufu.%d.XXXXXX",env->pid);

  ok=FALSE;

  if(ufu_com_init(env,env->panel[panel]->rem)) {

    b=ufu_alloc_buffer(env,UFU_B_PUT_BINFILE);
    ba=ufu_alloc_buffer_action(env,UFU_B_PUT_BINFILE);

    ufu_crypt_passwd(env,b,env->panel[panel]->rem);

    sprintf(env->msg,"Remote binfile to put: %s",fname);
    ufu_log(env);

    b->action=UFU_B_PUT_BINFILE;
    b->last=TRUE;
    b->error=FALSE;
    strcpy(b->dirname,dname);
    strcpy(b->filename,fname);

    // Send to UFUd daemon on remote host.
    ok=ufu_send(env,env->panel[panel]->rem,b);
    if(ok) {

      last=FALSE;
      i=0;

      // Create a temporary filename.
      if(!(mkstemp(f)<0)) {

        fp=fopen(f,"r");
        if(fp!=NULL) {

          ok=TRUE;

          // Receive from UFUd daemon.
          while(!last) {

            ufu_clear_string(env,ba->username,UFU_LEN_UNAME);
            ufu_clear_string(env,ba->password,UFU_LEN_PWORD);
            ufu_clear_string(env,ba->dirname,UFU_LEN_NAME);
            ufu_clear_string(env,ba->filename,UFU_LEN_NAME);
            ufu_clear_string(env,ba->errmsg,UFU_LEN_ERROR);

            if(ufu_send_action(env,env->panel[panel]->rem,ba)) {

              if(!ba->error) {

                // Do something with the data....

                last=ba->last;
                if(!last) {

                  fputs(ba->line,fp);

                  i++;

                }

              }
              else {

                // Error received from daemon,
                // so let's close the connection.
                last=TRUE;
                ok=FALSE;

                strcpy(env->msg,ba->errmsg);
                ufu_alert(env);

              }

            }
            else {

              // Something wrong with transfer,
              // so let's close the connection.
              last=TRUE;
              ok=FALSE;

              sprintf(env->msg,"Something wrong with transfer!");
              ufu_alert(env);

            }

          }

          sprintf(env->msg,"%ld bytes received from daemon.",(long)i*UFU_ACTION_BUFFER);
          ufu_log(env);

          fclose(fp);

        }
        else {

          sprintf(env->msg,"(%s) Unable to open temporary file for reading!",fname);
          ufu_alert(env);

        }

      }
      else {

        sprintf(env->msg,"(%s) Unable to open temporary file!",fname);
        ufu_alert(env);

      }

      ufu_com_exit(env,env->panel[panel]->rem);

    }
    else {

      // Send failed, clearing up panel p.
      ufu_com_exit(env,env->panel[panel]->rem);

      sprintf(env->msg,"Send failed!");
      ufu_alert(env);

    }

    ufu_free_buffer(env,b,UFU_B_GET_TXTFILE);

  }
  else {

    sprintf(env->msg,"Unable to setup connection!");
    ufu_log(env);

    sprintf(env->msg,"Host %s is not reponding!",env->panel[panel]->rem->hostname);
    ufu_alert(env);

  }

  ufu_free_string(env,f);
 
  return(ok);

}

