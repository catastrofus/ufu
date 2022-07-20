
#include "ufu.h"
#include "ufu.p"

struct s_remote *ufu_show_remote(struct s_env *env,int editable) {

  WINDOW *w;
  int again,changed,redraw,key,row,rows,seqno,seq_tos,seq_bos;
  int i,p,max_p,len,width;
  char *s,*pwd;
  struct s_remote *rh,*rh1,*rh2,*rhreturn,*rhadd,*cos,*tos;

  strcpy(env->fun,"ufu_show_remote");
  ufu_wai(env);

  s=ufu_alloc_string(env,UFU_LEN_NAME);

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
  again=TRUE;
  redraw=TRUE;

  if(env->rhfirst==NULL) {

    again=ufu_add_remote(env,cos);
    if(again) {
      tos=env->rhfirst;
      cos=env->rhfirst;
      rh=env->rhfirst;
    }

  }

  while(again) {

    if(redraw) {

      ufu_wclear(env->top);
      mvwprintw(env->top,0,0,"[%s] Show remote hosts",env->nodename);

      ufu_wrefresh(env->top);

      ufu_wclear(env->bottom);
      ufu_wrefresh(env->bottom);

      ufu_wclear(env->body);
      mvwprintw(env->body,0,1,"SeqNo");
      mvwprintw(env->body,0,7,"Hostname");
      mvwprintw(env->body,0,7+UFU_REM_HOSTNAME+1,"Port");
      mvwprintw(env->body,0,7+UFU_REM_HOSTNAME+7,"Username");
      mvwprintw(env->body,0,7+UFU_REM_HOSTNAME+UFU_REM_USERNAME+8,"pwd");

      mvwprintw(env->body,1,1,"-----");
      for(i=0;i<UFU_REM_HOSTNAME;i++) {
        mvwprintw(env->body,1,7+i,"-");
      }
      mvwprintw(env->body,1,7+UFU_REM_HOSTNAME+1,"-----");
      for(i=0;i<UFU_REM_HOSTNAME;i++) {
        mvwprintw(env->body,1,7+UFU_REM_HOSTNAME+7+i,"-");
      }
      mvwprintw(env->body,1,7+UFU_REM_HOSTNAME+UFU_REM_USERNAME+8,"---");

      ufu_wrefresh(env->body);

    }

    rh=tos;
    row=1;

    while(row<=rows) {

      if(rh!=NULL) {

        if(rh==cos)  wattron(env->body,A_REVERSE);

        mvwprintw(env->body,row+1,1,"%5d",rh->seqno);
        mvwprintw(env->body,row+1,7,"%s",rh->hostname);
	len=strlen(rh->hostname);
	for(i=len;i<UFU_REM_HOSTNAME;i++) {
          mvwprintw(env->body,row+1,2+len+i," ");
	}
        mvwprintw(env->body,row+1,8+UFU_REM_HOSTNAME,"%5d",rh->port);
        mvwprintw(env->body,row+1,14+UFU_REM_HOSTNAME,"%s",rh->username);
	len=strlen(rh->username);
	for(i=len;i<UFU_REM_USERNAME;i++) {
          mvwprintw(env->body,row+1,14+UFU_REM_HOSTNAME+i," ");
	}

        if(rh==cos)  wattroff(env->body,A_REVERSE);

	len=UFU_REM_HOSTNAME+UFU_REM_USERNAME;
	if(strlen(rh->password)>0) {
          mvwprintw(env->body,row+1,7+len+8," y ");
	}
	else {
          mvwprintw(env->body,row+1,7+len+8,"   ");
	}

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
	if(editable) {
          if(changed) {
            ufu_write_config(env);
          }
	}
	rhreturn=NULL;
        break;

      case UFU_KEY_HELP:
        ufu_help(env,env->mpanel+1,UFU_HELP_REMOTE);
        redraw=TRUE;
        break;

      case UFU_KEY_CONFIG:
      case UFU_KEY_F4:
        env->key_config++;
        ufu_show_config(env);
        redraw=TRUE;
        break;

      case UFU_KEY_SETTING:
      case UFU_KEY_F3:
        env->key_setting++;
        ufu_show_setting(env);
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
	if(editable) {
          rh1=cos->prev;
	  rh2=cos->next;
          if(ufu_rem_remote(env,cos)) {
            redraw=TRUE;
            if(rh2!=NULL) {
              cos=rh2;
	      if(rh1==NULL) {
		tos=rh2;
              }
	    }
	    else {
              if(rh1!=NULL) {
		tos=rh1;
		cos=rh1;
              }
	      else {
		again=FALSE;
              }
	    }
	  }
        }
        break;

      case UFU_KEY_EDIT:
        // Edit remote entry.
	if(editable) {
          if(ufu_add_remote(env,cos)) {
            redraw=TRUE;
	  }
        }
        break;

      case UFU_KEY_INSERT:
      case UFU_KEY_APPEND:
        // Add new remote entry.
	rhadd=NULL;
	if(editable) {
          if((!ufu_add_remote(env,rhadd))&&(env->rhfirst==NULL)) {
            again=FALSE;
	  }
	  else {
            redraw=TRUE;
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
	}
        break;

      case UFU_KEY_CLEAR:
        // (Re)Enter password.
        pwd=ufu_alloc_string(env,env->cols);
	sprintf(pwd,"Enter password for %s@%s",cos->username,cos->hostname);
	len=strlen(cos->hostname)+strlen(cos->username)+20;
	if(len<(UFU_REM_PASSWORD+3)) {
          len=UFU_REM_PASSWORD+3;
	}
	w=ufu_popup(env,len+4,3,(env->cols-len)/2,(env->rows/2)-1,pwd);
	ufu_rl2(env,w,1,1,UFU_REM_PASSWORD,FALSE,cos->password);
	ufu_popdown(env,w);
	ufu_free_string(env,pwd);
        break;

      case UFU_KEY_SELECT:
      case UFU_KEY_ENTER:
	if(editable) {
          redraw=TRUE;
	}
	else {
          again=FALSE;
	  rhreturn=cos;
	}
        break;

      default:
        ufu_wrong_key(env);
        break;

    }

  }

  ufu_free_string(env,s);

  return(rhreturn);

}

int ufu_add_remote(struct s_env *env,struct s_remote *rh) {

  int new,retval;
  char *s;
  struct s_remote *rh1,*rh2;

  rh1=ufu_alloc_remote(env);
  new=(rh==NULL);
  retval=FALSE;

  if(!new) {
    mvwprintw(env->top,0,0,"[%s] Edit remote host",env->nodename);
    strcpy(rh1->hostname,rh->hostname);
    strcpy(rh1->username,rh->username);
    strcpy(rh1->password,rh->password);
    rh1->port=rh->port;
  }
  else {
    mvwprintw(env->top,0,0,"[%s] Add remote host",env->nodename);
    strcpy(rh1->hostname,"");
    strcpy(rh1->username,"");
    strcpy(rh1->password,"");
    rh1->port=UFU_DEF_PORT;
  }

  ufu_wrefresh(env->top);

  ufu_wclear(env->bottom);
  ufu_wrefresh(env->bottom);

  ufu_wclear(env->body);
  mvwprintw(env->body,1,1,"Hostname:");
  mvwprintw(env->body,2,1,"Port:");
  mvwprintw(env->body,3,1,"Username:");
  mvwprintw(env->body,4,1,"Password:");
  mvwprintw(env->body,1,11,"%s",rh1->hostname);
  mvwprintw(env->body,2,11,"%d",rh1->port);
  mvwprintw(env->body,3,11,"%s",rh1->username);

  ufu_wrefresh(env->body);

  ufu_rl2(env,env->body,11,1,UFU_REM_HOSTNAME,TRUE,rh1->hostname);
  mvwprintw(env->body,1,11,"%s  ",rh1->hostname);
  s=ufu_alloc_string(env,UFU_REM_PORT+1);
  sprintf(s,"%d",rh1->port);
  ufu_rl2(env,env->body,11,2,UFU_REM_PORT,TRUE,s);
  if(strlen(s)>0) {
    rh1->port=atoi(s);
  }
  mvwprintw(env->body,2,11,"%d",rh1->port);
  ufu_free_string(env,s);
  ufu_rl2(env,env->body,11,3,UFU_REM_USERNAME,TRUE,rh1->username);
  mvwprintw(env->body,3,11,"%s",rh1->username);
  ufu_rl2(env,env->body,11,4,UFU_REM_PASSWORD,FALSE,rh1->password);
  mvwprintw(env->body,4,11,"%s",rh1->password);

  if((strlen(rh1->hostname)>0)&&(strlen(rh1->username)>0)) {
    if(!new) {
      strcpy(rh->hostname,rh1->hostname);
      strcpy(rh->username,rh1->username);
      strcpy(rh->password,rh1->password);
      rh->port=rh1->port;
    }
    else {
      if(env->rhfirst!=NULL) {
        rh2=env->rhfirst;
        while(rh2->next!=NULL) {
          rh2=rh2->next;
        }
        rh2->next=ufu_alloc_remote(env);
        rh2->next->prev=rh2;
        rh2->next->seqno=rh2->seqno+1;
        strcpy(rh2->next->hostname,rh1->hostname);
        strcpy(rh2->next->username,rh1->username);
        strcpy(rh2->next->password,rh1->password);
        rh2->next->port=rh1->port;
      }
      else {
        env->rhfirst=ufu_alloc_remote(env);
	env->rhfirst->seqno=0;
        strcpy(env->rhfirst->hostname,rh1->hostname);
        strcpy(env->rhfirst->username,rh1->username);
        strcpy(env->rhfirst->password,rh1->password);
        env->rhfirst->port=rh1->port;
      }
    }
    retval=TRUE;
  }

  ufu_free_remote(env,rh1);

  return(retval);

}

int ufu_rem_remote(struct s_env *env,struct s_remote *rh) {

  int retval;
  struct s_remote *rh1,*rh2;

  retval=FALSE;
  rh1=NULL;
  rh2=NULL;

  sprintf(env->msg,"Do you really want to remove %s@%s:%d? ",rh->username,rh->hostname,rh->port);
  if(ufu_get_yn(env)=='y') {

    if(rh->next!=NULL) {
      rh2=rh->next;
    }
    if(rh->prev!=NULL) {
      rh1=rh->prev;
    }
    else {
      env->rhfirst=rh2;
    }
    if(rh1!=NULL) {
      rh1->next=rh2;
    }
    if(rh2!=NULL) {
      rh2->prev=rh1;
    }

    ufu_free_remote(env,rh);

    retval=TRUE;

  }

  return(retval);

}

void ufu_free_remote_hosts(struct s_env *env) {

  struct s_remote *rh1,*rh2;

  strcpy(env->fun,"ufu_free_remote_hosts");
  ufu_wai(env);

  rh1=env->rhfirst;
  while(rh1!=NULL) {
    rh2=rh1->next;
    ufu_free_remote(env,rh1);
    rh1=rh2;
  }

}

