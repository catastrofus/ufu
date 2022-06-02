
#include "ufu.h"
#include "ufu.p"

int ufu_split_remote(struct s_env *env,struct s_remote *rh,char *s,int scope) {

  int result;
  int s1_ok,s2_ok,s3_ok,s4_ok;
  char *s1,*s2,*s3,*s4;

  strcpy(env->fun,"ufu_split_remote");
  ufu_wai(env);

  if(env->debug) {
    sprintf(env->msg,"Remote: Line to split \"%s\".",s);
  }

  s1=strtok(s,":");
  s2=strtok(NULL,":");
  s3=strtok(NULL,":");
  s4=strtok(NULL,":");

  s1_ok=(s1!=NULL)&&(strlen(s1)>0);
  s2_ok=(s2!=NULL)&&(strlen(s2)>0);
  s3_ok=(s3!=NULL)&&(strlen(s3)>0);
  s4_ok=(s4!=NULL)&&(strlen(s4)>0);

  result=FALSE;
  // Hostname.
  if(s1_ok) {
    // Port.
    if(s2_ok) {
      // Username.
      if(s3_ok) {
        result=TRUE;
        strcpy(rh->hostname,s1);
        strcpy(rh->username,s3);
        rh->port=atoi(s2);
        if(s4_ok) {
          strcpy(rh->dirname,s4);
        }
        else {
          strcpy(rh->dirname,"/tmp");
        }
      }
    }
  }

  if(env->debug) {
    if(result) {
      sprintf(env->msg,"Remote: Split ok!");
      sprintf(env->msg,"        Hostname=%s:%d",rh->hostname,rh->port);
      sprintf(env->msg,"        Username=%s",rh->username);
      sprintf(env->msg,"        Directory=%s",rh->dirname);
    }
    else {
      sprintf(env->msg,"Remote: Spliterror (%s).",s);
    }
  }

  return(result);

}

int ufu_edit_remote(struct s_env *env,struct s_remote *rh,int scope) {

  int field,again,key,ok,row,col,insert;
  char *s;

  strcpy(env->fun,"ufu_edit_remote");
  ufu_wai(env);

  s=ufu_alloc_string(env,UFU_REM_PORT);

  if(strlen(rh->hostname)>0) {
    sprintf(env->msg,"Edit remote host %d (host=%s,port=%d,user=%s).",rh->seqno,rh->hostname,rh->port,rh->username);
  }
  else {
    sprintf(env->msg,"Edit empty host.");
  }
  ufu_log(env);

  again=TRUE;
  field=UFU_RL_REMOTE_H;

  row=1;
  col=1;

  insert=FALSE;

  while(again) {

    ufu_wclear(env->top);
    mvwprintw(env->top,0,0,"[%s] Edit remote host properties",env->nodename);
    ufu_wrefresh(env->top);

    ufu_wclear(env->body);
    ufu_wrefresh(env->body);

    mvwprintw(env->body,row,col,"Hostname : %s",rh->hostname);
    mvwprintw(env->body,row+1,col,"Username : %s",rh->username);
    mvwprintw(env->body,row+2,col,"Port     : %d",rh->port);
    mvwprintw(env->body,row+3,col,"Directory: %s",rh->dirname);

    ufu_wrefresh(env->body);

    switch(field) {

      case UFU_RL_REMOTE_H:
        // Hostname.
        ufu_rl(env,env->body,col+11,row,insert,UFU_REM_HOSTNAME,UFU_REM_HOSTNAME,rh->hostname,TRUE,TRUE);
        //field=UFU_RL_REMOTE_U;
        break;

      case UFU_RL_REMOTE_U:
        // Username.
        ufu_rl(env,env->body,col+11,row+1,insert,UFU_REM_USERNAME,12,rh->username,TRUE,TRUE);
        //field=UFU_RL_REMOTE_P;
        break;

      case UFU_RL_REMOTE_P:
        // Port.
        sprintf(s,"%d",rh->port);
        if(strcmp(s,"0")==0) {
          strcpy(s,"");
        }
        ufu_rl(env,env->body,col+11,row+2,insert,UFU_REM_PORT,UFU_REM_PORT,s,TRUE,TRUE);
        rh->port=atoi(s);
        //field=UFU_RL_REMOTE_D;
        break;

      case UFU_RL_REMOTE_D:
        // Directory..
        ufu_rl(env,env->body,col+11,row+3,insert,UFU_REM_DIRNAME,UFU_REM_DIRNAME,rh->dirname,TRUE,TRUE);
        //field=UFU_RL_REMOTE_H;
        break;

    }

    ufu_wrefresh(env->body);

    key=ufu_get_key(env,UFU_IS_REMOTE,NULL);

    switch(key) {

      case UFU_KEY_QUIT:
      //case UFU_KEY_SELECT:
        again=FALSE;
        ok=FALSE;
        if(strlen(rh->hostname)>0) {
          if(strlen(rh->username)>0) {
            if(rh->port>0) {
              ok=TRUE;
              ufu_write_config(env);
            }
          }
        }
        break;

      case UFU_KEY_HOME:
        field=UFU_RL_REMOTE_H;
        break;

      case UFU_KEY_END:
        field=UFU_RL_REMOTE_D;
        break;

      case UFU_KEY_DOWN:
        switch(field) {
          case UFU_RL_REMOTE_H:
            field=UFU_RL_REMOTE_U;
            break;
          case UFU_RL_REMOTE_U:
            field=UFU_RL_REMOTE_P;
            break;
          case UFU_RL_REMOTE_P:
            field=UFU_RL_REMOTE_D;
            break;
          case UFU_RL_REMOTE_D:
            field=UFU_RL_REMOTE_H;
            break;
        }
        break;

      case UFU_KEY_UP:
        switch(field) {
          case UFU_RL_REMOTE_D:
            field=UFU_RL_REMOTE_P;
            break;
          case UFU_RL_REMOTE_P:
            field=UFU_RL_REMOTE_U;
            break;
          case UFU_RL_REMOTE_U:
            field=UFU_RL_REMOTE_H;
            break;
          case UFU_RL_REMOTE_H:
            field=UFU_RL_REMOTE_D;
            break;
        }
        break;

      default:
        ufu_wrong_key(env);
        break;

    }

  }

  ufu_free_string(env,s);

  return(ok);

}

