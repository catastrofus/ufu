
#include "ufu.h"
#include "ufu.p"

int ufu_mark_level(struct s_env *env,char *s) {

  int level;
  char *p;

  strcpy(env->fun,"ufu_mark_level");
  ufu_wai(env);

  if(strlen(s)>0) {
    p=s;
    level=0;
    while((p=strstr(p,"/"))!=NULL) {
      level++;
      p++;
    }
    if(env->debug) {
      sprintf(env->msg,"Level found for \"%s\": %d",s,level);
      ufu_log(env);
    }
    return(--level);
  }
  else {
    return(-1);
  }
}

int ufu_try2mark(struct s_env *env,int panel,struct s_entry *cos) {

  int marked;
  char *abs_name,*dname;

  strcpy(env->fun,"ufu_try2mark");
  ufu_wai(env);

  marked=FALSE;
  sprintf(env->msg,"Marking... ");
  ufu_msg(env);

  switch(ufu_can_mark(env,cos)) {
    case UFU_IS_NOT:
      break;
    case UFU_IS_REG:
        marked=ufu_mark(env,panel,env->panel[panel]->dirname,cos);
      break;
    case UFU_IS_DIR:
      if(env->confirmaction) {
        sprintf(env->msg,"%s, do you wish to mark this humble directory? ",env->master);
        if(ufu_get_y(env)) {
          abs_name=ufu_alloc_string(env,UFU_LEN_NAME);
          if(env->panel[panel]->remote) {
            dname=ufu_alloc_string(env,UFU_LEN_NAME);
            strcpy(dname,env->panel[panel]->dirname);
            ufu_concat_dir(env,abs_name,env->panel[panel]->dirname,cos->fname);
            marked=ufu_mark(env,panel,abs_name,NULL);
            ufu_free_string(env,abs_name);
            ufu_free_entries(env,panel);
            ufu_com_read_dir(env,dname,panel);
            ufu_free_string(env,dname);
          }
          else {
            ufu_concat_dir(env,abs_name,env->panel[panel]->dirname,cos->fname);
            marked=ufu_mark(env,panel,abs_name,NULL);
            ufu_free_string(env,abs_name);
          }
        }
        else {
          abs_name=ufu_alloc_string(env,UFU_LEN_NAME);
          if(env->panel[panel]->remote) {
            dname=ufu_alloc_string(env,UFU_LEN_NAME);
            strcpy(dname,env->panel[panel]->dirname);
            ufu_concat_dir(env,abs_name,env->panel[panel]->dirname,cos->fname);
            marked=ufu_mark(env,panel,abs_name,NULL);
            ufu_free_string(env,abs_name);
            ufu_free_entries(env,panel);
            ufu_com_read_dir(env,dname,panel);
            ufu_free_string(env,dname);
          }
          else {
            ufu_concat_dir(env,abs_name,env->panel[panel]->dirname,cos->fname);
            marked=ufu_mark(env,panel,abs_name,NULL);
            ufu_free_string(env,abs_name);
          }
        }
      }
        break;
  }

  return(marked);

}

int ufu_can_mark(struct s_env *env,struct s_entry *e) {

  int mark;

  strcpy(env->fun,"ufu_can_mark");
  ufu_wai(env);

  mark=UFU_IS_NOT;
  if(e!=NULL) {
    if(strcmp(e->fname,".")!=0) {
      if(strcmp(e->fname,"..")!=0) {
	if(e->is_reg) {
          mark=UFU_IS_REG;
        }
	else {
          if(e->is_dir) {
            if(env->incsubdir) {
              mark=UFU_IS_DIR;
            }
          }
        }
      }
    }
  }

  return(mark);

}

