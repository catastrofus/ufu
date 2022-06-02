
#include "ufu.h"
#include "ufu.p"

void ufu_open_log(struct s_env *env) {

  env->lognamefp=fopen(env->logname,"w");
  if(env->lognamefp!=NULL) {
    // No buffering.
    setbuf(env->lognamefp,NULL);
    sprintf(env->msg,"Log opened for writing.");
    ufu_log(env);
    sprintf(env->msg,UFU_VERSION);
    ufu_log(env);
    sprintf(env->msg,UFU_BUILDDATE);
    ufu_log(env);
    strcpy(env->fun,"ufu_open_log");
    ufu_wai(env);
  }

}

void ufu_close_log(struct s_env *env) {

  strcpy(env->fun,"ufu_close_log");
  ufu_wai(env);

  if(env->lognamefp!=NULL) {
    sprintf(env->msg,"Log \"%s\" closed for writing.",env->logname);
    ufu_log(env);
    fclose(env->lognamefp);
    if(env->removelog) {
      unlink(env->logname);
    }
  }

}

void ufu_log(struct s_env *env) {

  int year,month,day,hour,min,sec;
  char *tstamp;
  time_t t_result;
  struct tm *tm_result;

  tstamp=ufu_alloc_string(env,32);
  t_result=time(NULL);
  tm_result=localtime(&t_result);

  year=tm_result->tm_year+1900;
  month=tm_result->tm_mon+1;
  day=tm_result->tm_mday;

  hour=tm_result->tm_hour;
  min=tm_result->tm_min;
  sec=tm_result->tm_sec;

  sprintf(tstamp,"%4d%02d%02d%02d%02d%02d",year,month,day,hour,min,sec);

  if(env->lognamefp!=NULL) {
    fprintf(env->lognamefp,"%s: %s\n",tstamp,env->msg);
    fflush(env->lognamefp);
  }

  // ufu_msg(env);

  ufu_free_string(env,tstamp);

}

void ufu_dbg(struct s_env *env) {

  char *s;

  if(env->debug) {

    s=ufu_alloc_string(env,1024);
    if(strlen(env->fun)==0) {
      strcpy(env->fun,"UNKNOWN");
    }
    sprintf(s,"[DEBUG|%s] %s",env->fun,env->msg);
    strcpy(env->msg,s);
    ufu_free_string(env,s);

    ufu_log(env);
    strcpy(env->fun,"");

  }

}

void ufu_wai(struct s_env *env) {

  if(env->debug) {
    if(strlen(env->fun)>0) {
      sprintf(env->msg,"f(%s)",env->fun);
      strcpy(env->fun,"");
    }
    else {
      strcpy(env->msg,"[FUNCTION] Unknown!");
    }
    ufu_log(env);
  }

}

int ufu_read_log(struct s_env *env) {

  int seqno,len_text;
  char *line,*line_ts,*line_text;
  struct s_log *t;
  FILE *fp;

  strcpy(env->fun,"ufu_view_log");
  ufu_wai(env);

  sprintf(env->msg,"Opening logfile \"%s\".",env->logname);
  ufu_log(env);

  len_text=0;
  seqno=0;
  env->lfirst=NULL;

  if((fp=fopen(env->logname,"r"))!=NULL) {

    sprintf(env->msg,"Reading logfile \"%s\".",env->logname);
    ufu_log(env);

    t=env->lfirst;
    line=ufu_alloc_string(env,UFU_MAX_TEXT);

    while(fgets(line,UFU_MAX_TEXT-1,fp)!=NULL) {

      if(t!=NULL) {
        t->next=ufu_alloc_log(env,strlen(line));
        t->next->prev=t;
        t->next->seqno=seqno++;
        strncpy(t->next->ts,line,UFU_LEN_TS);
        t->next->ts[14]='\0';
        strcpy(t->next->text,line+16);
        if(len_text<strlen(t->next->text)) {
          len_text=strlen(t->next->text);
        }
        t=t->next;
      }
      else {
        env->lfirst=ufu_alloc_log(env,strlen(line));
        t=env->lfirst;
        t->seqno=seqno++;
        strncpy(t->ts,line,UFU_LEN_TS);
        t->ts[14]='\0';
        strcpy(t->text,line+16);
        if(len_text<strlen(t->text)) {
          len_text=strlen(t->text);
        }
      }

    }

    ufu_free_string(env,line);

    fclose(fp);

    sprintf(env->msg,"Logfile \"%s\" closed.",env->logname);
    ufu_log(env);

    sprintf(env->msg,"Lines in logfile \"%s\": %d",env->logname,seqno);
    ufu_log(env);

  }
  else {

    sprintf(env->msg,"Failed to open logfile \"%s\"!",env->logname);
    ufu_log(env);

  }

  return(len_text);

}

