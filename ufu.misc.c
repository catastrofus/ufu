
#include "ufu.h"
#include "ufu.p"

void ufu_not_implemented(struct s_env *env) {

  WINDOW *w;

  w=ufu_popup(env,39,3,(env->cols-40)/2,(env->rows-8)/2,"Not implemented");

  wattron(w,A_BLINK);
  mvwprintw(w,1,1," This feature is not implemented yet ");
  wattroff(w,A_BLINK);
  ufu_wrefresh(w);

  ufu_any_key(env);

  ufu_popdown(env,w);

}

int ufu_get_arg(int argc,char **argv) {

  int i,debug;

  debug=FALSE;

  // Process comandline arguments, if they exists.
  if(argc>0) {

    i=1;
    while(i<argc) {

      if(strcmp(argv[i],"-d")==0) {
        debug=TRUE;
      }

      if(strcmp(argv[i],"-v")==0) {
        printf("%s: a U)nix F)ile U)tility, %s\n",argv[0],UFU_VERSION);
        printf("usage: %s [-v]|[-d][<dir>[<dir>[...]]]\n",argv[0]);
        printf("where:  -v   -> Show version & usage.\n");
        printf("        -d   -> Turn DEBUG on.\n");
        printf("       <dir> -> Directory to include in panel.\n");
        exit(1);
      }

      i++;

    }

  }

  return(debug);

}


int ufu_get_rows(struct s_env *env) {

  struct winsize ws;

  // strcpy(env->fun,"ufu_get_rows");
  // ufu_wai(env);

  if(ioctl(0,TIOCGWINSZ,&ws)) {
    return(0);
  }
  else {
    return(ws.ws_row);
  }

}

int ufu_get_cols(struct s_env *env) {

  struct winsize ws;

  // strcpy(env->fun,"ufu_get_cols");
  // ufu_wai(env);

  if(ioctl(0,TIOCGWINSZ,&ws)) {
    return(0);
  }
  else {
    return(ws.ws_col);
  }

}

void ufu_cdate(struct s_env *env) {

  time_t result;

  // strcpy(env->fun,"ufu_cdate");
  // ufu_wai(env);

  result=time(NULL);
  strcpy(env->msg,asctime(localtime(&result)));
  env->msg[strlen(env->msg)-1]='\0';

}

void ufu_sdate(struct s_env *env,time_t *timestamp) {

  // strcpy(env->fun,"ufu_sdate");
  // ufu_wai(env);

  strcpy(env->msg,ctime(timestamp));
  env->msg[strlen(env->msg)-1]='\0';

}

void ufu_any_key(struct s_env *env) {

  // strcpy(env->fun,"ufu_any_key");
  // ufu_wai(env);

  ufu_wclear(env->bottom);
  mvwprintw(env->bottom,0,0,"%s, will you give me your any key? ",env->master);
  ufu_wrefresh(env->bottom);

  wgetch(env->bottom);

}

void ufu_wrong_key(struct s_env *env) {

  strcpy(env->fun,"ufu_wrong_key");
  ufu_wai(env);

  ufu_wclear(env->bottom);
  mvwprintw(env->bottom,0,0,"%s, this isn't and will never be a valid choice! ",env->master);
  ufu_wrefresh(env->bottom);
  wgetch(env->bottom);

}

int ufu_get_key(struct s_env *env,int textline,char *s) {

  strcpy(env->fun,"ufu_get_key");
  ufu_wai(env);

  ufu_wclear(env->bottom);
  switch(textline) {
    case UFU_NO_TEXT:
      if(s!=NULL) {
        mvwprintw(env->bottom,0,0,"%s ",s);
      }
      else {
        mvwprintw(env->bottom,0,0,"%s, your wish is my command: ",env->master);
      }
      break;
    case UFU_IS_MARKED:
      mvwprintw(env->bottom,0,0,"%s, your (marked) wish is my command: ",env->master);
      break;
    case UFU_IS_EXECUTED:
      mvwprintw(env->bottom,0,0,"%s, your (executed) wish is my command: ",env->master);
      break;
    case UFU_IS_REMOTE:
      mvwprintw(env->bottom,0,0,"%s, your (remote) wish is my command: ",env->master);
      break;
    case UFU_MAX_TO_MARK:
      mvwprintw(env->bottom,0,0,"%s, too many (>%d) marked files: ",env->master,env->max_to_mark);
      break;
  }
  ufu_wrefresh(env->bottom);

  return(tolower(wgetch(env->bottom)));

}