int ufu_mark(struct s_env *env,int panel,char *dname,struct s_entry *entry) {

  int i,marked;
  char *abs_name;

  struct s_entry *e;
  struct s_mark *m,*m2;
  struct dirent *de;

  DIR *dirp;

  strcpy(env->fun,"ufu_mark");
  ufu_wai(env);

  marked=FALSE;

  abs_name=ufu_alloc_string(env,strlen(dname)+strlen(entry->fname)+4);
  ufu_concat_dir(env,abs_name,dname,entry->fname);

  if(entry!=NULL) {
    // Object is a regular file.
    if(!entry->marked) {
      // Entry is not marked.
      if(!ufu_is_marked(env,panel,entry)) {
        // Entry doesn't exists in list of marked entries.

        entry->marked=TRUE;
        marked=TRUE;

        sprintf(env->msg,"Marking file \"%s\".",abs_name);
        ufu_log(env);
        sprintf(env->msg,"Marking file \"%s\" ",abs_name);
        ufu_msg(env);

        m=ufu_alloc_mark(env);
        m->entry=ufu_alloc_entry(env);
        m->entry->fstat=ufu_alloc_stat(env);
        m->entry->lstat=ufu_alloc_stat(env);
        m->panel=panel;
        m->level=ufu_mark_level(env,abs_name);
        strcpy(m->dname,dname);

        ufu_copy_marked(env,m,entry);

        if(env->mfirst==NULL) {
          sprintf(env->msg,"No marked entries present, adding marked entry 0.");
          ufu_log(env);
          m->seqno=0;
          env->mfirst=m;
          env->mlast=m;
          m->prev=NULL;
          m->next=NULL;
        }
        else {
          i=0;
          m2=env->mfirst;
          while(m2->next!=NULL) {
            m2=m2->next;
	    i++;
          }
          sprintf(env->msg,"Adding marked entry %d",i+1);
          ufu_log(env);
          m->seqno=++i;
          m->prev=m2;
          m2->next=m;
          m->next=NULL;
          env->mlast=m;
        } 

	ufu_find_entry_from_mark(env,m,TRUE);

        sprintf(env->msg,"File \"%s\" successfully marked.",abs_name);
        ufu_log(env);

        m=env->mfirst;
        while(m!=NULL) {
          sprintf(env->msg,"[%4d] %s/%s",m->seqno,m->dname,m->entry->fname);
          ufu_log(env);
          m=m->next;
        }

      }
      else {
        // Entry already exists in list of marked entries.

        sprintf(env->msg,"File \"%s\" already exists in the list of marked enries!",abs_name);
        ufu_log(env);

	marked=TRUE;

      }

    }
    else {
      // Entry is already marked.
      entry->marked=FALSE;

      if(ufu_is_marked(env,panel,entry)) {
        ufu_rem_mark(env,NULL,FALSE,dname,entry->fname);
        sprintf(env->msg,"File \"%s\" successfully unmarked.",abs_name);
        ufu_log(env);
      }

      marked=FALSE;

    }
  }
  else {
    // Object is a directory.
    if(env->incsubdir) {
      if(env->panel[panel]->remote) {
        // Remote.




      }
      else {
        if((dirp=opendir(dname))!=NULL) {
          de=ufu_alloc_dirent(env);
          while((de=readdir(dirp))!=NULL) {
            e=ufu_alloc_entry(env);
            ufu_concat_dir(env,e->fname,dname,de->d_name);
            ufu_stat_entry(env,env->mpanel+1,e);
            strcpy(e->fname,de->d_name);
            e->next=NULL;
            e->prev=NULL;
            ufu_concat_dir(env,abs_name,dname,e->fname);
            switch(ufu_can_mark(env,e)) {
              case UFU_IS_NOT:
                // Not a regular file or directory!
                sprintf(env->msg," Ignoring \"%s\"!",abs_name);
                ufu_log(env);
                break;
              case UFU_IS_REG:
                marked=ufu_mark(env,panel,dname,e);
                break;
              case UFU_IS_DIR:
                sprintf(env->msg," Entering directory \"%s\" recursively.",abs_name);
                ufu_log(env);
                marked=ufu_mark(env,panel,abs_name,NULL);
                break;
            }
            if(e!=NULL) {
              ufu_free_entry(env,e);
            }
          }
          ufu_free_dirent(env,de);
          closedir(dirp);
        }
      }
    }

  }

  ufu_free_string(env,abs_name);

  return(marked);

}

void ufu_copy_marked(struct s_env *env,struct s_mark *m, struct s_entry *e) {

  strcpy(env->fun,"ufu_copy_marked");
  ufu_wai(env);

  // Manual copy of entry.
  m->entry->seqno=0;

  m->entry->uid=e->uid;
  m->entry->gid=e->gid;

  m->entry->fatime=e->fatime;
  m->entry->fmtime=e->fmtime;
  m->entry->fctime=e->fctime;

  m->entry->latime=e->latime;
  m->entry->lmtime=e->lmtime;
  m->entry->lctime=e->lctime;

  m->entry->u_read=e->u_read;
  m->entry->u_write=e->u_write;
  m->entry->u_exec=e->u_exec;

  m->entry->g_read=e->g_read;
  m->entry->g_write=e->g_write;
  m->entry->g_exec=e->g_exec;

  m->entry->o_read=e->o_read;
  m->entry->o_write=e->o_write;
  m->entry->o_exec=e->o_exec;

  m->entry->size=e->size;

  m->entry->fmode=e->fmode;
  m->entry->lmode=e->lmode;

  m->entry->blocks=e->blocks;
  m->entry->links=e->links;

  m->entry->is_dir=e->is_dir;
  m->entry->is_lnk=e->is_lnk;

  strcpy(m->entry->fname,e->fname);
  strcpy(m->entry->lname,e->lname);

  strcpy(m->entry->fperm,e->fperm);
  strcpy(m->entry->lperm,e->lperm);

  strcpy(m->entry->fusrname,e->fusrname);
  strcpy(m->entry->fgrpname,e->fgrpname);

  strcpy(m->entry->lusrname,e->lusrname);
  strcpy(m->entry->lgrpname,e->lgrpname);

  m->entry->next=NULL;
  m->entry->prev=NULL;

}

