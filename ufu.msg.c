
#include "ufu.h"
#include "ufu.p"

void ufu_show_msg(struct s_env *env) {

  int rem,again,found,redraw,key,len,row,col,rows,seqno,seq_tos,seq_bos,i,r;
  int len_msg,len_tot,len_tmp,c;
  char *s;
  struct s_msg *m,*cos,*tos;

  strcpy(env->fun,"ufu_show_msg");
  ufu_wai(env);

  sprintf(env->msg,"Scanning messagelength.");
  ufu_log(env);

  len_msg=0;
  m=env->msgfirst;
  while(m!=NULL) {
    if(strlen(m->msg)>len_msg) {
      len_msg=strlen(m->msg);
      sprintf(env->msg,"Max length message: %d",len_msg);
      ufu_log(env);
    }
    m=m->next;
  }

  sprintf(env->msg,"Scanning finished.");
  ufu_log(env);

  s=ufu_alloc_string(env,len_msg+4);

  tos=env->msgfirst;
  cos=env->msgfirst;

  m=env->msgfirst;

  rows=env->rows-6;

  if(env->hist_msg>0) {

    seqno=env->hist_msg;
    m=env->msgfirst;
    while((m!=NULL)&&(m->seqno!=seqno)) {
      m=m->next;
    }

    if(m!=NULL) {
      cos=m;
      tos=m;

      r=rows/2;
      while((m->prev!=NULL)&&(r>0)) {
        r--;
        tos=m;
        m=m->prev;
      }
    }
    else {
      m=env->msgfirst;
    }

  }

  redraw=TRUE;
  again=(m!=NULL);

  while(again) {

    if(redraw) {

      ufu_wclear(env->top);
      mvwprintw(env->top,0,0,"[%s] Show messages",env->nodename);

      ufu_wrefresh(env->top);

      ufu_wclear(env->bottom);
      ufu_wrefresh(env->bottom);

      ufu_wclear(env->body);

      // Seqno.
      mvwprintw(env->body,0,1,"Seqno");
      mvwprintw(env->body,1,1,"-----");
      // ID.
      mvwprintw(env->body,0,7,"ID");
      mvwprintw(env->body,1,7,"--------");

      // Message.
      len_tot=17+len_msg;
      if(len_tot>env->cols) {
	len_tot=env->cols;
	len_msg=env->cols-17;
      }
      mvwprintw(env->body,0,16,"Message");
      for(i=0;i<len_msg;i++) {
        mvwprintw(env->body,1,16+i,"-");
      }

      ufu_wrefresh(env->body);

    }

    m=tos;
    row=1;

    while(row<=rows) {

      if(m!=NULL) {

        if(m==cos)  wattron(env->body,A_REVERSE);

	// Seqno.
        mvwprintw(env->body,row+1,1,"%5d",m->seqno);

	// ID.
        mvwprintw(env->body,row+1,7,"%8s",m->id);

	// Message.
	ufu_resize_string(env,m->msg,env->cols-17,s);
        mvwprintw(env->body,row+1,16,"%s",s);
        for(i=strlen(m->msg);i<len_msg;i++) {
          mvwprintw(env->body,row+1,16+i," ");
        }

        if(m==cos)  wattroff(env->body,A_REVERSE);

        m=m->next;

      }

      row++;

    }

    ufu_wrefresh(env->body);

    key=ufu_get_key(env,UFU_NO_TEXT,NULL);

    switch(key) {

      case UFU_KEY_QUIT:
        env->hist_msg=cos->seqno;
        again=FALSE;
        break;

      case UFU_KEY_LOG:
        env->key_view++;
	ufu_view_log(env);
        break;

      case UFU_KEY_HELP:
      case UFU_KEY_F1:
        env->key_help++;
        ufu_help(env,env->mpanel+1,UFU_HELP_U);
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
        tos=env->msgfirst;
        cos=env->msgfirst;
        break;

      case UFU_KEY_LAST:
      case UFU_KEY_END:
        env->key_last++;
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

      case KEY_NPAGE:
      case UFU_KEY_NEXTPAGE:
        env->key_next_page++;
        i=0;
        while((i<rows)&&(tos->next!=NULL)) {
          tos=tos->next;
          i++;
        }
        cos=tos;
        break;

      case KEY_PPAGE:
      case UFU_KEY_PREVPAGE:
        env->key_prev_page++;
        i=0;
        while((i<rows)&&(tos->prev!=NULL)) {
          tos=tos->prev;
          i++;
        }
        cos=tos;
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

      default:
        ufu_wrong_key(env);
        break;

    }

  }

  ufu_free_string(env,s);

}