int ufu_get_yn(struct s_env *env) {

  // strcpy(env->fun,"ufu_get_yn");
  // ufu_wai(env);

  ufu_wclear(env->bottom);
  mvwprintw(env->bottom,0,0,"%s",env->msg);
  ufu_wrefresh(env->bottom);

  return(tolower(wgetch(env->bottom)));

}

int ufu_get_y(struct s_env *env) {

  // strcpy(env->fun,"ufu_get_y");
  // ufu_wai(env);

  ufu_wclear(env->bottom);
  mvwprintw(env->bottom,0,0,"%s",env->msg);
  ufu_wrefresh(env->bottom);

  return(tolower(wgetch(env->bottom))=='y');

}

void ufu_msg(struct s_env *env) {

  // strcpy(env->fun,"ufu_msg");
  // ufu_wai(env);

  if(strlen(env->msg)>0) {
    ufu_wclear(env->bottom);
    mvwprintw(env->bottom,0,0,env->msg);
    ufu_wrefresh(env->bottom);
  }

}

void ufu_alert(struct s_env *env) {

  // strcpy(env->fun,"ufu_alert");
  // ufu_wai(env);

  sprintf(env->msg,"%s Any key? ",env->msg);
  ufu_wclear(env->bottom);
  mvwprintw(env->bottom,0,0,env->msg);
  ufu_wrefresh(env->bottom);
  wgetch(env->bottom);

}

char *ufu_concat_dir(struct s_env *env,char *name,char *dname,char *fname) {

  // strcpy(env->fun,"ufu_concat_dir");
  // ufu_wai(env);

  if(name!=NULL) {
    strcpy(name,dname);
    if(strlen(name)>1) {
      strcat(name,"/");
    }
    strcat(name,fname);
  }

  return(name);

}

int ufu_display_header(struct s_env *env) {

  int hdrdisplay;

  strcpy(env->fun,"ufu_display_header");
  ufu_wai(env);

  if(env->cols<=80) {
    // Display just usr/group.
    hdrdisplay=1;
  }
  else {
    if(env->cols<=(80+25)) {
      // Display user/group + date of last access.
      hdrdisplay=2;
    }
    else {
      if(env->cols<=(80+25+25)) {
        // Display user/group + date of last access & date of last modification.
        hdrdisplay=3;
      }
      else {
        if(env->cols<=(80+25+25+25)) {
          // Display user/group + date of last access & date of last modification & date of last change.
          hdrdisplay=4;
        }
      }
    }
  }

  return(hdrdisplay);

}

int ufu_exec_ext(struct s_env *env,int panel,char *cmd,char *dir,char *file) {

  int r,c,len,seqno;
  char *s,*line;
  FILE *p;
  struct s_txt *t;

  strcpy(env->fun,"ufu_exec_ext");
  ufu_wai(env);

  seqno=0;

  len=strlen(env->panel[panel]->dirname)+strlen(cmd);
  if(dir!=NULL)  len+=strlen(dir);
  if(file!=NULL) len+=strlen(file);
  len+=24;

  line=ufu_alloc_string(env,len);
  s=ufu_alloc_string(env,256);

  strcpy(line,"cd ");
  strcat(line,env->panel[panel]->dirname);
  strcat(line,"; ");
  strcat(line,cmd);

  if(dir!=NULL) {
    strcat(line," ");
    strcat(line,dir);
    if(file!=NULL) {
      if(strcmp(dir,"/")!=0) {
        strcat(line,"/");
      }
      strcat(line,file);
    }
  }

  sprintf(env->msg,"Command to execute: %s.",line);
  ufu_log(env);

  r=1;
  c=1;

  p=popen(line,"r");
  if(p!=NULL) {
    while((fgets(s,250,p))!=NULL) {
      s[strlen(s)-1]='\0';
      strcpy(env->msg,s);
      ufu_log(env);
      t=env->tfirst;
      if(t!=NULL) {
	while(t->next!=NULL) {
	  t=t->next;
	}
	t->next=ufu_alloc_txt(env);
	t->next->prev=t;
	t=t->next;
	t->seqno=seqno++;
	strcpy(t->line,s);
      }
      else {
        env->tfirst=ufu_alloc_txt(env);
	env->tfirst->seqno=seqno++;
	strcpy(env->tfirst->line,s);
      }
    }
    pclose(p);
  }
  else {
    sprintf(env->msg,"Failed to run command \"%s\"",line);
    ufu_log(env);
  }

  sprintf(env->msg,"Finished command \"%s\".",line);
  ufu_log(env);

  // Show output.
  ufu_show_txt(env,panel,cmd);

  // Removing list.
  t=env->tfirst;
  while(t!=NULL) {
    if(t->next!=NULL) {
      t=t->next;
      ufu_free_txt(env,t->prev);
    }
    else {
      ufu_free_txt(env,t);
      t=NULL;
    }
  }
  env->tfirst=NULL;

  ufu_free_string(env,line);
  ufu_free_string(env,s);

  return(0);

}