void ufu_add_remote(struct s_env *env,struct s_remote *rh,int scope) {

  struct s_remote *rh_tmp,*rhlast;

  strcpy(env->fun,"ufu_add_remote");
  ufu_wai(env);

  rh_tmp=ufu_alloc_remote(env);
  strcpy(rh_tmp->hostname,rh->hostname);
  strcpy(rh_tmp->username,rh->username);
  strcpy(rh_tmp->dirname,rh->dirname);
  rh_tmp->port=rh->port;
  rh_tmp->local=scope;

  if(env->rhlast!=NULL) {

    rhlast=env->rhlast;
    rhlast->next=rh_tmp;
    rh_tmp->prev=rhlast;
    rh_tmp->seqno=rhlast->seqno+1;
    env->rhlast=rh_tmp;

  }
  else {

    rh_tmp->seqno=0;
    rh_tmp->next=NULL;
    rh_tmp->prev=NULL;
    env->rhfirst=rh_tmp;
    env->rhlast=rh_tmp;

  }

  //sprintf(env->msg,"Add remote host (%s,%d,%s).",rh_tmp->hostname,rh_tmp->port,rh_tmp->username);
  //ufu_log(env);

}

int ufu_rem_remote(struct s_env *env,struct s_remote *rh) {

  int seqno;

  strcpy(env->fun,"ufu_rem_remote");
  ufu_wai(env);

  sprintf(env->msg,"Delete remote host %d (host=%s,port=%d,user=%s).",rh->seqno,rh->hostname,rh->port,rh->username);
  ufu_log(env);

  if(rh->prev==NULL) {
    // First entry.
    if(rh->next==NULL) {
      // And no entry left.
      env->rhfirst=NULL;
      env->rhlast=NULL;
    }
    else {
      // Entry is the first of more entries.
      env->rhfirst=rh->next;
      rh->next->prev=NULL;
    }
  }
  else {
    // Not the first entry.
    if(rh->next==NULL) {
      // Last entry.
      env->rhlast=rh->prev;
      rh->prev->next=NULL;
    }
    else {
      // Entry is one of more entries.
      rh->next->prev=rh->prev;
      rh->prev->next=rh->next;
    }
  }

  ufu_free_remote(env,rh);

  seqno=0;
  rh=env->rhfirst;
  while(rh!=NULL) {
    rh->seqno=seqno++;
    rh=rh->next;
  }

  return(TRUE);
}

