
#include "ufu.h"
#include "ufu.p"

void ufu_mark_action_menu(struct s_env *env) {

  int again,key,menu_min,menu_max,menu_c,col,i;
  char *menu[]={
    "Move the marked files  ",
    "Copy the marked files  ",
    "Remove the marked files",
    "Unmark the marked files",
    "Exit this pretty menu  "};
  char *r;
  WINDOW *w;

  strcpy(env->fun,"ufu_mark_action_menu");
  ufu_wai(env);

  // Check if there is a remote panel available.
  if(!ufu_mark_xcheck(env)) {
    sprintf(env->msg,"No remote panel(s) available!");
    ufu_alert(env);
  }
  else {
    w=ufu_popup(env,36,7,(env->cols-36)/2,(env->rows-9)/2,"Select action");
    ufu_wrefresh(w);

    r=ufu_alloc_string(env,4);

    menu_min=0;
    menu_max=4;

    menu_c=ufu_find_hist(env,UFU_HIST_MARK_ACTION,NULL);

    col=6;

    again=TRUE;

    while(again) {

      for(i=menu_min;i<=menu_max;i++) {
        if(i==menu_c)  wattron(w,A_REVERSE);
        mvwprintw(w,i+1,col," %s ",menu[i]);
        if(i==menu_c)  wattroff(w,A_REVERSE);
      }

      ufu_wrefresh(w);

      key=ufu_get_key(env,UFU_NO_TEXT,NULL);

      switch(key) {

        case UFU_KEY_QUIT:
          again=FALSE;
          ufu_add_hist(env,UFU_HIST_MARK_ACTION,NULL,menu_c);
          break;

        case UFU_KEY_UP:
          if(menu_c>menu_min)
            menu_c--;
          else
            menu_c=menu_max;
          break;

        case UFU_KEY_DOWN:
          if(menu_c<menu_max)
            menu_c++;
          else
            menu_c=menu_min;
          break;

        case UFU_KEY_SELECT:
        case UFU_KEY_ENTER:
        case UFU_KEY_LEFT:
        case UFU_KEY_RIGHT:
        case UFU_KEY_MARK:
          switch(menu_c) {
            case 0:
              // Move action.
              ufu_mark_action(env,UFU_MARK_ACTION_MOVE);
              again=FALSE;
              ufu_add_hist(env,UFU_HIST_MARK_ACTION,NULL,menu_c);
              break;
            case 1:
              // Copy action.
              ufu_mark_action(env,UFU_MARK_ACTION_COPY);
              again=FALSE;
              ufu_add_hist(env,UFU_HIST_MARK_ACTION,NULL,menu_c);
              break;
            case 2:
              // Remove action.
              ufu_mark_action(env,UFU_MARK_ACTION_REMOVE);
              again=FALSE;
              ufu_add_hist(env,UFU_HIST_MARK_ACTION,NULL,menu_c);
              break;
            case 3:
              // Clear list of marked files.
              env->key_expunge++;
              ufu_expunge_mark(env);
              again=FALSE;
              ufu_add_hist(env,UFU_HIST_MARK_ACTION,NULL,menu_c);
              break;
            case 4:
              // Quit.
              again=FALSE;
              ufu_add_hist(env,UFU_HIST_MARK_ACTION,NULL,menu_c);
              break;

          }

          break;

        case UFU_KEY_HELP:
        case UFU_KEY_F1:
          env->key_help++;
          ufu_help(env,0,UFU_HELP_SELECT_ACTION);
          ufu_wrefresh(w);
          break;

        default:
          ufu_wrong_key(env);
          break;

      }

    }

    ufu_free_string(env,r);

    ufu_popdown(env,w);
    ufu_wrefresh(env->body);

  }

}

