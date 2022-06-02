
#include "ufu.h"
#include "ufu.p"

void ufu_show_tc(struct s_env *env) {

  int again,redraw,key,row,rows,seqno,seq_tos,seq_bos,i,r,width;
  char *fname,*s;
  struct s_tc *tc,*cos,*tos;

  strcpy(env->fun,"ufu_show_tc");
  ufu_wai(env);

  fname=ufu_alloc_string(env,UFU_LEN_NAME);
  s=ufu_alloc_string(env,UFU_LEN_NAME);

  width=env->cols;

  rows=env->rows-6;

  tos=env->tcfirst;
  cos=env->tcfirst;

  seqno=env->hist_tc;
  tc=env->tcfirst;
  while((tc!=NULL)&&(tc->seqno!=seqno)) {
    tc=tc->next;
  }

  if(tc!=NULL) {
    cos=tc;
    tos=tc;

    r=rows/2;
    while((tc->prev!=NULL)&&(r>0)) {
      r--;
      tos=tc;
      tc=tc->prev;
    }
 
  }
  else {
    tc=env->tcfirst;
  }

  redraw=TRUE;
  again=(tc!=NULL);

  while((again)&&(env->tcfirst!=NULL)) {

    if(redraw) {

      ufu_wclear(env->top);
      mvwprintw(env->top,0,0,"[%s] Show trashcan",env->nodename);

      ufu_wrefresh(env->top);

      ufu_wclear(env->bottom);
      ufu_wrefresh(env->bottom);

      ufu_wclear(env->body);

      mvwprintw(env->body,0,1,"SeqNo");
      mvwprintw(env->body,0,7,"Node");
      mvwprintw(env->body,0,24,"Alias");
      mvwprintw(env->body,0,33,"Deleted file");

      mvwprintw(env->body,1,1,"-----");
      mvwprintw(env->body,1,7,"----------------");
      mvwprintw(env->body,1,24,"--------");

      for(i=0;i<(width-34);i++) {
        mvwprintw(env->body,1,i+33,"-");
      }

      ufu_wrefresh(env->body);

    }

    tc=tos;
    row=1;

    while(row<=rows) {

      if(tc!=NULL) {

        if(tc==cos)  wattron(env->body,A_REVERSE);

        ufu_concat_dir(env,fname,tc->dir,tc->file);
        ufu_resize_string(env,fname,width-34,s);

        mvwprintw(env->body,row+1,1,"%5d",tc->seqno);
        mvwprintw(env->body,row+1,7,"%-16s",tc->nodename);
        mvwprintw(env->body,row+1,24,"%-8s",tc->alias);

        mvwprintw(env->body,row+1,33,"%s",s);
        for(i=strlen(s);i<(width-34);i++) {
          mvwprintw(env->body,row+1,i+33," ");
        }

        if(tc==cos)  wattroff(env->body,A_REVERSE);

        tc=tc->next;

      }

      row++;

    }

    ufu_wrefresh(env->body);

    sprintf(env->msg,"This file is deleted on Sun Jul 22 11:57:35 2007! Your wish is my command:");

    if(strlen(env->msg)>0)
      key=ufu_get_key(env,UFU_NO_TEXT,env->msg);
    else
      key=ufu_get_key(env,UFU_NO_TEXT,NULL);

    //key=ufu_get_key(env,UFU_NO_TEXT,NULL);

    switch(key) {

      case UFU_KEY_QUIT:
        ufu_add_hist(env,UFU_HIST_TC,NULL,cos->seqno);
        again=FALSE;
        break;

      case UFU_KEY_HELP:
      case UFU_KEY_F1:
        env->key_help++;
        ufu_help(env,env->mpanel+1,UFU_HELP_TC);
        redraw=TRUE;
        break;

      case UFU_KEY_DOWN:
        env->key_down++;
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
        env->key_up++;
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

      case UFU_KEY_FIRST:
      case UFU_KEY_HOME:
        env->key_first++;
        tos=env->tcfirst;
        cos=env->tcfirst;
        break;

      case UFU_KEY_LAST:
      case UFU_KEY_END:
        env->key_last++;
        while(cos->next!=NULL) {
          cos=cos->next;
        }
        //env->panel[panel]->cos=cos;
        //env->panel[panel]->tos=cos;
        tos=cos;
        i=0;
        while((i<(rows-1))&&(tos->prev!=NULL)) {
          tos=tos->prev;
          i++;
        }
        break;

      case UFU_KEY_NEXTPAGE:
      case KEY_NPAGE:
        env->key_next_page++;
        i=0;
        while((i<rows)&&(tos->next!=NULL)) {
          tos=tos->next;
          i++;
        }
        cos=tos;
        break;

      case UFU_KEY_PREVPAGE:
      case KEY_PPAGE:
        env->key_prev_page++;
        i=0;
        while((i<rows)&&(tos->prev!=NULL)) {
          tos=tos->prev;
          i++;
        }
        cos=tos;
        break;

      case UFU_KEY_SETTING:
        env->key_setting++;
        ufu_show_setting(env);
        redraw=TRUE;
        break;

      case UFU_KEY_LOG:
        env->key_view++;
	ufu_view_log(env);
        break;

      default:
        ufu_wrong_key(env);
        break;

    }

  }

  ufu_free_string(env,fname);
  ufu_free_string(env,s);

}