void ufu_view_log(struct s_env *env) {

  int rem,again,found,redraw,key,len,row,col,rows,seqno,seq_tos,seq_bos,i,r;
  int len_tmp,len_text,show_seqno,show_ts,offset,pos;
  char *search;
  struct s_log *t,*t_tmp,*cos,*tos;

  strcpy(env->fun,"ufu_show_log");
  ufu_wai(env);

  len_text=ufu_read_log(env);

  if(env->lfirst!=NULL) {

    search=ufu_alloc_string(env,UFU_LEN_SEARCH);
    ufu_clear_string(env,search,UFU_LEN_SEARCH);

    tos=env->lfirst;
    cos=env->lfirst;

    t=env->lfirst;

    show_seqno=TRUE;
    show_ts=TRUE;
    rows=env->rows-6;

    redraw=TRUE;
    again=(t!=NULL);
    offset=0;

    while(cos->next!=NULL) {
      cos=cos->next;
    }
    tos=cos;
    i=0;
    while((i<(rows-1))&&(tos->prev!=NULL)) {
      tos=tos->prev;
      i++;
    }

    while(again) {

      if(redraw) {

        ufu_wclear(env->top);
        mvwprintw(env->top,0,0,"[%s] Show log",env->nodename);
        if(offset>0) {
          mvwprintw(env->top,0,env->cols-12,"Offset: %d",offset);
        }

        ufu_wrefresh(env->top);

        ufu_wclear(env->bottom);
        ufu_wrefresh(env->bottom);

        ufu_wclear(env->body);

        pos=0;

        if(show_seqno) {
          // Seqno.
          mvwprintw(env->body,0,pos+1,"   Seqno");
          mvwprintw(env->body,1,pos+1,"--------");
          pos+=9;
        }

        if(show_ts) {
          // TS.
          mvwprintw(env->body,0,pos+1,"Timestamp");
          mvwprintw(env->body,1,pos+1,"--------------");
          pos+=15;
        }

        // Text.
        mvwprintw(env->body,0,pos+1,"Text");
        for(i=pos+1;i<(env->cols-1);i++) {
          mvwprintw(env->body,1,i,"-");
        }

        ufu_wrefresh(env->body);

      }

      t=tos;
      row=1;

      while(row<=rows) {

        if(t!=NULL) {

          if(t==cos)  wattron(env->body,A_REVERSE);

          pos=0;

          if(show_seqno) {
	    // Seqno.
            mvwprintw(env->body,row+1,pos+1,"%8d",t->seqno);
            pos+=9;
          }

          if(show_ts) {
	    // TS.
            mvwprintw(env->body,row+1,pos+1,"%14s",t->ts);
            pos+=15;
          }

          // Text.
          if(strlen(t->text)<(env->cols-pos)) {
            if(offset<(strlen(t->text)-1)) {
              mvwprintw(env->body,row+1,pos+1,"%s",t->text+offset);
              for(i=pos-offset+strlen(t->text);i<(env->cols-1);i++) {
                mvwprintw(env->body,row+1,i," ");
              }
            }
          }
          else {
            if(t==cos)  wattroff(env->body,A_REVERSE);
            mvwprintw(env->body,row+1,pos,"!");
            if(t==cos)  wattron(env->body,A_REVERSE);
            if(offset<(strlen(t->text)-1)) {
              for(i=pos+1;i<(env->cols-1);i++) {
                mvwprintw(env->body,row+1,i,"%c",t->text[i-pos-1+offset]);
              }
            }
          }

          if(t==cos)  wattroff(env->body,A_REVERSE);

          t=t->next;

        }

        row++;

      }

      ufu_wrefresh(env->body);

      key=ufu_get_key(env,UFU_NO_TEXT,NULL);

      switch(key) {

        case UFU_KEY_QUIT:
          env->hist_g=cos->seqno;
          again=FALSE;
          break;

        case UFU_KEY_HELP:
        case UFU_KEY_F1:
          env->key_help++;
          ufu_help(env,env->mpanel+1,UFU_HELP_LOG);
          redraw=TRUE;
          break;

        case UFU_KEY_LEFT:
          env->key_left++;
          if(offset>0) {
            offset--;
          }
          break;

        case UFU_KEY_RIGHT:
          env->key_right++;
          if(offset<(len_text-1)) {
            offset++;
          }
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
          tos=env->lfirst;
          cos=env->lfirst;
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

        case UFU_KEY_1:
          show_seqno=!show_seqno;
          break;
 
        case UFU_KEY_2:
          show_ts=!show_ts;
          break;
 
	case UFU_KEY_SCROLL_DOWN:
	  // Forward search.
	  i=ufu_search_log(env,key,search,cos);
	  if(i>=0) {
	    cos=env->lfirst;
	    while((cos!=NULL)&&(cos->seqno!=i)) {
              cos=cos->next;
	    }
            tos=cos;
            i=0;
            while((i<(rows-1))&&(tos->prev!=NULL)) {
              tos=tos->prev;
              i++;
            }
	  }
          break;

	case UFU_KEY_SCROLL_UP:
	  // Backward search.
	  i=ufu_search_log(env,key,search,cos);
	  if(i>=0) {
	    cos=env->lfirst;
	    while((cos!=NULL)&&(cos->seqno!=i)) {
              cos=cos->next;
	    }
            tos=cos;
            i=0;
            while((i<(rows-1))&&(tos->prev!=NULL)) {
              tos=tos->prev;
              i++;
            }
	  }
	  break;

        default:
          ufu_wrong_key(env);
          break;

      }

    }

    ufu_free_string(env,search);

    // Free log structures.
    t=env->lfirst;
    while(t!=NULL) {
      t_tmp=t->next;
      ufu_free_log(env,t);
      t=t_tmp;
    }

  }

}