int ufu_mark_action_cpmv_menu(struct s_env *env,char action) {

  int again,key,menu_min,menu_max,menu_c,col,i;
  char *menu[]={
    "Absolute path action ",
    "Relative path action ",
    "Exit this pretty menu"};
  char *r,path;
  WINDOW *w;

  strcpy(env->fun,"ufu_mark_action_cpmv_menu");
  ufu_wai(env);

  switch(action) {
    case 'c':
      w=ufu_popup(env,36,5,(env->cols-22)/2,(env->rows-7)/2,"Select path for copying files");
      break;
    case 'm':
      w=ufu_popup(env,36,5,(env->cols-22)/2,(env->rows-7)/2,"Select path for moving files");
      break;
  }

  //ufu_wrefresh(w);

  r=ufu_alloc_string(env,4);

  menu_min=0;
  menu_max=2;

  menu_c=0;

  col=6;

  again=TRUE;

  while(again) {

    for(i=menu_min;i<=menu_max;i++) {
      if(i==menu_c)  wattron(w,A_REVERSE);
      mvwprintw(w,i+1,col," %s ",menu[i]);
      if(i==menu_c)  wattroff(w,A_REVERSE);
    }

    ufu_wrefresh(w);

    key=ufu_get_key(env,UFU_NO_TEXT,NULL);

    switch(key) {

      case UFU_KEY_QUIT:
        path='q';
        again=FALSE;
        break;

      case UFU_KEY_UP:
        if(menu_c>menu_min)
          menu_c--;
        else
          menu_c=menu_max;
        break;

      case UFU_KEY_DOWN:
        if(menu_c<menu_max)
          menu_c++;
        else
          menu_c=menu_min;
        break;

      case UFU_KEY_SELECT:
      case UFU_KEY_ENTER:
      case UFU_KEY_LEFT:
      case UFU_KEY_RIGHT:
      case UFU_KEY_MARK:
        switch(menu_c) {
          case 0:
            // Absolute.
            path='a';
            again=FALSE;
            break;
          case 1:
            // Relative.
            path='r';
            again=FALSE;
            break;
          case 2:
            // Quit.
            path='q';
            again=FALSE;
            break;

        }

        break;

      case UFU_KEY_HELP:
      case UFU_KEY_F1:
        env->key_help++;
        ufu_help(env,0,UFU_HELP_SELECT_PATH);
        ufu_wrefresh(w);
        break;

      default:
        ufu_wrong_key(env);
        break;

    }

  }

  ufu_free_string(env,r);

  ufu_popdown(env,w);
  ufu_wrefresh(env->body);

  return(path);

}

