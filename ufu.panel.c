
#include "ufu.h"
#include "ufu.p"

void ufu_read_panels(struct s_env *env) {

  int p;

  strcpy(env->fun,"ufu_read_panels");
  ufu_wai(env);

  sprintf(env->msg,"Start reading panels.");
  ufu_log(env);

  for(p=UFU_MIN_PANEL;p<=env->mpanel;p++) {
    if(ufu_read_entries(env,p)) {
      ufu_sort(env,p);
    }
  }

  sprintf(env->msg,"End of reading panels.");
  ufu_log(env);

}

int ufu_show_panels(struct s_env *env,int panel) {

  int redraw,again,changed,key,tmp_panel,i,p,max_p,used_p,tos,height,width,in_use;
  char *s;
  struct s_entry *e;
  struct s_mark *m;

  strcpy(env->fun,"ufu_show_panels");
  ufu_wai(env);

  s=ufu_alloc_string(env,UFU_LEN_NAME);

  tmp_panel=panel;
  height=env->rows-6;
  width=env->cols;
  
  redraw=TRUE;
  again=TRUE;
  changed=FALSE;

  tos=panel-(height/2);
  if(tos<UFU_MIN_PANEL) {
    tos=UFU_MIN_PANEL;
  };

  // p=UFU_MIN_PANEL;
  // while((p<=env->mpanel)&&(strlen(env->panel[p]->dirname)>0)) {
  //   max_p=p;
  //   p++;
  // }
  max_p=UFU_MAX_PANEL;

  panel=ufu_find_hist(env,UFU_HIST_PANEL,NULL);

  while(again) {

    used_p=0;
    p=UFU_MIN_PANEL;
    while(p<=UFU_MAX_PANEL) {
      if(strlen(env->panel[p]->dirname)>0) {
        used_p++;
      }
      p++;
    }

    if(redraw) {

      ufu_wclear(env->top);
      mvwprintw(env->top,0,0,"[%s] Choose panel. Total %d panels, %d used, %d unused",env->nodename,UFU_MIN_PANEL+1,used_p,UFU_MAX_PANEL+1-used_p);
      ufu_wrefresh(env->top);
  
      ufu_wclear(env->bottom);
      ufu_wrefresh(env->bottom);

      ufu_wclear(env->body);
      mvwprintw(env->body,0,1,"No");
      mvwprintw(env->body,0,4,"?");
      mvwprintw(env->body,0,6,"Node");
      mvwprintw(env->body,0,19,"Directory");
      mvwprintw(env->body,1,1,"--");
      mvwprintw(env->body,1,4,"-");
      mvwprintw(env->body,1,6,"------------");

      mvwprintw(env->body,0,65+(width-UFU_MIN_COLS),"Files");
      mvwprintw(env->body,0,71+(width-UFU_MIN_COLS),"Dirs");
      mvwprintw(env->body,0,76+(width-UFU_MIN_COLS),"HCO");
      for(i=0;i<(45+(width-UFU_MIN_COLS));i++) {
        mvwprintw(env->body,1,19+i,"-");
      }
      mvwprintw(env->body,1,65+(width-UFU_MIN_COLS),"-----");
      mvwprintw(env->body,1,71+(width-UFU_MIN_COLS),"----");
      mvwprintw(env->body,1,76+(width-UFU_MIN_COLS),"---");

      ufu_wrefresh(env->body);

      redraw=FALSE;

    }

    for(p=tos;p<(tos+height);p++) {

      if(p<=max_p) {
        if(p==panel)  wattron(env->body,A_REVERSE);
        mvwprintw(env->body,p-tos+2,1,"%2d",p);

        if(env->panel[p]->local)
          mvwprintw(env->body,p-tos+2,4,"L");
        else
          mvwprintw(env->body,p-tos+2,4,"G");

        if(strlen(env->panel[p]->dirname)>0) {
          if(env->panel[p]->remote)
            mvwprintw(env->body,p-tos+2,6,"%-12s",env->panel[p]->nodename);
          else
            mvwprintw(env->body,p-tos+2,6,"[local node]");


          ufu_resize_string(env,env->panel[p]->dirname,45+(width-UFU_MIN_COLS),s);
          mvwprintw(env->body,p-tos+2,19,"%s",s);
          for(i=strlen(s);i<(45+(width-UFU_MIN_COLS));i++) {
            mvwprintw(env->body,p-tos+2,19+i," ");
          }
          mvwprintw(env->body,p-tos+2,65+(width-UFU_MIN_COLS),"%5d",env->panel[p]->nfiles);
          mvwprintw(env->body,p-tos+2,71+(width-UFU_MIN_COLS),"%4d",env->panel[p]->ndirs);
          switch(env->panel[p]->header) {
            case UFU_HDR_STD:
              mvwprintw(env->body,p-tos+2,76+(width-UFU_MIN_COLS),"S");
              break;
            case UFU_HDR_LA:
              mvwprintw(env->body,p-tos+2,76+(width-UFU_MIN_COLS),"A");
              break;
            case UFU_HDR_LM:
              mvwprintw(env->body,p-tos+2,76+(width-UFU_MIN_COLS),"M");
              break;
            case UFU_HDR_LC:
              mvwprintw(env->body,p-tos+2,76+(width-UFU_MIN_COLS),"C");
              break;
          }
          switch(env->panel[p]->sortc) {
            case UFU_SORT_FNAME:
              mvwprintw(env->body,p-tos+2,77+(width-UFU_MIN_COLS),"F");
              break;
            case UFU_SORT_SIZE:
              mvwprintw(env->body,p-tos+2,77+(width-UFU_MIN_COLS),"S");
              break;
            case UFU_SORT_LA:
              mvwprintw(env->body,p-tos+2,77+(width-UFU_MIN_COLS),"A");
              break;
            case UFU_SORT_LM:
              mvwprintw(env->body,p-tos+2,77+(width-UFU_MIN_COLS),"M");
              break;
            case UFU_SORT_LC:
              mvwprintw(env->body,p-tos+2,77+(width-UFU_MIN_COLS),"C");
              break;
          }
          switch(env->panel[p]->sorto) {
            case UFU_SORT_ASC:
              mvwprintw(env->body,p-tos+2,78+(width-UFU_MIN_COLS),"A");
              break;
            case UFU_SORT_DSC:
              mvwprintw(env->body,p-tos+2,78+(width-UFU_MIN_COLS),"D");
              break;
          }
        }
        else {
          ufu_resize_string(env,"[ not used ]",45+(width-UFU_MIN_COLS),s);
          mvwprintw(env->body,p-tos+2,19,"%s",s);
          for(i=strlen(s);i<(45+(width-UFU_MIN_COLS));i++) {
            mvwprintw(env->body,p-tos+2,19+i," ");
          }
          mvwprintw(env->body,p-tos+2,4," ");
          // mvwprintw(env->body,p+2,19,"[ not used ]");
        }
        if(p==panel) {
          wattroff(env->body,A_REVERSE);
        }
      }

    }

    ufu_wrefresh(env->body);

    key=ufu_get_key(env,UFU_NO_TEXT,NULL);

    switch(key) {

      case UFU_KEY_QUIT:
      case UFU_KEY_PANEL:
        ufu_add_hist(env,UFU_HIST_PANEL,NULL,panel);
        panel=tmp_panel;
        again=FALSE;
        if(changed) {
          ufu_write_config(env);
        }
        break;

      case UFU_KEY_HELP:
      case UFU_KEY_F1:
        env->key_help++;
        ufu_help(env,panel,UFU_HELP_PANEL);
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

      case UFU_KEY_LOG:
        env->key_view++;
	ufu_view_log(env);
        redraw=TRUE;
        break;

      case UFU_KEY_0:
      case UFU_KEY_1:
      case UFU_KEY_2:
      case UFU_KEY_3:
      case UFU_KEY_4:
      case UFU_KEY_5:
      case UFU_KEY_6:
      case UFU_KEY_7:
      case UFU_KEY_8:
      case UFU_KEY_9:
        env->key_select++;
	panel=key-'0';
        if(strlen(env->panel[panel]->dirname)>0) {
          again=FALSE;
          ufu_add_hist(env,UFU_HIST_PANEL,NULL,panel);
        }
        break;

      case UFU_KEY_SELECT:
        env->key_select++;
        if(strlen(env->panel[panel]->dirname)>0) {
          again=FALSE;
          ufu_add_hist(env,UFU_HIST_PANEL,NULL,panel);
        }
        break;

      case UFU_KEY_UP:
        env->key_up++;
        if(panel>UFU_MIN_PANEL) {
          panel--;
          if(panel<tos) {
            tos-=height;
            if(tos<UFU_MIN_PANEL) {
              tos=UFU_MIN_PANEL;
            }
          }
          redraw=TRUE;
        }
        break;

      case UFU_KEY_DOWN:
        env->key_down++;
        if(panel<max_p) {
          panel++;
          if(panel>(tos+height-1)) {
            tos=tos+height;
            if(tos>max_p) {
              tos=max_p;
            }
          }
          redraw=TRUE;
        }
        break;

      case UFU_KEY_PREVPAGE:
        env->key_prev_page++;
        panel=panel-height;
        if(panel<UFU_MIN_PANEL)  panel=UFU_MIN_PANEL;
        tos=tos-height;
        if(tos<UFU_MIN_PANEL)    tos=UFU_MIN_PANEL;
        break;

      case UFU_KEY_NEXTPAGE:
        env->key_next_page++;
        panel=panel+height;
        if(panel>max_p) {
          panel=max_p;
        }
        tos=tos+height;
        if(tos>max_p) {
          tos=max_p-height+1;
          if(tos<UFU_MIN_PANEL) {
            tos=UFU_MIN_PANEL;
          }
        }
        break;

      case UFU_KEY_FIRST:
      case UFU_KEY_HOME:
        env->key_first++;
        panel=UFU_MIN_PANEL;
        tos=UFU_MIN_PANEL;
        redraw=TRUE;
        break;

      case UFU_KEY_LAST:
      case UFU_KEY_END:
        env->key_last++;
        panel=max_p;
        tos=max_p-height+1;
        if(tos<UFU_MIN_PANEL) {
          tos=UFU_MIN_PANEL;
        }
        redraw=TRUE;
        break;

      case UFU_KEY_APPEND:
        env->key_add_panel++;
        // Only append non-remote panels!
        if(!(env->panel[panel]->remote)) {
          if(strlen(env->panel[panel]->dirname)==0) {
            if(panel>0) {
              strcpy(env->panel[panel]->dirname,env->panel[UFU_MIN_PANEL]->dirname);
              env->panel[panel]->header=env->panel[UFU_MIN_PANEL]->header;
              ufu_read_entries(env,panel);
            }
            changed=TRUE;
          }
        }
        else {
          sprintf(env->msg,"Can't copy a remote panel!");
          ufu_alert(env);
        }
        redraw=TRUE;
        break;

      case UFU_KEY_ERASE:
        env->key_rem_panel++;
        // Panel #0 (HOMEDIR) can't be removed.
        if(panel>0) {

          in_use=0;
          m=env->mfirst;
          while(m!=NULL) {
            if(m->panel==panel) {
              in_use++;
            }
            m=m->next;
          }

          if(in_use>0) {
            sprintf(env->msg,"Can't delete panel #%d! Marked entries for this panel: %d.",panel,in_use);
            ufu_alert(env);
          }
          else {
            if(strlen(env->panel[panel]->dirname)>0) {
              if(env->confirmremove) {
                sprintf(env->msg,"Delete panel #%d (%s). Are you sure? ",panel,env->panel[panel]->dirname);
              }
              if((!env->confirmremove)||(ufu_get_y(env))) {
                ufu_rem_mark_from_panel(env,panel);
                ufu_free_panel(env,panel);
                env->panel[panel]=ufu_alloc_panel(env,panel);
                // for(p=panel;p<max_p;p++) {
                //   env->panel[p]=env->panel[p+1];
                // }
                // env->panel[max_p]=ufu_alloc_panel(env,max_p);
                // p=UFU_MIN_PANEL;
                // while((p<=env->mpanel)&&(strlen(env->panel[p]->dirname)>0)) {
                //   max_p=p;
                //   p++;
                // }
                // if(panel>max_p) {
                //   panel=max_p;
                // }
              }
              changed=TRUE;
            }
          }
        }
        else {
          sprintf(env->msg,"Can't remove a fixed panel!");
          ufu_alert(env);
        }
        redraw=TRUE;
        break;

      case UFU_KEY_INFO:
        env->key_info++;
        if(env->panel[panel]->remote) {
          e=ufu_alloc_entry(env);
          ufu_com_read_entry(env,env->panel[panel]->dirname,panel,e);
          ufu_info_entry(env,e,panel,NULL,env->panel[panel],UFU_INFO_PANEL);
          ufu_free_entry(env,e);
        }
        else {
          ufu_stat_entry(env,env->mpanel+1,env->panel[panel]->info);
          ufu_info_entry(env,env->panel[panel]->info,panel,NULL,env->panel[panel],UFU_INFO_PANEL);
        }
        redraw=TRUE;
        break;

      case UFU_KEY_MARKED:
        env->key_show_mark++;
        ufu_show_mark(env);
        redraw=TRUE;
        break;

      case UFU_KEY_MARK:
        env->key_add_mark++;
        ufu_try2mark(env,panel,NULL);
        break;

      case UFU_KEY_REMOTE:
        ufu_show_remote(env,FALSE);
        redraw=TRUE;
        // Recount panels.
        p=UFU_MIN_PANEL;
        while((p<=env->mpanel)&&(strlen(env->panel[p]->dirname)>0)) {
          max_p=p;
          p++;
        }
        break;

      default:
        ufu_wrong_key(env);
        break;
    }

  }

  ufu_free_string(env,s);

  if(panel>max_p) {
    panel=max_p;
    sprintf(env->msg,"Adjusting current panel to %d (%s).",panel,env->panel[panel]->dirname);
    ufu_log(env);
  }

  return(panel);

}