int ufu_read_tc(struct s_env *env) {

  int c;
  char *f,*s;
  char *s1,*s2,*s3,*s4,*s5,*s6;
  struct s_tc *tc,*tc_next,*tc_prev;
  FILE *fp;

  strcpy(env->fun,"ufu_read_tc");
  ufu_wai(env);

  f=ufu_alloc_string(env,UFU_LEN_NAME);
  s=ufu_alloc_string(env,UFU_LEN_NAME);
  s1=ufu_alloc_string(env,UFU_LEN_MSG);

  if(env->tcfirst!=NULL) {

    sprintf(env->msg,"Clearing current trash list.");
    ufu_log(env);

    tc=env->tcfirst;
    while(tc!=NULL) {
      tc_next=tc->next;
      ufu_free_tc(env,tc);
      tc=tc_next;
    }

    env->tcfirst=NULL;

  }
  else {

    sprintf(env->msg,"No trash in use, no need to clear the list!");
    ufu_log(env);

  }

  c=0;

  strcpy(f,env->tcfile);
  if((fp=fopen(f,"r"))!=NULL) {

    sprintf(env->msg,"Opening trashfile \"%s\" for reading.",f);
    ufu_log(env);

    tc_prev=NULL;

    while(fgets(s1,UFU_LEN_MSG-1,fp)!=NULL) {

      s2=strtok(s1,"|");    // Nodename.
      s3=strtok(NULL,"|");  // Directory.
      s4=strtok(NULL,"|");  // Filename.
      s5=strtok(NULL,"|");  // Aliasname.
      s6=strtok(NULL,"|");  // Date.

      tc=ufu_alloc_tc(env);
      tc->seqno=c;
      strcpy(tc->nodename,s2);
      strcpy(tc->dir,s3);
      strcpy(tc->file,s4);
      strcpy(tc->alias,s5);
      tc->date=atol(s6);

      if(c>0) {
        tc->prev=tc_prev;   // 'prev' of current struct.
        tc->next=NULL;
        tc->prev->next=tc;  // 'next' of previous struct.
      }
      else {
        env->tcfirst=tc;
      }
      tc_prev=tc;

      ufu_concat_dir(env,s,tc->dir,tc->file);
      sprintf(env->msg,"Trash #%d: %s (%s)",c,s,tc->alias);
      ufu_log(env);

      c++;

    }

    fclose(fp);

    sprintf(env->msg,"Read %d item(s) from trashfile.",c);
    ufu_log(env);

  }
  else {

    sprintf(env->msg,"Trashfile \"%s\" does not exist!",f);
    ufu_log(env);

  }

  ufu_free_string(env,s1);
  ufu_free_string(env,f);

  return(c);

}

int ufu_write_tc(struct s_env *env) {

  int c;
  char *f;
  struct s_tc *tc;
  FILE *fp;

  strcpy(env->fun,"ufu_write_tc");
  ufu_wai(env);

  f=ufu_alloc_string(env,UFU_LEN_NAME);

  c=0;

  tc=env->tcfirst;
  if(tc!=NULL) {
    strcpy(f,env->tcfile);
    if((fp=fopen(f,"w"))!=NULL) {

      sprintf(env->msg,"Opening trashfile \"%s\" for writing.",f);
      ufu_log(env);

      while(tc!=NULL) {

        fprintf(fp,"%s:%s:%s:%s:%ld",tc->nodename,tc->dir,tc->file,tc->alias,tc->date);

        tc->seqno=c;

        c++;
        tc=tc->next;
      }

      fclose(fp);

      sprintf(env->msg,"Wrote %d item(s) to trashfile.",c);
      ufu_log(env);

    }
    else {

      sprintf(env->msg,"Error opening trashfile \"%s\"!",f);
      ufu_log(env);

    }

  }
  else {

    sprintf(env->msg,"No items available to write to trashfile!");
    ufu_log(env);

  }

  ufu_free_string(env,f);

  return(c);

}