int ufu_rem_mark(struct s_env *env,struct s_mark *m,int rearrange,char *dname,char *fname) {

  int seqno,found;
  char *s1,*s2;
  struct s_mark *m1,*m2;

  strcpy(env->fun,"ufu_rem_mark");
  ufu_wai(env);

  if(m!=NULL) {

    sprintf(env->msg,"Unmarking file \"%s\" ",m->entry->fname);
    ufu_log(env);

    if(m->prev!=NULL) {
      m->prev->next=m->next;
    }
    else {
      env->mfirst=m->next;
    }
    if(m->next!=NULL) {
      m->next->prev=m->prev;
    }
    else {
      env->mlast=m->prev;
    }
    ufu_find_entry_from_mark(env,m,FALSE);
    ufu_free_mark(env,m);
  }
  else {

    s1=ufu_alloc_string(env,strlen(dname)+strlen(fname)+4);
    ufu_concat_dir(env,s1,dname,fname);
    found=FALSE;
    m1=env->mfirst;
    while((m1!=NULL)&&(!found)) {
      s2=ufu_alloc_string(env,strlen(m1->dname)+strlen(m1->entry->fname)+4);
      ufu_concat_dir(env,s2,m1->dname,m1->entry->fname);
      if(strcmp(s1,s2)==0) {
        found=TRUE;
        ufu_find_entry_from_mark(env,m1,FALSE);
	ufu_rem_mark(env,m1,TRUE,NULL,NULL);
      }
      else {
        m1=m1->next;
      }
      ufu_free_string(env,s2);
    }
    ufu_free_string(env,s1);

  }

  if(rearrange) {

    sprintf(env->msg,"Rearranging list of marked entries.");
    ufu_log(env);

    seqno=0;
    m2=env->mfirst;
    while(m2!=NULL) {
      m2->seqno=seqno++;
      m2=m2->next;
    }

    sprintf(env->msg,"Finished rearranging list of marked entries.");
    ufu_log(env);

  }

  return(TRUE);

}

void ufu_rem_mark_from_panel(struct s_env *env,int panel) {

  struct s_mark *m;

  strcpy(env->fun,"ufu_rem_mark_from_panel");
  ufu_wai(env);

  m=env->mfirst;
  while(m!=NULL) {

    if(m->panel==panel) {

      ufu_rem_mark(env,m,FALSE,NULL,NULL);

    }

    m=m->next;

  }

}