char *ufu_clear_string(struct s_env *env,char *str,int len) {

  int i;

  // strcpy(env->fun,"ufu_clear_string");
  // ufu_wai(env);

  // Init parameter 'str' with \0.
  for(i=0;i<len;i++) {
    str[i]='\0';
  }

  return(str);

}

void ufu_display_size(struct s_env *env,long int size,int len,char *s) {

  // strcpy(env->fun,"ufu_display_size");
  // ufu_wai(env);


}

void ufu_resize_string(struct s_env *env,char *str,int len,char *s) {

  int i,sp;

  strcpy(env->fun,"ufu_resize_string");
  ufu_wai(env);

  if(env->debug) {
    sprintf(env->msg,"Resizing string \"%s\" to length %d",str,strlen(str));
    ufu_log(env);
  }

  sp=20;

  if(strlen(str)>len) {

    ufu_clear_string(env,s,len);

    if(len>sp) {

      for(i=0;i<8;i++) {
        s[i]=str[i];
      }
      strcat(s,"...");
      strcat(s,str+(strlen(str)-len+11));

    }
    else {
      strcpy(s,str+(len-sp));
    }

  }
  else {
    strcpy(s,str);
  }

  if(env->debug) {
    sprintf(env->msg,"Old string: %s, length: %d",str,strlen(str));
    ufu_log(env);
    sprintf(env->msg,"New string: %s, length: %d",s,strlen(s));
    ufu_log(env);
  }

}

void ufu_goto_seqno(struct s_env *env,char what,int panel,int seqno) {

  int r;
  struct s_entry *e,*e_cos,*e_tos;
  struct s_mark *m,*m_cos,*m_tos;

  strcpy(env->fun,"ufu_goto_seqno");
  ufu_wai(env);

  switch(what) {
    case UFU_HIST_ENTRY:
      // Find the wanted seqno.
      e=env->panel[panel]->first;
      while((e!=NULL)&&(e->seqno!=seqno)) {
        e=e->next;
      }

      if(e!=NULL) {
        // Ok, seqno found.
        // Now trying to determine the top-of-screen (tos) entry.
        sprintf(env->msg,"Found ENTRY for seqno %d (%s)",seqno,e->fname);
        ufu_log(env);

        e_cos=e;
        r=env->rows/2;
        while((e->prev!=NULL)&&(r>0)) {
          r--;
          e=e->prev;
        }
        if(e!=NULL) {
          //sprintf(env->msg,"Found TOS with seqno %d (%s)",e->seqno,e->fname);
          //ufu_log(env);
          e_tos=e;
        }
        else {
          //sprintf(env->msg,"No TOS found. Seqno=%d (%s)",e_cos->seqno,e_cos->fname);
          //ufu_log(env);
          e_tos=e_cos;
        }
        env->panel[panel]->cos=e_cos;
        env->panel[panel]->tos=e_tos;
      }
      break;

    case UFU_HIST_PANEL:
      // Not used (for now).
      break;

    case UFU_HIST_MARK:
      m=env->mfirst;
      while((m!=NULL)&&(m->seqno!=seqno)) {
        m=m->next;
      }

      //env->m_cos=NULL;
      //env->m_tos=NULL;

      if(m!=NULL) {

        m_cos=m;
        r=env->rows/2;
        while((m->prev!=NULL)&&(r>0)) {
          r--;
          m=m->prev;
        }
        if(m!=NULL) {
          m_tos=m;
        }
        else {
          m_tos=m_cos;
        }
        env->m_cos=m_cos;
        env->m_tos=m_tos;

      }

      break;

  }

}