void ufu_free_remote_hosts(struct s_env *env) {

  struct s_remote *r,*r_tmp;

  strcpy(env->fun,"ufu_free_remote_hosts");
  ufu_wai(env);

  r=env->rhfirst;
  while(r!=NULL) {
    r_tmp=r->next;
    ufu_free_remote(env,r);
    r=r_tmp;
  }

}

void ufu_set_pwd(struct s_env *env,struct s_remote *r) {

  struct s_remote *r1;

  strcpy(env->fun,"ufu_set_pwd");
  ufu_wai(env);

  if(strlen(r->password)>0) {

    r1=env->rhfirst;
    while(r1!=NULL) {

      if(r1->seqno!=r->seqno) {
        if(strcmp(r1->hostname,r->hostname)==0) {
          if(strcmp(r1->username,r->username)==0) {
            strcpy(r1->password,r->password);
          }
        }
      }

      r1=r1->next;

    }

  }

}

void ufu_show_remote(struct s_env *env,char *missing) {

  int again,changed,redraw,key,row,rows,seqno,seq_tos,seq_bos,i,p,max_p,width;
  char *s;
  struct s_remote *rh,*cos,*tos;

  strcpy(env->fun,"ufu_show_remote");
  ufu_wai(env);

  s=ufu_alloc_string(env,UFU_LEN_NAME);
  // s=ufu_alloc_string(env,1024);

  seqno=ufu_find_hist(env,UFU_HIST_REMOTE,NULL);

  width=env->cols;

  rows=env->rows-6;

  rh=env->rhfirst;
  while((rh!=NULL)&&(rh->seqno!=seqno)) {
    rh=rh->next;
  }

  if(rh!=NULL) {
    cos=rh;
    tos=rh;
    i=(rows/2);
    while((i>0)&&(tos!=NULL)) {
      if(tos->prev!=NULL) {
        tos=tos->prev;
      }
      i--;
    }
    if(tos==NULL) {
      tos=rh;
    }
  }
  else {
    tos=env->rhfirst;
    cos=env->rhfirst;
    rh=env->rhfirst;
  }

  changed=FALSE;
  again=(rh!=NULL);

  while(again) {

    if(redraw) {

      ufu_wclear(env->top);
      if(missing!=NULL) { 
        mvwprintw(env->top,0,0,"[%s] REMOTE - Missing connection: %s",env->nodename,missing);
      }
      else {
        mvwprintw(env->top,0,0,"[%s] Show remote hosts",env->nodename);
      }

      ufu_wrefresh(env->top);

      ufu_wclear(env->bottom);
      ufu_wrefresh(env->bottom);

      ufu_wclear(env->body);
      mvwprintw(env->body,0,1,"SeqNo");
      mvwprintw(env->body,0,7,"Hostname");
      mvwprintw(env->body,0,25,"Port");
      mvwprintw(env->body,0,30,"Username");
      mvwprintw(env->body,0,43,"Directory");
      mvwprintw(env->body,0,76+(width-UFU_MIN_COLS),"pwd");

      mvwprintw(env->body,1,1,"-----");
      mvwprintw(env->body,1,7,"----------------");
      mvwprintw(env->body,1,24,"-----");
      mvwprintw(env->body,1,30,"------------");
      mvwprintw(env->body,1,76+(width-UFU_MIN_COLS),"---");
      for(i=0;i<(32+(width-UFU_MIN_COLS));i++) {
        mvwprintw(env->body,1,43+i,"-");
      }

      ufu_wrefresh(env->body);

    }

    rh=tos;
    row=1;

    while(row<=rows) {

      if(rh!=NULL) {

        if(rh==cos)  wattron(env->body,A_REVERSE);

        mvwprintw(env->body,row+1,1,"%5d",rh->seqno);
        ufu_resize_string(env,rh->hostname,16,s);
        mvwprintw(env->body,row+1,7,"%-16s",s);
        mvwprintw(env->body,row+1,24,"%5d",rh->port);
        ufu_resize_string(env,rh->username,16,s);
        mvwprintw(env->body,row+1,30,"%-12s",s);

        ufu_resize_string(env,rh->dirname,32+(width-UFU_MIN_COLS),s);
        mvwprintw(env->body,row+1,43,"%s",s);
        for(i=strlen(s);i<(32+(width-UFU_MIN_COLS));i++) {
          mvwprintw(env->body,row+1,43+i," ");
        }

        if(rh==cos)  wattroff(env->body,A_REVERSE);

        if(strlen(rh->password)>0)
          mvwprintw(env->body,row+1,76+(width-UFU_MIN_COLS)," Y ");
        else
          mvwprintw(env->body,row+1,76+(width-UFU_MIN_COLS),"   ");

        rh=rh->next;

      }

      row++;

    }

    ufu_wrefresh(env->body);

    key=ufu_get_key(env,UFU_NO_TEXT,NULL);

    switch(key) {

      case UFU_KEY_QUIT:
        again=FALSE;
        ufu_add_hist(env,UFU_HIST_REMOTE,NULL,cos->seqno);
        if(changed) {
          ufu_write_config(env);
        }
        break;

      case UFU_KEY_HELP:
        ufu_help(env,env->mpanel+1,UFU_HELP_REMOTE);
        redraw=TRUE;
        break;

      case UFU_KEY_DOWN:
        if(cos->next!=NULL) {
          seq_tos=tos->seqno;
          seq_bos=seq_tos+rows-1;
          cos=cos->next;
          if(cos->seqno>seq_bos) {
            tos=cos;
          }
        }
        redraw=TRUE;
        break;

      case UFU_KEY_UP:
        if(cos->prev!=NULL) {
          seq_tos=tos->seqno;
          seq_bos=seq_tos+rows-1;
          cos=cos->prev;
          if(cos->seqno<seq_tos) {
            i=0;
            while((i<rows)&&(tos->prev!=NULL)) {
              tos=tos->prev;
              i++;
            }
          }
        }
        break;

      case UFU_KEY_HOME:
      case UFU_KEY_FIRST:
        tos=env->rhfirst;
        cos=env->rhfirst;
        break;

      case UFU_KEY_END:
      case UFU_KEY_LAST:
        while(cos->next!=NULL) {
          cos=cos->next;
        }
        tos=cos;
        i=0;
        while((i<(rows-1))&&(tos->prev!=NULL)) {
          tos=tos->prev;
          i++;
        }
        break;

      case UFU_KEY_NEXTPAGE:
        i=0;
        while((i<rows)&&(tos->next!=NULL)) {
          tos=tos->next;
          i++;
        }
        cos=tos;
        break;

      case UFU_KEY_PREVPAGE:
        i=0;
        while((i<rows)&&(tos->prev!=NULL)) {
          tos=tos->prev;
          i++;
        }
        cos=tos;
        break;

      case UFU_KEY_DELETE:
      case UFU_KEY_ERASE:
        // Delete a remote entry.
        if(env->confirmremove) {
          sprintf(env->msg,"%s, are you sure to remove this (%d) entry? ",env->master,cos->seqno);
        }
        if((!env->confirmremove)||(ufu_get_yn(env))) {
          changed=TRUE;
          rh=cos;
          if(cos->next!=NULL) {
            seq_tos=tos->seqno;
            seq_bos=seq_tos+rows-1;
            if(seq_bos>env->rhlast->seqno) {
              seq_bos=env->rhlast->seqno;
            }
            if(tos==cos) {
              cos=cos->next;
              tos=cos;
              seq_tos=tos->seqno;
            }
            else {
              cos=cos->next;
            }
            if(cos->seqno>seq_bos) {
              tos=cos;
            }
          }
          else {
            if(cos->prev!=NULL) {
              cos->prev->next=NULL;
              cos=env->rhfirst;
              while(cos->next!=NULL) {
                cos=cos->next;
              }
              tos=cos;
              i=0;
              while((i<(rows-1))&&(tos->prev!=NULL)) {
                tos=tos->prev;
                i++;
              }
            }
            else {
              again=FALSE;
            }
          }
          ufu_rem_remote(env,rh);
          if((changed)&&(!again)) {
            ufu_write_config(env);
          }
        }
        break;

      case UFU_KEY_EDIT:
        // Edit remote entry.
        if(cos->local) {
          ufu_edit_remote(env,cos,cos->local);
          if((strlen(cos->hostname)==0)||(strlen(cos->username)==0)||(cos->port==0)) {
            ufu_rem_remote(env,cos);
          }
          changed=TRUE;
        }
        break;

      case UFU_KEY_INSERT:
      case UFU_KEY_APPEND:
        // Add new remote entry.
        redraw=TRUE;
        rh=ufu_alloc_remote(env);
        ufu_add_remote(env,rh,TRUE);
        ufu_free_remote(env,rh);

        cos=env->rhlast;
        tos=cos;
        i=0;
        while((i<(rows-1))&&(tos->prev!=NULL)) {
          tos=tos->prev;
          i++;
        }

        rh=tos;
        row=1;
        while(row<=rows) {
          if(rh!=NULL) {
            mvwprintw(env->body,row+1,1,"%5d",rh->seqno);
            if(rh==cos)  wattron(env->body,A_REVERSE);
            mvwprintw(env->body,row+1,7,"%-32s",rh->hostname);
            mvwprintw(env->body,row+1,40,"%5d",rh->port);
            mvwprintw(env->body,row+1,46,"%-16s",rh->username);
            if(rh==cos)  wattroff(env->body,A_REVERSE);
            if(strlen(rh->password)>0) {
              mvwprintw(env->body,row+1,63,"Y");
            }
            else {
              mvwprintw(env->body,row+1,63,"N");
            }
            rh=rh->next;
          }
          row++;
        }
        ufu_wrefresh(env->body);

        changed=ufu_edit_remote(env,cos,cos->local);
        if((strlen(cos->hostname)==0)||(strlen(cos->username)==0)||(cos->port==0)) {
          rh=cos;
          if(rh->next!=NULL) {
            rh=rh->next;
          }
          else {
            if(rh->prev!=NULL) {
              rh=rh->prev;
            }
          }
          ufu_rem_remote(env,cos);
          cos=rh;
          changed=FALSE;
        }

        break;

      case UFU_KEY_CLEAR:
        // (Re)Enter password.
        ufu_clear_string(env,cos->password,UFU_LEN_PWORD);
        sprintf(env->msg,"Enter password for user %s: ",cos->username);
        ufu_msg(env);
        ufu_rl(env,env->bottom,strlen(env->msg),0,TRUE,UFU_LEN_PWORD+1,UFU_LEN_PWORD+1,cos->password,FALSE,FALSE);

        ufu_set_pwd(env,cos);

        break;

      case UFU_KEY_SELECT:
      case UFU_KEY_ENTER:

        if(strlen(cos->password)==0) {

          // Get the password first.
          sprintf(env->msg,"Enter password for user %s: ",cos->username);
          ufu_msg(env);

          ufu_rl(env,env->bottom,strlen(env->msg),0,TRUE,UFU_LEN_PWORD+1,UFU_LEN_PWORD+1,cos->password,FALSE,FALSE);

        }

        sprintf(env->msg,"%s, please wait... ",env->master);
        ufu_msg(env);

        if(strlen(cos->password)>0) {

          ufu_set_pwd(env,cos);

          p=UFU_MIN_PANEL;
          while((p<=env->mpanel)&&(strlen(env->panel[p]->dirname)>0)) {
            max_p=p;
            p++;
          }
          p=max_p+1;
          if(p<env->mpanel) {

            sprintf(env->msg,"Free panel found: %d",p);
            ufu_log(env);

            env->panel[p]->rem=cos;
            if(ufu_com_get_version(env,cos)) {
              ufu_com_read_dir(env,cos->dirname,p);
            }

          }
          else {

            sprintf(env->msg,"No free panels left!");
            ufu_log(env);

          }

        }

        if(missing!=NULL) {
          again=FALSE;
        }

        redraw=TRUE;

        break;

      default:
        ufu_wrong_key(env);
        break;

    }

  }

  ufu_free_string(env,s);

}