void ufu_show_mark(struct s_env *env) {

  int again,redraw,key,row,rows,seqno,seq_tos,seq_bos,i,r,lineno,panel,len,width;
  char *fname,*s,*inp;
  struct s_mark *m,*cos,*tos;

  strcpy(env->fun,"ufu_show_mark");
  ufu_wai(env);

  fname=ufu_alloc_string(env,UFU_LEN_NAME);
  s=ufu_alloc_string(env,UFU_LEN_NAME);

  panel=0;

  width=env->cols;

  rows=env->rows-6;

  tos=env->mfirst;
  cos=env->mfirst;

  seqno=env->hist_mark;
  m=env->mfirst;

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
    m=env->mfirst;
  }

  redraw=TRUE;
  again=(m!=NULL);

  while((again)&&(env->mfirst!=NULL)) {

    if(redraw) {

      ufu_wclear(env->top);
      mvwprintw(env->top,0,0,"[%s] Show marked entries",env->nodename);

      ufu_wrefresh(env->top);

      ufu_wclear(env->bottom);
      ufu_wrefresh(env->bottom);

      ufu_wclear(env->body);
      mvwprintw(env->body,0,1,"SeqNo");
      mvwprintw(env->body,0,7,"Panel");
      mvwprintw(env->body,0,13,"Name");
      mvwprintw(env->body,0,68+(width-UFU_MIN_COLS)+7,"Size");
      mvwprintw(env->body,1,1,"----- -----");
      mvwprintw(env->body,1,61+(width-UFU_MIN_COLS)+7,"-----------");
      for(i=0;i<(54+(width-UFU_MIN_COLS));i++) {
        mvwprintw(env->body,1,13+i,"-");
      }
      ufu_wrefresh(env->body);

    }

    m=tos;
    row=1;

    while(row<=rows) {

      if(m!=NULL) {

        if(m==cos)  wattron(env->body,A_REVERSE);
        mvwprintw(env->body,row+1,1,"%5d",m->seqno);
        mvwprintw(env->body,row+1,7,"%5d",m->panel);

        ufu_concat_dir(env,fname,m->dname,m->entry->fname);

        ufu_resize_string(env,fname,54+(width-UFU_MIN_COLS),s);
        mvwprintw(env->body,row+1,13,"%-s",s);
        for(i=strlen(s);i<(54+(width-UFU_MIN_COLS));i++) {
          mvwprintw(env->body,row+1,13+i," ");
        }

        if(S_ISDIR(m->entry->lmode)) {
          mvwprintw(env->body,row+1,68+(width-UFU_MIN_COLS),"           ");
        }
        else {
          mvwprintw(env->body,row+1,68+(width-UFU_MIN_COLS),"%11d",m->entry->size);
        }

        if(m==cos)  wattroff(env->body,A_REVERSE);
        m=m->next;
      }

      row++;

    }

    ufu_wrefresh(env->body);

    if(strlen(cos->msg)>0)
      key=ufu_get_key(env,UFU_NO_TEXT,cos->msg);
    else
      key=ufu_get_key(env,UFU_NO_TEXT,NULL);

    switch(key) {

      case UFU_KEY_QUIT:
        ufu_add_hist(env,UFU_HIST_MARK,NULL,cos->seqno);
        again=FALSE;
        break;

      case UFU_KEY_HELP:
      case UFU_KEY_F1:
        env->key_help++;
        ufu_help(env,env->mpanel+1,UFU_HELP_MARK);
        redraw=TRUE;
        break;

      case UFU_KEY_INFO:
      case UFU_KEY_F2:
      case UFU_KEY_ENTER:
      case UFU_KEY_SELECT:
        env->key_info++;
        ufu_info_entry(env,cos->entry,cos->panel,cos,NULL,UFU_INFO_MARK);
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
        tos=env->mfirst;
        cos=env->mfirst;
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

      case UFU_KEY_MARK:
        env->key_rem_mark++;
        m=cos;
        // What to do if there is no next entry?
        if(cos->next!=NULL) {
          if(cos->prev==NULL) {
            env->mfirst=cos->next;
          }
          if(tos==cos) {
            tos=cos->next;
          }
          cos=cos->next;
        }
        else {
          // What to do if there is no previous entry?
          if(cos->prev!=NULL) {
            env->mlast=cos->prev;
            cos=cos->prev;
            if(cos->seqno<seq_tos) {
              i=0;
              while((i<rows)&&(tos->prev!=NULL)) {
                tos=tos->prev;
                i++;
              }
            }
          }
          else {
            // No next or previous entry...
            env->mfirst=NULL;
            env->mlast=NULL;
            again=FALSE;
          }
        }
        ufu_rem_mark(env,m,TRUE,NULL,NULL);
        //seq_cos=cos->seqno;
        seq_tos=tos->seqno;
        redraw=TRUE;
        again=(env->mfirst!=NULL);
        break;

      case UFU_KEY_EDIT:
        env->key_edit++;
        if(env->panel[cos->panel]->remote) {
          ufu_com_get_txtfile(env,cos->dname,cos->entry->fname,cos->panel,UFU_EDIT);
        }
        else {
          ufu_cmd_edit(env,cos->dname,cos->entry->fname);
        }
        break;

      case UFU_KEY_VIEW:
        env->key_view++;
        if(env->panel[cos->panel]->remote) {
          ufu_com_get_txtfile(env,cos->dname,cos->entry->fname,cos->panel,UFU_VIEW);
        }
        else {
          ufu_cmd_view(env,cos->dname,cos->entry->fname);
        }
        break;

      case UFU_KEY_GO:
        env->key_go++;
        inp=ufu_alloc_string(env,6);
        sprintf(env->msg,"%s, enter your seqno:",env->master);
        ufu_msg(env);
        ufu_clear_string(env,inp,6);
        ufu_rl2(env,env->bottom,strlen(env->msg)+1,0,5,TRUE,inp);
        lineno=atoi(inp);

        env->m_cos=cos;
        env->m_tos=tos;

        ufu_goto_seqno(env,UFU_SEARCH_MARK,panel,lineno);

        cos=env->m_cos;
        tos=env->m_tos;

        ufu_free_string(env,inp);
        break;

      case UFU_KEY_CONFIG:
        env->key_config++;
        ufu_show_config(env);
        redraw=TRUE;
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

      case UFU_KEY_EXPUNGE:
        ufu_expunge_mark(env);
        break;

      case UFU_KEY_SEARCH:

        env->key_search++;
        sprintf(env->msg,"%s, enter pattern:",env->master);
        ufu_msg(env);
        len=env->cols-strlen(env->msg)-1;
        inp=ufu_alloc_string(env,len);
        ufu_clear_string(env,inp,len);
        ufu_rl2(env,env->bottom,strlen(env->msg)+1,0,len-1,TRUE,inp);

        env->m_cos=cos;
        env->m_tos=tos;

        ufu_goto_fname(env,UFU_SEARCH_MARK,panel,inp);

        cos=env->m_cos;
        tos=env->m_tos;

        ufu_free_string(env,inp);

        break;

      case UFU_KEY_MARK_ACTION:

        env->key_show_mark_action++;
	ufu_not_implemented(env);
        redraw=TRUE;

        break;

      default:
        ufu_wrong_key(env);
        break;

    }

  }

  ufu_free_string(env,fname);
  ufu_free_string(env,s);

}