int ufu_mark_action(struct s_env *env,int action) {

  int ok,remove,panel,sel,ar;
  int rem_ok,rem_error;
  char *str_action;
  struct s_mark *m,*m_next;

  strcpy(env->fun,"ufu_mark_action");
  ufu_wai(env);

  ok=TRUE;

  str_action=ufu_alloc_string(env,10);

  switch(action) {
    case UFU_MARK_ACTION_REMOVE:
      sel=TRUE;
      break;
    case UFU_MARK_ACTION_COPY:
      panel=ufu_select_panel(env,0);
      ufu_wrefresh(env->body);
      sel=(panel>=0);
      if(sel) {
        ar=ufu_mark_action_cpmv_menu(env,'c');
        sel=(ar!='q');
      } 
      break;
    case UFU_MARK_ACTION_MOVE:
      panel=ufu_select_panel(env,0);
      sel=(panel>=0);
      if(sel) {
        ar=ufu_mark_action_cpmv_menu(env,'m');
        sel=(ar!='q');
      } 
      break;
  }

  ufu_wrefresh(env->body);

  if(sel) {

    rem_ok=0;
    rem_error=0;

    m=env->mfirst;

    while(m!=NULL) {

      sprintf(env->msg,"Processing entry \"%s\"...",m->entry->fname);
      //ufu_log(env);
      ufu_msg(env);

      remove=FALSE;

      switch(action) {
        case UFU_MARK_ACTION_REMOVE:
          if(ufu_mark_action_unlink(env,m)) {
            rem_ok++;
            remove=TRUE;
            m->entry->status_action=UFU_MARK_ACTION_REMOVE;
            ufu_set_action_status(env,m->dname,m->entry->fname,action);
            sprintf(env->msg,"Entry \"%s\" marked as being removed.",m->entry->fname);
            ufu_log(env);
          }
          else {
            rem_error++;
          }
          break;
        case UFU_MARK_ACTION_COPY:
          if(ufu_mark_action_copy(env,m,panel)) {
            rem_ok++;
            remove=TRUE;
            m->entry->status_action=UFU_MARK_ACTION_COPY;
            ufu_set_action_status(env,m->dname,m->entry->fname,action);
            sprintf(env->msg,"Entry \"%s\" marked as being copied.",m->entry->fname);
            ufu_log(env);
          }
          else {
            rem_error++;
          }
          break;
        case UFU_MARK_ACTION_MOVE:
          if(ufu_mark_action_move(env,m,panel)) {
            rem_ok++;
            remove=TRUE;
            m->entry->status_action=UFU_MARK_ACTION_MOVE;
            ufu_set_action_status(env,m->dname,m->entry->fname,action);
            sprintf(env->msg,"Entry \"%s\" marked as being moved.",m->entry->fname);
            ufu_log(env);
          }
          else {
            rem_error++;
          }
          break;
      }

      if(!remove) {

        // Action failed, continue with next marked entry.
        m=m->next;

      }
      else {

        // Action ok, remove current marked entry from list of entries.
        ufu_mark_action_remove_entry(env,m);
        m_next=m->next;
        ufu_free_mark(env,m);
        m=m_next;

      }

    }

    switch(action) {
      case UFU_MARK_ACTION_REMOVE:
        strcpy(str_action,"removed");
        break;
      case UFU_MARK_ACTION_COPY:
        strcpy(str_action,"copied");
        break;
      case UFU_MARK_ACTION_MOVE:
        strcpy(str_action,"moved");
        break;
    }

    if(rem_error==0) {
      if(rem_ok>1) {
        sprintf(env->msg,"All %d files are %s.",rem_ok,str_action);
      }
      else {
        sprintf(env->msg,"1 file is %s.",str_action);
      }
    }
    else {
      if(rem_error>1) {
        sprintf(env->msg,"%d files (of %d) are NOT %s!",rem_error,rem_ok+rem_error,str_action);
      }
      else {
        sprintf(env->msg,"%d file (of %d) is NOT %s!",rem_error,rem_ok+rem_error,str_action);
      }
    }
    ufu_alert(env);

  }

  ufu_free_string(env,str_action);

  return(ok);

}

void ufu_mark_action_remove_entry(struct s_env *env,struct s_mark *m) {

  struct s_mark *m1,*m2;

  strcpy(env->fun,"ufu_mark_action_remove_entry");
  ufu_wai(env);

  m1=m->prev;
  m2=m->next;

  if(m1!=NULL) {
    m1->next=m2;
    if(m2!=NULL) {
      m2->prev=m1;
    }
    else {
      env->mlast=m1;
    }
  }
  else {
    env->mfirst=m2;
    if(m2!=NULL) {
      m2->prev=NULL;
    }
    else {
      env->mlast=m1;
    }
  }

}

