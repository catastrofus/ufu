
#include "ufu.h"
#include "ufu.p"

void ufu_main(struct s_env *env) {

  int i,len,is_dir,is_lnk,local,remote;
  int panel,key,again,redraw,changed,marked,seqno,lineno;
  int rows,seq_tos,seq_bos;
  int sortc,sorto,header;
  char c;
  char *dname,*tmp_dname,*pname,*inp;
  struct s_entry *tos,*cos,*tmp_tos,*tmp_cos;
  struct s_remote *r;

  again=TRUE;
  redraw=TRUE;
  marked=FALSE;
  changed=FALSE;
  panel=UFU_MIN_PANEL;
  key=UFU_KEY_NOP;

  strcpy(env->fun,"ufu_main");
  ufu_wai(env);

  tmp_dname=ufu_alloc_string(env,UFU_LEN_NAME);
  // Size of the body minus top, bottom & header.
  rows=env->rows-6;

  ufu_clear(env);

  while(again) {

    is_dir=FALSE;
    is_lnk=FALSE;

    cos=ufu_show_entries(env,panel,key,redraw);

    if(marked)
      key=ufu_get_key(env,UFU_IS_MARKED,NULL);
    else
      key=ufu_get_key(env,UFU_NO_TEXT,NULL);
    marked=FALSE;

    switch(key) {

      case UFU_KEY_QUIT:
        if(env->confirmquit) {
          sprintf(env->msg,"%s, are you sure to quit this humble executable? ",env->master);
          c=ufu_get_yn(env);
          if((c=='y')||(c=='q')) {
            again=FALSE;
	    if(env->writeconfig) {
              ufu_write_config(env);
	    }
          }
        }
        else {
          again=FALSE;
	  if(env->writeconfig) {
            ufu_write_config(env);
	  }
        }
        break;

      case UFU_KEY_HELP:
      case UFU_KEY_F1:
        env->key_help++;
        ufu_help(env,panel,UFU_HELP_MAIN);
        redraw=TRUE;
        break;

      case UFU_KEY_INFO:
      case UFU_KEY_F2:
        env->key_info++;
        ufu_info_entry(env,cos,panel,NULL,NULL,UFU_INFO_ENTRY);
        redraw=TRUE;
        break;

      case UFU_KEY_SCROLL_DOWN:
        env->key_scroll_down++;
        if(cos->next!=NULL) {
          seq_tos=env->panel[panel]->tos->next->seqno;
          seq_bos=seq_tos+rows;
	  env->panel[panel]->tos=env->panel[panel]->tos->next;
          env->panel[panel]->cos=env->panel[panel]->cos->next;
          if(env->panel[panel]->cos->seqno>seq_bos) {
            env->panel[panel]->tos=env->panel[panel]->cos;
          }
        }
	break;

      case UFU_KEY_SCROLL_UP:
        env->key_scroll_up++;
        if(cos->prev!=NULL) {
          seq_tos=env->panel[panel]->tos->next->seqno;
          seq_bos=seq_tos+rows;
          env->panel[panel]->tos=env->panel[panel]->tos->prev;
          env->panel[panel]->cos=env->panel[panel]->cos->prev;
          if(env->panel[panel]->cos->seqno<seq_tos) {
            i=0;
            while((i<rows)&&(env->panel[panel]->tos->prev!=NULL)) {
              env->panel[panel]->tos=env->panel[panel]->tos->prev;
              i++;
            }
          }
        }
	break;

      case UFU_KEY_DOWN:
        env->key_down++;
        if(cos->next!=NULL) {
          seq_tos=env->panel[panel]->tos->seqno;
          seq_bos=seq_tos+rows-1;
          env->panel[panel]->cos=env->panel[panel]->cos->next;
          if(env->panel[panel]->cos->seqno>seq_bos) {
            env->panel[panel]->tos=env->panel[panel]->cos;
          }
        }
        break;

      case UFU_KEY_UP:
        env->key_up++;
        if(cos->prev!=NULL) {
          seq_tos=env->panel[panel]->tos->seqno;
          seq_bos=seq_tos+rows-1;
          env->panel[panel]->cos=env->panel[panel]->cos->prev;
          if(env->panel[panel]->cos->seqno<seq_tos) {
            i=0;
            while((i<rows)&&(env->panel[panel]->tos->prev!=NULL)) {
              env->panel[panel]->tos=env->panel[panel]->tos->prev;
              i++;
            }
          }
        }
        break;

      case UFU_KEY_FIRST:
      case UFU_KEY_HOME:
        env->key_first++;
        env->panel[panel]->tos=env->panel[panel]->first;
        env->panel[panel]->cos=env->panel[panel]->first;
        if(env->panel[panel]->cos->next->next!=NULL) {
          env->panel[panel]->cos=env->panel[panel]->cos->next->next;
        }
        break;

      case UFU_KEY_LAST:
      case UFU_KEY_END:
        env->key_last++;
        while(cos->next!=NULL) {
          cos=cos->next;
        }
        env->panel[panel]->cos=cos;
        env->panel[panel]->tos=cos;
        i=0;
        while((i<(rows-1))&&(env->panel[panel]->tos->prev!=NULL)) {
          env->panel[panel]->tos=env->panel[panel]->tos->prev;
          i++;
        }
        break;

      case UFU_KEY_NEXTPAGE:
      case KEY_NPAGE:
        env->key_next_page++;
        i=0;
        tos=env->panel[panel]->tos;
        while((i<rows)&&(tos->next!=NULL)) {
          tos=tos->next;
          i++;
        }
        if(i==rows) {
          env->panel[panel]->tos=tos;
          env->panel[panel]->cos=tos;
        }
        break;

      case UFU_KEY_PREVPAGE:
      case KEY_PPAGE:
        env->key_prev_page++;
        i=0;
        while((i<rows)&&(env->panel[panel]->tos->prev!=NULL)) {
          env->panel[panel]->tos=env->panel[panel]->tos->prev;
          i++;
        }
        env->panel[panel]->cos=env->panel[panel]->tos;
        break;

      case UFU_KEY_DIRHOME:
        env->key_dir_home++;

        sprintf(env->msg,"%s, please wait... ",env->master);
        ufu_msg(env);

        if(!(env->panel[panel]->remote)) {
          ufu_add_hist(env,UFU_HIST_ENTRY,env->panel[panel]->dirname,cos->seqno);
          header=env->panel[panel]->header;
          sortc=env->panel[panel]->sortc;
          sorto=env->panel[panel]->sorto;
          dname=ufu_alloc_string(env,UFU_LEN_NAME);
          pname=ufu_alloc_string(env,UFU_LEN_NAME);
          strcpy(dname,env->homedir);
          strcpy(pname,env->panel[panel]->dirname);
          ufu_free_panel(env,panel);
          env->panel[panel]=ufu_alloc_panel(env,panel);
          strcpy(env->panel[panel]->dirname,dname);
          strcpy(env->panel[panel]->dirnameprev,pname);
          ufu_read_entries(env,panel);
          env->panel[panel]->header=header;
          env->panel[panel]->sortc=sortc;
          env->panel[panel]->sorto=sorto;
          ufu_sort(env,panel);
          ufu_free_string(env,dname);
          ufu_free_string(env,pname);
          seqno=ufu_find_hist(env,UFU_HIST_ENTRY,env->panel[panel]->dirname);
          if(seqno>0) {
            ufu_goto_seqno(env,UFU_HIST_ENTRY,panel,seqno);
          }
        }
        else {
          sprintf(env->msg,"Not allowed for remote panels!");
          ufu_alert(env);
        }
        break;

      case UFU_KEY_DIRWORK:
        env->key_dir_work++;

        sprintf(env->msg,"%s, please wait... ",env->master);
        ufu_msg(env);

        if(!(env->panel[panel]->remote)) {
          ufu_add_hist(env,UFU_HIST_ENTRY,env->panel[panel]->dirname,cos->seqno);
          header=env->panel[panel]->header;
          sortc=env->panel[panel]->sortc;
          sorto=env->panel[panel]->sorto;
          dname=ufu_alloc_string(env,UFU_LEN_NAME);
          pname=ufu_alloc_string(env,UFU_LEN_NAME);
          strcpy(dname,env->workdir);
          strcpy(pname,env->panel[panel]->dirname);
          ufu_free_panel(env,panel);
          env->panel[panel]=ufu_alloc_panel(env,panel);
          strcpy(env->panel[panel]->dirname,dname);
          strcpy(env->panel[panel]->dirnameprev,pname);
          ufu_read_entries(env,panel);
          env->panel[panel]->header=header;
          env->panel[panel]->sortc=sortc;
          env->panel[panel]->sorto=sorto;
          ufu_sort(env,panel);
          ufu_free_string(env,dname);
          ufu_free_string(env,pname);
          seqno=ufu_find_hist(env,UFU_HIST_ENTRY,env->panel[panel]->dirname);
          if(seqno>0) {
            ufu_goto_seqno(env,UFU_HIST_ENTRY,panel,seqno);
          }
        }
        else {
          sprintf(env->msg,"Not allowed for remote panels!");
          ufu_alert(env);
        }
        break;

      case UFU_KEY_DIRROOT:
        env->key_dir_root++;

        sprintf(env->msg,"%s, please wait... ",env->master);
        ufu_msg(env);

        ufu_add_hist(env,UFU_HIST_ENTRY,env->panel[panel]->dirname,cos->seqno);
        header=env->panel[panel]->header;
        sortc=env->panel[panel]->sortc;
        sorto=env->panel[panel]->sorto;
        dname=ufu_alloc_string(env,UFU_LEN_NAME);
        pname=ufu_alloc_string(env,UFU_LEN_NAME);
        strcpy(dname,"/");
        strcpy(pname,env->panel[panel]->dirname);
        local=env->panel[panel]->local;
        remote=env->panel[panel]->remote;
        if(remote) {
          r=env->panel[panel]->rem;
        }
        ufu_free_panel(env,panel);
        env->panel[panel]=ufu_alloc_panel(env,panel);
        strcpy(env->panel[panel]->dirname,dname);
        strcpy(env->panel[panel]->dirnameprev,pname);
        if(remote) {
          env->panel[panel]->remote=remote;
          env->panel[panel]->rem=r;
          env->panel[panel]->local=FALSE;
          ufu_com_read_dir(env,env->panel[panel]->dirname,panel);
        }
        else {
          env->panel[panel]->remote=FALSE;
          env->panel[panel]->rem=NULL;
          env->panel[panel]->local=local;
          ufu_read_entries(env,panel);
        }
        env->panel[panel]->header=header;
        env->panel[panel]->sortc=sortc;
        env->panel[panel]->sorto=sorto;
        ufu_sort(env,panel);
        ufu_free_string(env,dname);
        ufu_free_string(env,pname);
        seqno=ufu_find_hist(env,UFU_HIST_ENTRY,env->panel[panel]->dirname);
        if(seqno>0) {
          ufu_goto_seqno(env,UFU_HIST_ENTRY,panel,seqno);
        }
        break;

      case UFU_KEY_DIRUP:
        env->key_dir_up++;

        sprintf(env->msg,"%s, please wait... ",env->master);
        ufu_msg(env);

        ufu_add_hist(env,UFU_HIST_ENTRY,env->panel[panel]->dirname,cos->seqno);
        header=env->panel[panel]->header;
        sortc=env->panel[panel]->sortc;
        sorto=env->panel[panel]->sorto;
        dname=ufu_alloc_string(env,UFU_LEN_NAME);
        pname=ufu_alloc_string(env,UFU_LEN_NAME);
        strcpy(tmp_dname,env->panel[panel]->dirname);
        strcpy(pname,env->panel[panel]->dirname);
        if(strcmp(cos->fname,"/")!=0) {
          strcpy(dname,env->panel[panel]->dirname);
          i=strlen(dname)-1;
          while(dname[i]!='/') {
            dname[i]='\0';
            i--;
          }
          if(strcmp(dname,"/")!=0) {
            dname[i]='\0';
          }
        }
        local=env->panel[panel]->local;
        remote=env->panel[panel]->remote;
        if(remote) {
          r=env->panel[panel]->rem;
        }
        ufu_free_panel(env,panel);
        env->panel[panel]=ufu_alloc_panel(env,panel);
        strcpy(env->panel[panel]->dirname,dname);
        if(remote) {
          env->panel[panel]->remote=remote;
          env->panel[panel]->rem=r;
          env->panel[panel]->local=FALSE;
          ufu_com_read_dir(env,env->panel[panel]->dirname,panel);
        }
        else {
          env->panel[panel]->remote=FALSE;
          env->panel[panel]->rem=NULL;
          env->panel[panel]->local=local;
          ufu_read_entries(env,panel);
        }
        if(env->panel[panel]->ntotal==0) {
          ufu_free_panel(env,panel);
          env->panel[panel]=ufu_alloc_panel(env,panel);
          strcpy(env->panel[panel]->dirname,tmp_dname);
          ufu_read_entries(env,panel);
        }
        else {
          strcpy(env->panel[panel]->dirnameprev,pname);
        }
        env->panel[panel]->header=header;
        env->panel[panel]->sortc=sortc;
        env->panel[panel]->sorto=sorto;
        ufu_sort(env,panel);
        ufu_free_string(env,dname);
        ufu_free_string(env,pname);
        changed=TRUE;
        seqno=ufu_find_hist(env,UFU_HIST_ENTRY,env->panel[panel]->dirname);
        if(seqno>0) {
          ufu_goto_seqno(env,UFU_HIST_ENTRY,panel,seqno);
        }
        break;

      case UFU_KEY_PREVDIR:
        env->key_dir_prev++;

        sprintf(env->msg,"%s, please wait... ",env->master);
        ufu_msg(env);

        if(strlen(env->panel[panel]->dirnameprev)>0) {
          ufu_add_hist(env,UFU_HIST_ENTRY,env->panel[panel]->dirname,cos->seqno);
          header=env->panel[panel]->header;
          sortc=env->panel[panel]->sortc;
          sorto=env->panel[panel]->sorto;
          dname=ufu_alloc_string(env,UFU_LEN_NAME);
          pname=ufu_alloc_string(env,UFU_LEN_NAME);
          strcpy(dname,env->panel[panel]->dirnameprev);
          strcpy(pname,env->panel[panel]->dirname);
          local=env->panel[panel]->local;
          remote=env->panel[panel]->remote;
          if(remote) {
            r=env->panel[panel]->rem;
          }
          ufu_free_panel(env,panel);
          env->panel[panel]=ufu_alloc_panel(env,panel);
          strcpy(env->panel[panel]->dirname,dname);
          strcpy(env->panel[panel]->dirnameprev,pname);
          if(remote) {
            env->panel[panel]->remote=remote;
            env->panel[panel]->rem=r;
            env->panel[panel]->local=FALSE;
            ufu_com_read_dir(env,env->panel[panel]->dirname,panel);
          }
          else {
            env->panel[panel]->remote=FALSE;
            env->panel[panel]->rem=NULL;
            env->panel[panel]->local=local;
            ufu_read_entries(env,panel);
          }
          env->panel[panel]->header=header;
          env->panel[panel]->sortc=sortc;
          env->panel[panel]->sorto=sorto;
          ufu_sort(env,panel);
          ufu_free_string(env,dname);
          ufu_free_string(env,pname);
          seqno=ufu_find_hist(env,UFU_HIST_ENTRY,env->panel[panel]->dirname);
          if(seqno>0) {
            ufu_goto_seqno(env,UFU_HIST_ENTRY,panel,seqno);
          }
        }
        break;

      case UFU_KEY_SELECT:
      case UFU_KEY_ENTER:
        ufu_clean_hist(env);

        sprintf(env->msg,"%s, please wait... ",env->master);
        ufu_msg(env);

        if(env->panel[panel]->remote)
          is_dir=cos->is_dir;
        else
          is_dir=S_ISDIR(cos->fstat->st_mode);

        if(is_dir) {
          env->key_select++;
          ufu_add_hist(env,UFU_HIST_ENTRY,env->panel[panel]->dirname,cos->seqno);
          header=env->panel[panel]->header;
          sortc=env->panel[panel]->sortc;
          sorto=env->panel[panel]->sorto;
          local=env->panel[panel]->local;
          remote=env->panel[panel]->remote;
          if(remote) {
            r=env->panel[panel]->rem;
          }
          dname=ufu_alloc_string(env,UFU_LEN_NAME);
          pname=ufu_alloc_string(env,UFU_LEN_NAME);
          strcpy(tmp_dname,env->panel[panel]->dirname);
          strcpy(pname,env->panel[panel]->dirname);
          if(strcmp(cos->fname,".")==0) {
            strcpy(dname,env->panel[panel]->dirname);
          }
          else {
            if(strcmp(cos->fname,"..")==0) {
              if(strcmp(cos->fname,"/")!=0) {
                strcpy(dname,env->panel[panel]->dirname);
                i=strlen(dname)-1;
                while(dname[i]!='/') {
                  dname[i]='\0';
                  i--;
                }
                if(strcmp(dname,"/")!=0) {
                  dname[i]='\0';
                }
              }
            }
            else {
              if(env->panel[panel]->remote)
                is_lnk=cos->is_lnk;
              else
                is_lnk=S_ISLNK(cos->lstat->st_mode);

              if(is_lnk) {
                if(cos->lname[0]=='/') {
                  strcpy(dname,cos->lname);
                }
                else {
                  strcpy(dname,env->panel[panel]->dirname);
                  strcat(dname,"/");
                  strcat(dname,cos->lname);
                }
              }
              else {
                strcpy(dname,env->panel[panel]->dirname);
                if(strcmp(dname,"/")!=0) {
                  strcat(dname,"/");
                  strcat(dname,cos->fname);
                }
                else {
                  strcat(dname,cos->fname);
                }
              }
            }
          }
          ufu_free_panel(env,panel);
          env->panel[panel]=ufu_alloc_panel(env,panel);
          strcpy(env->panel[panel]->dirname,dname);
          env->panel[panel]->remote=remote;
          if(remote) {
            env->panel[panel]->remote=remote;
            env->panel[panel]->rem=r;
            env->panel[panel]->local=FALSE;
            ufu_com_read_dir(env,env->panel[panel]->dirname,panel);
          }
          else {
            env->panel[panel]->remote=FALSE;
            env->panel[panel]->rem=NULL;
            env->panel[panel]->local=local;
            ufu_read_entries(env,panel);
          }

          if(env->panel[panel]->ntotal==0) {
            local=env->panel[panel]->local;
            remote=env->panel[panel]->remote;
            if(remote) {
              r=env->panel[panel]->rem;
            }
            ufu_free_panel(env,panel);
            env->panel[panel]=ufu_alloc_panel(env,panel);
            strcpy(env->panel[panel]->dirname,tmp_dname);
            if(remote) {
              env->panel[panel]->remote=remote;
              env->panel[panel]->rem=r;
              env->panel[panel]->local=FALSE;
              ufu_com_read_dir(env,env->panel[panel]->dirname,panel);
            }
            else {
              env->panel[panel]->remote=FALSE;
              env->panel[panel]->rem=NULL;
              env->panel[panel]->local=local;
              ufu_read_entries(env,panel);
            }
          }
          else {
            strcpy(env->panel[panel]->dirnameprev,pname);
          }
          env->panel[panel]->header=header;
          env->panel[panel]->sortc=sortc;
          env->panel[panel]->sorto=sorto;
          ufu_sort(env,panel);
          ufu_free_string(env,dname);
          ufu_free_string(env,pname);
          changed=TRUE;
          seqno=ufu_find_hist(env,UFU_HIST_ENTRY,env->panel[panel]->dirname);
          if(seqno>0) {
            ufu_goto_seqno(env,UFU_HIST_ENTRY,panel,seqno);
          }
        }
        else {
          env->key_info++;
          ufu_info_entry(env,cos,panel,NULL,NULL,UFU_INFO_ENTRY);
          redraw=TRUE;
        }
        break;

      case UFU_KEY_PANEL:
        env->key_panel++;
        ufu_clean_hist(env);
        ufu_add_hist(env,UFU_HIST_ENTRY,env->panel[panel]->dirname,cos->seqno);
        panel=ufu_show_panels(env,panel);
        seqno=ufu_find_hist(env,UFU_HIST_ENTRY,env->panel[panel]->dirname);
        if(seqno>0) {
          ufu_goto_seqno(env,UFU_HIST_ENTRY,panel,seqno);
        }
        redraw=TRUE;
        break;

      case UFU_KEY_HEADER:
      case UFU_KEY_F12:
        env->key_header++;
        switch(env->panel[panel]->header) {
          case UFU_HDR_STD:
            env->panel[panel]->header=UFU_HDR_LA;
            break;
          case UFU_HDR_LA:
            env->panel[panel]->header=UFU_HDR_LM;
            break;
          case UFU_HDR_LM:
            env->panel[panel]->header=UFU_HDR_LC;
            break;
          case UFU_HDR_LC:
            env->panel[panel]->header=UFU_HDR_STD;
            break;
        }
        redraw=TRUE;
        changed=TRUE;
        break;

      case UFU_KEY_SORT_COL:
      case UFU_KEY_F9:
        env->key_sort_col++;
        switch(env->panel[panel]->sortc) {
          case UFU_SORT_FNAME:
            env->panel[panel]->sortc=UFU_SORT_SIZE;
            break;
          case UFU_SORT_SIZE:
            env->panel[panel]->sortc=UFU_SORT_LA;
            break;
          case UFU_SORT_LA:
            env->panel[panel]->sortc=UFU_SORT_LM;
            break;
          case UFU_SORT_LM:
            env->panel[panel]->sortc=UFU_SORT_LC;
            break;
          case UFU_SORT_LC:
            env->panel[panel]->sortc=UFU_SORT_FNAME;
            break;
        }
        ufu_sort(env,panel);
        redraw=TRUE;
        changed=TRUE;
        break;

      case UFU_KEY_SORT_ORDER:
      case UFU_KEY_F10:
        env->key_sort_order++;
        switch(env->panel[panel]->sorto) {
          case UFU_SORT_ASC:
            env->panel[panel]->sorto=UFU_SORT_DSC;
            break;
          case UFU_SORT_DSC:
            env->panel[panel]->sorto=UFU_SORT_ASC;
            break;
        }
        ufu_sort(env,panel);
        cos=tos;
        redraw=TRUE;
        changed=TRUE;
        break;

      case UFU_KEY_LEFT:
        env->key_prev_panel++;
        if(panel>UFU_MIN_PANEL) {
          panel--;
          redraw=TRUE;
        }
        else {
          panel=env->mpanel;
          while(strlen(env->panel[panel]->dirname)==0) {
            panel--;
          }
        }
        break;

      case UFU_KEY_RIGHT:
        env->key_next_panel++;
        if(panel<env->mpanel) {
          if(strlen(env->panel[panel+1]->dirname)>0) {
            panel++;
          }
          else {
            panel=UFU_MIN_PANEL;
          }
          redraw=TRUE;
        }
        break;

      case UFU_KEY_MARKED:
      case UFU_KEY_F11:
        env->key_show_mark++;
        ufu_show_mark(env);
        redraw=TRUE;
        break;

      case UFU_KEY_MARK:
        env->key_add_mark++;
        tmp_tos=env->panel[panel]->tos;
        tmp_cos=env->panel[panel]->cos;
        marked=ufu_try2mark(env,panel,cos);
        env->panel[panel]->cos=tmp_cos;
        env->panel[panel]->tos=tmp_tos;
        redraw=TRUE;
        break;

      case UFU_KEY_APPEND:
        env->key_add_mark++;
	sprintf(env->msg,"Do you wish to mark all the regular files? ");
	if(ufu_get_yn(env)=='y') {
          tmp_cos=env->panel[panel]->first;
	  while(tmp_cos!=NULL) {
            if(tmp_cos->is_reg) {
              sprintf(env->msg,"Marking entry: %s",tmp_cos->fname);
	      ufu_log(env);
              sprintf(env->msg,"Marking entry: %s",tmp_cos->fname);
	      ufu_msg(env);
              marked=ufu_try2mark(env,panel,tmp_cos);
	    }
	    tmp_cos=tmp_cos->next;
	  }
	}
        redraw=FALSE;;
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

      case UFU_KEY_EDIT:
        env->key_edit++;
        if(env->panel[panel]->remote) {
          ufu_com_get_txtfile(env,env->panel[panel]->dirname,cos->fname,panel,UFU_EDIT);
        }
        else {
          ufu_cmd_edit(env,env->panel[panel]->dirname,cos->fname);
        }
        break;

      case UFU_KEY_VIEW:
        env->key_view++;
        if(env->panel[panel]->remote) {
          ufu_com_get_txtfile(env,env->panel[panel]->dirname,cos->fname,panel,UFU_VIEW);
        }
        else {
          ufu_cmd_view(env,env->panel[panel]->dirname,cos->fname);
        }
        break;

      case UFU_KEY_UCMD:
        if(!env->panel[panel]->remote) {
          ufu_show_ucmd(env,panel,cos->fname);
        }
        break;

      case UFU_KEY_REMOTE:
        ufu_show_remote(env,NULL);
        break;

      case UFU_KEY_LOG:
        env->key_view++;
        ufu_view_log(env);
        break;

      case UFU_KEY_TC:
        env->key_tc++;
        ufu_show_tc(env);
        break;

      case UFU_KEY_PLUS:
        env->key_u++;
        ufu_show_u(env);
        break;

      case UFU_KEY_MINUS:
        env->key_g++;
        ufu_show_g(env);
        break;

      case UFU_KEY__:
        env->key_msg++;
        ufu_show_msg(env);
        break;

      case UFU_KEY_0:
        env->key_ucmd_0++;
        ufu_cmd_exec_main(env,panel,UFU_KEY_0);
        break;
      case UFU_KEY_1:
        env->key_ucmd_1++;
        ufu_cmd_exec_main(env,panel,UFU_KEY_1);
        break;
      case UFU_KEY_2:
        env->key_ucmd_2++;
        ufu_cmd_exec_main(env,panel,UFU_KEY_2);
        break;
      case UFU_KEY_3:
        env->key_ucmd_3++;
        ufu_cmd_exec_main(env,panel,UFU_KEY_3);
        break;
      case UFU_KEY_4:
        env->key_ucmd_4++;
        ufu_cmd_exec_main(env,panel,UFU_KEY_4);
        break;
      case UFU_KEY_5:
        env->key_ucmd_5++;
        ufu_cmd_exec_main(env,panel,UFU_KEY_5);
        break;
      case UFU_KEY_6:
        env->key_ucmd_6++;
        ufu_cmd_exec_main(env,panel,UFU_KEY_6);
        break;
      case UFU_KEY_7:
        env->key_ucmd_7++;
        ufu_cmd_exec_main(env,panel,UFU_KEY_7);
        break;
      case UFU_KEY_8:
        env->key_ucmd_8++;
        ufu_cmd_exec_main(env,panel,UFU_KEY_8);
        break;
      case UFU_KEY_9:
        env->key_ucmd_9++;
        ufu_cmd_exec_main(env,panel,UFU_KEY_9);
        break;

      case UFU_KEY_GO:
        env->key_go++;
        inp=ufu_alloc_string(env,6);
        sprintf(env->msg,"%s, enter your seqno:",env->master);
        ufu_msg(env);
        ufu_clear_string(env,inp,6);
        ufu_rl(env,env->bottom,strlen(env->msg)+1,0,TRUE,5,5,inp,TRUE,FALSE);
        lineno=atoi(inp);

        ufu_goto_seqno(env,UFU_SEARCH_ENTRY,panel,lineno);
        cos=env->panel[panel]->cos;
        tos=env->panel[panel]->tos;

        ufu_free_string(env,inp);
        break;

      case UFU_KEY_SEARCH:

        env->key_search++;
        sprintf(env->msg,"%s, enter pattern:",env->master);
        ufu_msg(env);
        len=env->cols-strlen(env->msg)-1;
        inp=ufu_alloc_string(env,len);
        ufu_clear_string(env,inp,len);
        ufu_rl(env,env->bottom,strlen(env->msg)+1,0,TRUE,len-1,len-1,inp,TRUE,FALSE);

        env->panel[panel]->cos=cos;
        env->panel[panel]->tos=tos;

        ufu_goto_fname(env,UFU_SEARCH_ENTRY,panel,inp);

        cos=env->panel[panel]->cos;
        tos=env->panel[panel]->tos;

        ufu_free_string(env,inp);
        break;

      default:
        ufu_wrong_key(env);
        break;

    }

  }

  ufu_free_string(env,tmp_dname);

}