void ufu_sort_msg(struct s_env *env) {

  struct s_msg *m1,*m2,*t;

  strcpy(env->fun,"ufu_sort_msg");
  ufu_wai(env);

  sprintf(env->msg,"Start sorting messages");
  ufu_log(env);

  m1=env->msgfirst;
  while(m1!=NULL) {

    m2=env->msgfirst;
    while((m2!=NULL)&&(m2->seqno<m1->seqno)) {

      if(strcmp(m2->id,m1->id)>0) {

        sprintf(env->msg,"Swapping messages %s & %s.",m1->id,m2->id);
        ufu_log(env);

        t=ufu_alloc_msg(env);

	strcpy(t->id,m2->id);
	strcpy(t->msg,m2->msg);

	strcpy(m2->id,m1->id);
	strcpy(m2->msg,m1->msg);

	strcpy(m1->id,t->id);
	strcpy(m1->msg,t->msg);

        ufu_free_msg(env,t);

      }

      m2=m2->next;

    }

    m1=m1->next;
  }

  sprintf(env->msg,"Finished sorting messages");
  ufu_log(env);

  sprintf(env->msg,"List of sorted messages");
  ufu_log(env);

  m1=env->msgfirst;
  while(m1!=NULL) {

    sprintf(env->msg," (%05d) %s: %s",m1->seqno,m1->id,m1->msg);
    ufu_log(env);

    m1=m1->next;
  }

  sprintf(env->msg,"End list");
  ufu_log(env);

}

char *ufu_find_msg(struct s_env *env,char *id) {

  int found;
  char *s;
  struct s_msg *m;

  strcpy(env->fun,"ufu_find_msg");
  ufu_wai(env);

  s=NULL;

  found=FALSE;
  m=env->msgfirst;
  while((m!=NULL)&&(!found)) {

    if(strcmp(m->id,id)==0) {
      s=m->msg;
      found=TRUE;
      sprintf(env->msg,"Found ID %s: %s",m->id,m->msg);
      ufu_log(env);
    }

    m=m->next;

  }

  return(s);

}

void ufu_read_msg(struct s_env *env) {

  int seqno;
  char *line1,*line2;
  FILE *fp;
  struct s_msg *m,*tmp_m;

  strcpy(env->fun,"ufu_read_msg");
  ufu_wai(env);

  if((fp=fopen(env->msgglobal,"r"))!=NULL) {

    sprintf(env->msg,"Reading messagefile (%s).",env->msgglobal);
    ufu_log(env);

    seqno=0;
    line1=ufu_alloc_string(env,1024);

    while(fgets(line1,256,fp)!=NULL) {

      line2=strtok(line1,"|");

      if(line2!=NULL) {

        m=ufu_alloc_msg(env);

        m->seqno=seqno;
        strcpy(m->id,line2);

        line2=strtok(NULL,"|");

	if(line2!=NULL) {

          strcpy(m->msg,line2);
          m->msg[strlen(m->msg)-1]='\0';

          sprintf(env->msg," Reading id \"%s\", text \"%s\".",m->id,m->msg);
          ufu_log(env);

          if(seqno>0) {
	    m->prev=tmp_m;
	    m->prev->next=m;
            m->next=NULL;
	    tmp_m=m;
          }
          else {
	    env->msgfirst=m;
	    m->prev=NULL;
            m->next=NULL;
	    tmp_m=m;
          }

          seqno++;

        }

      }

    }

    ufu_free_string(env,line1);

    fclose(fp);

    sprintf(env->msg,"Reading finished.");
    ufu_log(env);

    ufu_sort_msg(env);

  }
  else {

    sprintf(env->msg,"Unable to open messagefile \"%s\"!",UFU_GLOBAL_MSG);
    ufu_log(env);

  }

}