int ufu_mark_action_unlink(struct s_env *env,struct s_mark *m) {

  int ok;
  char *f;
  struct s_buffer *b;

  strcpy(env->fun,"ufu_mark_action_unlink");
  ufu_wai(env);

  f=ufu_alloc_string(env,UFU_LEN_NAME);

  ok=FALSE;

  // Remote entry.

  if(ufu_com_init(env,env->panel[m->panel]->rem)) {

    b=ufu_alloc_buffer(env,UFU_B_UNLINK_ENTRY);
    ufu_crypt_passwd(env,b,env->panel[m->panel]->rem);
    b->action=UFU_B_UNLINK_ENTRY;
    b->last=TRUE;
    b->error=FALSE;
    ufu_clear_string(env,b->dirname,UFU_LEN_NAME);
    ufu_clear_string(env,b->filename,UFU_LEN_NAME);

    if(S_ISDIR(m->entry->fmode)) {
      // Directory.
      strcpy(b->dirname,env->panel[m->panel]->dirname);
      strcpy(f,b->dirname);
    }
    else {
      // File.
      strcpy(b->dirname,env->panel[m->panel]->dirname);
      strcpy(b->filename,m->entry->fname);
      ufu_concat_dir(env,f,b->dirname,b->filename);
    }

    sprintf(env->msg,"Remote file to unlink: %s",f);
    ufu_log(env);

    // Send to UFUd daemon on remote host.
    ok=ufu_send(env,env->panel[m->panel]->rem,b);
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

      if(ufu_recv(env,env->panel[m->panel]->rem,b)) {
        if(!b->error) {
          // Do something with the data....
          sprintf(env->msg,"Unlink succesfull!");
          ufu_log(env);
        }
        else {
          // Send failed, clearing up panel p.
          strcpy(env->msg,b->errmsg);
          ufu_log(env);
        }
      }
      else {
        // Something wrong with transfer,
        // so let's close the connection.
        sprintf(env->msg,"Something wrong with transfer!");
        ufu_alert(env);
      }

      ufu_com_exit(env,env->panel[m->panel]->rem);

    }
    else {
      // Send failed, clearing up panel p.
      ufu_com_exit(env,env->panel[m->panel]->rem);

      sprintf(env->msg,"Send failed!");
      ufu_alert(env);

    }

    ufu_free_buffer(env,b,UFU_B_UNLINK_ENTRY);

  }
  else {
    sprintf(env->msg,"Host %s is not reponding!",env->panel[m->panel]->rem->hostname);
    ufu_log(env);
    ufu_alert(env);
  }

  ufu_free_string(env,f);

  return(ok);

}

int ufu_mark_action_copy(struct s_env *env,struct s_mark *m,int panel) {

/*
 * Well, we have to manage 4 possibilities here:
 *   1 - Local to local.
 *   2 - Local to remote host.
 *   3 - Remote to local host.
 *   4 - Remote to remote host.
*/

  int ok,rem_src,rem_dst;

  strcpy(env->fun,"ufu_mark_action_copy");
  ufu_wai(env);

  ok=FALSE;

  // Source panel a remote panel?
  rem_src=env->panel[m->panel]->remote;

  // Destination panel a remote panel?
  rem_dst=env->panel[panel]->remote;

  if(rem_dst) {




  }
  else {



  }

  return(ok);

}

int ufu_mark_action_move(struct s_env *env,struct s_mark *m,int panel) {

  int ok;

  strcpy(env->fun,"ufu_emark_action_move");
  ufu_wai(env);

  ok=FALSE;


  return(ok);

}

int ufu_mark_xcheck(struct s_env *env) {

  int result,found,total,failed;
  char *nodename,*username,*s1,*s2;
  struct s_mark *m;
  struct s_xcheck *x,*x_tmp;

  strcpy(env->fun,"ufu_mark_xcheck");
  ufu_wai(env);

  result=TRUE;
  x=NULL;

  total=0;
  failed=0;

  nodename=ufu_alloc_string(env,UFU_REM_HOSTNAME);
  username=ufu_alloc_string(env,UFU_REM_USERNAME);

  s1=ufu_alloc_string(env,8);
  s2=ufu_alloc_string(env,UFU_LEN_UTS+UFU_LEN_UNAME);

  sprintf(env->msg,"Crosschecking marked entries...");
  ufu_msg(env);

  m=env->mfirst;
  while(m!=NULL) {

    total++;

    if(!env->panel[m->panel]->remote) {
      strcpy(s1,"local");
      strcpy(nodename,env->nodename);
      strcpy(username,env->uname);
    }
    else {
      strcpy(s1,"remote");
      strcpy(nodename,env->panel[m->panel]->rem->hostname);
      strcpy(username,env->panel[m->panel]->rem->username);
    }

    found=ufu_mark_xcheck_panel(env,m,username,nodename);

    if((found==UFU_X_F_NC)||(found==UFU_X_NF)) {

      sprintf(s2,"%s@%s",username,nodename);
      ufu_show_remote(env,s2);

      found=ufu_mark_xcheck_panel(env,m,username,nodename);

      if((found==UFU_X_F_NC)||(found==UFU_X_NF)) {
        sprintf(env->msg,"(%d) NO REMOTE CONNECTION (%s@%s) for this %s entry \"%s\"!",found,username,nodename,s1,m->entry->fname);
        ufu_log(env);
        result=FALSE;
        failed++;
        break;
      }

    }

    m=m->next;
  }

  if(result) {
    sprintf(env->msg,"XChecked marked entries: %d.",total);
    ufu_log(env);
  }
  else {
    sprintf(env->msg,"XChecked marked entries: %d. Failed: %d.",total,failed);
    ufu_log(env);
  }

  ufu_free_string(env,nodename);
  ufu_free_string(env,username);

  ufu_free_string(env,s1);
  ufu_free_string(env,s2);

  // Clean up the temporary linked list.
  x=env->xfirst;
  while(x!=NULL) {
    x_tmp=x->next;
    sprintf(env->msg,"Releasing remote \"%s@%s\".",x->username,x->nodename);
    ufu_log(env);
    ufu_free_xcheck(env,x);
    x=x_tmp;
  }
  env->xfirst=NULL;

  return(result);

}