void ufu_goto_fname(struct s_env *env,char what,int panel,char *pattern) {

  int r,found,len;
  char *s;
  struct s_entry *e,*e_cos,*e_tos;
  struct s_mark *m,*m_cos,*m_tos;

  strcpy(env->fun,"ufu_goto_fname");
  ufu_wai(env);

  switch(what) {
    case UFU_SEARCH_ENTRY:
      // Find the wanted seqno.
      e=env->panel[panel]->cos;
      found=FALSE;
      while((e!=NULL)&&(!found)) {
        len=strlen(e->fname)+1;
        s=ufu_alloc_string(env,len);
        strcpy(s,e->fname);
        found=(strstr(ufu_lower(env,s),pattern)!=NULL);
        ufu_free_string(env,s);
        if(!found) {
          e=e->next;
        }
      }

      if(found) {
        // Ok, seqno found.
        // Now trying to determine the top-of-screen (tos) entry.
        sprintf(env->msg,"Found ENTRY for pattern %s (%s)",s,e->fname);
        ufu_log(env);

        e_cos=e;
        r=env->rows/2;
        while((e->prev!=NULL)&&(r>0)) {
          r--;
          e=e->prev;
        }
        if(e!=NULL) {
          //sprintf(env->msg,"Found TOS with seqno %d (%s)",e->seqno,e->fname);
          //ufu_log(env);
          e_tos=e;
        }
        else {
          //sprintf(env->msg,"No TOS found. Seqno=%d (%s)",e_cos->seqno,e_cos->fname);
          //ufu_log(env);
          e_tos=e_cos;
        }
        env->panel[panel]->cos=e_cos;
        env->panel[panel]->tos=e_tos;
      }
      else {
        sprintf(env->msg,"No entry for pattern %s",s);
        ufu_log(env);
      }
      break;

    case UFU_SEARCH_PANEL:
      // Not used (for now).
      break;

    case UFU_SEARCH_MARK:
      // Find the wanted pattern.
      m=env->m_cos;
      found=FALSE;
      while((m!=NULL)&&(!found)) {
        len=strlen(m->entry->fname)+1;
        s=ufu_alloc_string(env,len);
        strcpy(s,m->entry->fname);
        found=(strstr(ufu_lower(env,s),pattern)!=NULL);
        ufu_free_string(env,s);
        if(!found) {
          m=m->next;
        }
      }

      if(found) {

        m_cos=m;
        r=env->rows/2;
        while((m->prev!=NULL)&&(r>0)) {
          r--;
          m=m->prev;
        }
        if(m!=NULL) {
          m_tos=m;
        }
        else {
          m_tos=m_cos;
        }
        env->m_cos=m_cos;
        env->m_tos=m_tos;

      }

      break;

  }

}

char *ufu_lower(struct s_env *env,char *s) {

  int i;

  // strcpy(env->fun,"ufu_lower");
  // ufu_wai(env);

  i=0;
  while(s[i]!='\0') {
    s[i]=tolower(s[i]);
    i++;
  }

  return(s);

}

int ufu_direxist(struct s_env *env,char *dirname) {

  int ok;
  DIR *dirp;

  strcpy(env->fun,"ufu_direxist");
  ufu_wai(env);

  ok=FALSE;

  if(strlen(dirname)>0) {

    if((dirp=opendir(dirname))!=NULL) {

      ok=TRUE;

      closedir(dirp);

    }

  }

  return(ok);

}

int ufu_set_action_status(struct s_env *env,char *dname,char *fname,int status) {

  int c,found,founde;
  struct s_panel *p;
  struct s_entry *e;

  strcpy(env->fun,"ufu_set_action_status");
  ufu_wai(env);

  found=0;

  sprintf(env->msg,"Finding entry \"%s\":",fname);
  ufu_log(env);

  for(c=UFU_MIN_PANEL;c<=UFU_MAX_PANEL;c++) {

    p=env->panel[c];
    if((p!=NULL)&&(strlen(p->dirname)>0)) {
      if(strcmp(p->dirname,dname)==0) {
        founde=0;
        e=p->first;
        while(e!=NULL) {
          if(strcmp(e->fname,fname)==0) {
            e->status_action=status;
            found++;
            founde++;
          }
          e=e->next;
        }
        sprintf(env->msg," Panel #%d (%s): found %d",c,p->dirname,founde);
        ufu_log(env);
      }
    }
  }

  sprintf(env->msg," All panels: found %d",found);
  ufu_log(env);

  return(found);

}