int ufu_select_panel(struct s_env *env,int panel) {

  int redraw,again,key,p,max_p,tos,height;
  char *s;
  WINDOW *w;

  strcpy(env->fun,"ufu_select_panel");
  ufu_wai(env);

  s=ufu_alloc_string(env,UFU_LEN_NAME);

  height=env->rows-6;
  
  redraw=TRUE;
  again=TRUE;

  tos=panel-(height/2);
  if(tos<UFU_MIN_PANEL) {
    tos=UFU_MIN_PANEL;
  };

  p=UFU_MIN_PANEL;
  while((p<=env->mpanel)&&(strlen(env->panel[p]->dirname)>0)) {
    max_p=p;
    p++;
  }

  panel=ufu_find_hist(env,UFU_HIST_PANEL,NULL);

  w=ufu_popup(env,65,15,(env->cols-70)/2,(env->rows-14)/2,"Select panel");
  ufu_wrefresh(w);

  while(again) {

    if(redraw) {

      ufu_wrefresh(w);

      mvwprintw(w,1,1,"No");
      mvwprintw(w,1,4,"?");
      mvwprintw(w,1,6,"Node");
      mvwprintw(w,1,19,"Directory");
      mvwprintw(w,2,1,"--");
      mvwprintw(w,2,4,"-");
      mvwprintw(w,2,6,"------------");
      mvwprintw(w,2,19,"---------------------------------------------");

      ufu_wrefresh(w);

      redraw=FALSE;

    }

    for(p=tos;p<(tos+height);p++) {

      if(p<=max_p) {
        if(p==panel)  wattron(w,A_REVERSE);
        mvwprintw(w,p-tos+3,1,"%2d",p);

        if(env->panel[p]->local)
          mvwprintw(w,p-tos+3,4,"L");
        else
          mvwprintw(w,p-tos+3,4,"G");

        if(strlen(env->panel[p]->dirname)>0) {
          if(env->panel[p]->remote)
            mvwprintw(w,p-tos+3,6,"%-12s",env->panel[p]->nodename);
          else
            mvwprintw(w,p-tos+3,6,"[local node]");


          ufu_resize_string(env,env->panel[p]->dirname,45,s);
          mvwprintw(w,p-tos+3,19,"%-45s",s);

        }
        else {
          mvwprintw(w,p+3,3,"[ NA ]");
        }
        if(p==panel)  wattroff(w,A_REVERSE);
      }

    }

    ufu_wrefresh(w);

    key=ufu_get_key(env,UFU_NO_TEXT,NULL);

    switch(key) {

      case UFU_KEY_QUIT:
      case UFU_KEY_PANEL:
        ufu_add_hist(env,UFU_HIST_PANEL,NULL,panel);
        panel=-1;
        again=FALSE;
        break;

      case UFU_KEY_SELECT:
        env->key_select++;
        if(strlen(env->panel[panel]->dirname)>0) {
          again=FALSE;
          ufu_add_hist(env,UFU_HIST_PANEL,NULL,panel);
        }
        break;

      case UFU_KEY_UP:
        env->key_up++;
        if(panel>UFU_MIN_PANEL) {
          panel--;
          if(panel<tos) {
            tos-=height;
            if(tos<UFU_MIN_PANEL) {
              tos=UFU_MIN_PANEL;
            }
          }
          redraw=TRUE;
        }
        break;

      case UFU_KEY_DOWN:
        env->key_down++;
        if(panel<max_p) {
          panel++;
          if(panel>(tos+height-1)) {
            tos=tos+height;
            if(tos>max_p) {
              tos=max_p;
            }
          }
          redraw=TRUE;
        }
        break;

      case UFU_KEY_PREVPAGE:
        env->key_prev_page++;
        panel=panel-height;
        if(panel<UFU_MIN_PANEL)  panel=UFU_MIN_PANEL;
        tos=tos-height;
        if(tos<UFU_MIN_PANEL)    tos=UFU_MIN_PANEL;
        break;

      case UFU_KEY_NEXTPAGE:
        env->key_next_page++;
        panel=panel+height;
        if(panel>max_p) {
          panel=max_p;
        }
        tos=tos+height;
        if(tos>max_p) {
          tos=max_p-height+1;
          if(tos<UFU_MIN_PANEL) {
            tos=UFU_MIN_PANEL;
          }
        }
        break;

      case UFU_KEY_FIRST:
      case UFU_KEY_HOME:
        env->key_first++;
        panel=UFU_MIN_PANEL;
        tos=UFU_MIN_PANEL;
        redraw=TRUE;
        break;

      case UFU_KEY_LAST:
      case UFU_KEY_END:
        env->key_last++;
        panel=max_p;
        tos=max_p-height+1;
        if(tos<UFU_MIN_PANEL) {
          tos=UFU_MIN_PANEL;
        }
        redraw=TRUE;
        break;

      case UFU_KEY_HELP:
      case UFU_KEY_F1:
        env->key_help++;
        ufu_help(env,panel,UFU_HELP_SELECT_PANEL);
        ufu_wrefresh(w);
        redraw=TRUE;
        break;

      default:
        ufu_wrong_key(env);
        break;
    }

  }

  ufu_free_string(env,s);

  ufu_popdown(env,w);
  ufu_wrefresh(env->body);

  return(panel);

}