void ufu_expunge_mark(struct s_env *env) {

  struct s_mark *m,*m_next;

  strcpy(env->fun,"ufu_expunge_mark");
  ufu_wai(env);

  sprintf(env->msg,"%s, do you wish to empty your remarkable list? ",env->master);
  if(ufu_get_y(env)) {
    env->key_expunge++;
    m=env->mfirst;
    while(m!=NULL) {
      m_next=m->next;
      ufu_rem_mark(env,m,FALSE,NULL,NULL);
      m=m_next;
    }
    env->mfirst=NULL;
    env->mlast=NULL;
  } 
}

int ufu_is_marked(struct s_env *env,int panel,struct s_entry *e) {

  int found;
  char *s1,*s2;
  struct s_mark *m;

  strcpy(env->fun,"ufu_is_marked");
  ufu_wai(env);

  found=FALSE;

  s1=ufu_alloc_string(env,strlen(env->panel[panel]->dirname)+strlen(e->fname)+4);

  ufu_concat_dir(env,s1,env->panel[panel]->dirname,e->fname);
  sprintf(env->msg,"Is file \"%s\" present in the list of marked entries?",s1);
  ufu_log(env);
  m=env->mfirst;
  while((m!=NULL)&&(!found)) {
    s2=ufu_alloc_string(env,strlen(m->dname)+strlen(m->entry->fname)+4);
    ufu_concat_dir(env,s2,m->dname,m->entry->fname);
    if(strcmp(s1,s2)==0) {
      found=TRUE;
    }
    ufu_free_string(env,s2);
    m=m->next;
  }

  if(found) {
    sprintf(env->msg,"File \"%s\" found in list of marked entries!",s1);
    ufu_log(env);
  }
  else {
    sprintf(env->msg,"File \"%s\" NOT found in list of marked entries.",s1);
    ufu_log(env);
  }

  ufu_free_string(env,s1);

  return(found);

}

void ufu_find_entry_from_mark(struct s_env *env,struct s_mark *m,int marked) {

  int c,p,found;
  struct s_entry *e;

  strcpy(env->fun,"ufu_find_entry_from_mark");
  ufu_wai(env);

  sprintf(env->msg,"Trying to find entry \"%s\".",m->entry->fname);
  ufu_log(env);

  c=0;
  for(p=UFU_MIN_PANEL;p<=UFU_MAX_PANEL;p++) {
    if(strlen(env->panel[p]->dirname)>0) {
      sprintf(env->msg,"Looking in dir \"%s\".",env->panel[p]->dirname);
      ufu_log(env);
      if(strcmp(m->dname,env->panel[p]->dirname)==0) {
        e=env->panel[p]->first;
	found=FALSE;
	while((e!=NULL)&&(!found)) {
          if(strcmp(e->fname,m->entry->fname)==0) {
            c++;
            sprintf(env->msg,"Found it in panel %d!",p);
            ufu_log(env);
            e->marked=marked;
	    found=TRUE;
	  }
	  e=e->next;
	}
      }
    }
  }

  if(c>1) {
    sprintf(env->msg,"Found entry \"%s\" in %d panels.",m->entry->fname,c);
    ufu_log(env);
  }
  else {
    if(c>0) {
      sprintf(env->msg,"Found entry \"%s\" in 1 panel.",m->entry->fname);
      ufu_log(env);
    }
  }

}