int ufu_mark_xcheck_panel(struct s_env *env,struct s_mark *m,char *username,char *nodename) {

  int found,p;
  struct s_xcheck *x;

  strcpy(env->fun,"ufu_mark_xcheck_panel");
  ufu_wai(env);

  found=FALSE;
  p=UFU_MIN_PANEL;
  while((p<=UFU_MAX_PANEL)&&(strlen(env->panel[p]->dirname)>0)&&(!found)) {
    if(env->panel[p]->rem!=NULL) {
      if(strcmp(nodename,env->panel[p]->rem->hostname)==0) {
        if(strcmp(username,env->panel[p]->rem->username)==0) {
          found=TRUE;
          x=env->xfirst;
          while(x!=NULL) {
            if(strcmp(x->username,username)==0) {
              if(strcmp(x->nodename,nodename)==0) {
                x->connected=UFU_X_F_C;
                if(env->debug) {
                  sprintf(env->msg,"%s@%s: Setting connection status (CONNECTED).",username,nodename);
                  ufu_log(env);
                }
                return(x->connected);
              }
            }
            x=x->next;
          }
          if(env->xfirst!=NULL) {
            x=env->xfirst;
            while(x->next!=NULL) {
              x=x->next;
            }
            x->next=ufu_alloc_xcheck(env);
            strcpy(x->next->username,username);
            strcpy(x->next->nodename,nodename);
            x->next->connected=UFU_X_F_C;
            return(x->next->connected);
          }
          else {
            env->xfirst=ufu_alloc_xcheck(env);
            strcpy(env->xfirst->username,username);
            strcpy(env->xfirst->nodename,nodename);
            env->xfirst->connected=UFU_X_F_C;
            return(env->xfirst->connected);
          }
          if(env->debug) {
            sprintf(env->msg,"%s@%s: Adding connection status (CONNECTED).",username,nodename);
            ufu_log(env);
          }
        }
      }
    }
    p++;
  }

  if(!found) {
    if(env->xfirst!=NULL) {
      x=env->xfirst;
      while(x->next!=NULL) {
        x=x->next;
      }
      x->next=ufu_alloc_xcheck(env);
      strcpy(x->next->username,username);
      strcpy(x->next->nodename,nodename);
      x->next->connected=UFU_X_F_NC;
    }
    else {
      env->xfirst=ufu_alloc_xcheck(env);
      strcpy(env->xfirst->username,username);
      strcpy(env->xfirst->nodename,nodename);
      env->xfirst->connected=UFU_X_F_NC;
    }
    if(env->debug) {
      sprintf(env->msg,"%s@%s: Adding connection status (NOT CONNECTED).",username,nodename);
      ufu_log(env);
    }
    return(UFU_X_F_NC);
  }

  sprintf(env->msg,"%s@%s: Setting connection status (NOT FOUND).",username,nodename);
  ufu_log(env);

  return(UFU_X_NF);

}