int ufu_search_log(struct s_env *env,int key,char *search,struct s_log *e) {

  int seqno,found;
  WINDOW *w;
  struct s_log *tmp_e;

  strcpy(env->fun,"ufu_search_log");
  ufu_wai(env);

  switch(key) {
    case UFU_KEY_SCROLL_DOWN:
      w=ufu_popup(env,UFU_LEN_SEARCH+2,3,(env->cols-UFU_LEN_SEARCH)/2,(env->rows/2)-4,"Forward search");
      ufu_wrefresh(env->bottom);
      ufu_rl(env,w,1,1,TRUE,UFU_LEN_SEARCH,UFU_LEN_SEARCH,search,TRUE,FALSE);
      seqno=e->seqno;
      if(e->next!=NULL) {
	e=e->next;
      }
      else {
	e=env->lfirst;
      }
      found=FALSE;
      while((e!=NULL)&&(e->seqno!=seqno)&&(!found)) {
	found=((strstr(e->text,search)!=NULL)||(strstr(e->ts,search)!=NULL));
	if(!found) {
	  if(e->next!=NULL) {
	    e=e->next;
	  }
	  else {
            e=env->lfirst;
	  }
	}
      }
      if(found) {
	sprintf(env->msg,"Searchstring \"%s\" found at seqno %d.",search,e->seqno);
	ufu_alert(env);
	return(e->seqno);
      }
      else {
        return(-1);
      }
      break;
    case UFU_KEY_SCROLL_UP:
      // Backward.
      w=ufu_popup(env,UFU_LEN_SEARCH+2,3,(env->cols-UFU_LEN_SEARCH)/2,(env->rows/2)-4,"Backward search");
      ufu_wrefresh(env->bottom);
      ufu_rl(env,w,1,1,TRUE,UFU_LEN_SEARCH,UFU_LEN_SEARCH,search,TRUE,FALSE);
      seqno=e->seqno;
      if(e->prev!=NULL) {
	e=e->prev;
      }
      else {
	e=env->lfirst;
	while(e!=NULL) {
	  tmp_e=e;
          e=e->next;
	}
	e=tmp_e;
      }
      found=FALSE;
      while((e!=NULL)&&(e->seqno!=seqno)&&(!found)) {
	found=((strstr(e->text,search)!=NULL)||(strstr(e->ts,search)!=NULL));
	if(!found) {
	  if(e->prev!=NULL) {
	    e=e->prev;
	  }
	  else {
            e=env->lfirst;
	    while(e!=NULL) {
	      tmp_e=e;
              e=e->next;
	    }
	    e=tmp_e;
	  }
	}
      }
      if(found) {
	sprintf(env->msg,"Searchstring \"%s\" found at seqno %d.",search,e->seqno);
	ufu_alert(env);
	seqno=e->seqno;
      }
      else {
        seqno=-1;
      }
      break;
  }

  ufu_popdown(env,w);

  return(seqno);

}

