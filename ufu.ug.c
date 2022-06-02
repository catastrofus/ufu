
#include "ufu.h"
#include "ufu.p"

void ufu_show_u(struct s_env *env) {

  int rem,again,found,redraw,key,len,row,col,rows,seqno,seq_tos,seq_bos,i,r;
  int header,len_uname,len_gname,len_dir,len_sh,len_tot,len_tmp,c,sort_uid;
  int offset;
  char *gname,*s;
  struct s_usr *u,*cos,*tos;
  struct s_grp *g;
  struct s_grpusr *gu;

  strcpy(env->fun,"ufu_show_u");
  ufu_wai(env);

  header=1;
  gname=ufu_alloc_string(env,18);

  sort_uid=TRUE;
  ufu_sort_u(env,sort_uid);

  sprintf(env->msg,"Scanning fieldlengths");
  ufu_log(env);

  len_uname=0;
  len_dir=0;
  len_sh=0;
  u=env->ufirst;
  while(u!=NULL) {
    if(strlen(u->uname)>len_uname) {
      len_uname=strlen(u->uname);
      sprintf(env->msg,"Max length username: %d",len_uname);
      ufu_log(env);
    }
    if(strlen(u->dir)>len_dir) {
      len_dir=strlen(u->dir);
      sprintf(env->msg,"Max length dir: %d",len_dir);
      ufu_log(env);
    }
    if(strlen(u->sh)>len_sh) {
      len_sh=strlen(u->sh);
      sprintf(env->msg,"Max length shell: %d",len_sh);
      ufu_log(env);
    }
    u=u->next;
  }
  len_gname=0;
  g=env->gfirst;
  while(g!=NULL) {
    if(strlen(g->gname)>len_gname) {
      len_gname=strlen(g->gname);
      sprintf(env->msg,"Max length groupname: %d",len_gname);
      ufu_log(env);
    }
    g=g->next;
  }

  tos=env->ufirst;
  cos=env->ufirst;

  u=env->ufirst;

  rows=env->rows-6;

  if(env->hist_u>0) {

    seqno=env->hist_u;
    u=env->ufirst;
    while((u!=NULL)&&(u->seqno!=seqno)) {
      u=u->next;
    }

    if(u!=NULL) {
      cos=u;
      tos=u;

      r=rows/2;
      while((u->prev!=NULL)&&(r>0)) {
        r--;
        tos=u;
        u=u->prev;
      }
    }
    else {
      u=env->ufirst;
    }

  }

  offset=0;

  redraw=TRUE;
  again=(u!=NULL);

  while(again) {

    if(redraw) {

      ufu_wclear(env->top);
      mvwprintw(env->top,0,0,"[%s] Show users",env->nodename);
      if((header==2)&&(offset>0)) {
        mvwprintw(env->top,0,env->cols-12,"Offset: %d",offset);
      }

      ufu_wrefresh(env->top);

      ufu_wclear(env->bottom);
      ufu_wrefresh(env->bottom);

      ufu_wclear(env->body);

      // UID.
      mvwprintw(env->body,0,1,"  UID");
      mvwprintw(env->body,1,1,"-----");
      // Username.
      mvwprintw(env->body,0,7,"User");
      for(i=0;i<len_uname;i++) {
        mvwprintw(env->body,1,7+i,"-");
      }

      // GID.
      len_tot=8+len_uname;
      mvwprintw(env->body,0,len_tot,"  GID");
      mvwprintw(env->body,1,len_tot,"-----");
      // Groupname..
      len_tot=14+len_uname;
      mvwprintw(env->body,0,len_tot,"Group");
      for(i=0;i<len_gname;i++) {
        mvwprintw(env->body,1,len_tot+i,"-");
      }

      switch(header) {

	case 1:
          // Directory.
          len_tot=15+len_uname+len_gname;
          if((env->cols-len_tot)>len_dir) {
            mvwprintw(env->body,0,len_tot,"Directory");
	    for(i=0;i<len_dir;i++) {
              mvwprintw(env->body,1,len_tot+i,"-");
	    }
          }

          // Shell.
          len_tot=16+len_uname+len_gname+len_dir;
          if((env->cols-len_tot)>len_sh) {
            mvwprintw(env->body,0,len_tot,"Shell");
	    for(i=0;i<len_sh;i++) {
              mvwprintw(env->body,1,len_tot+i,"-");
	    }
          }
	  break;

	case 2:
          // Secondairy groups.
          len_tot=15+len_uname+len_gname;
          if((env->cols-len_tot)>len_sh) {
            mvwprintw(env->body,0,len_tot,"Secondary groups");
	    for(i=0;i<(env->cols-len_tot-1);i++) {
              mvwprintw(env->body,1,len_tot+i,"-");
	    }
          }
	  break;

      }

      ufu_wrefresh(env->body);

    }

    u=tos;
    row=1;

    while(row<=rows) {

      if(u!=NULL) {

        if(u==cos)  wattron(env->body,A_REVERSE);

        strcpy(gname,"??");
        found=FALSE;
        g=env->gfirst;
        while((g!=NULL)&&(!found)) {
          if(u->gid==g->gid) {
            found=TRUE;
            strcpy(gname,g->gname);
          }
          g=g->next;
        }

        mvwprintw(env->body,row+1,1,"%5d",u->uid);
        mvwprintw(env->body,row+1,8+len_uname,"%5d",u->gid);

	// Username.
        mvwprintw(env->body,row+1,7,"%s",u->uname);
        for(i=strlen(u->uname);i<len_uname;i++) {
          mvwprintw(env->body,row+1,7+i," ");
        }

	// Groupname.
	len_tot=14+len_uname;
        mvwprintw(env->body,row+1,len_tot,"%s",gname);
        for(i=strlen(gname);i<len_gname;i++) {
          mvwprintw(env->body,row+1,len_tot+i," ");
        }

	switch(header) {

          case 1:
	    // Directory.
	    len_tot=15+len_uname+len_gname;
            if((env->cols-len_tot)>len_dir) {
              mvwprintw(env->body,row+1,len_tot,"%s",u->dir);
	      for(i=strlen(u->dir);i<len_dir;i++) {
                mvwprintw(env->body,row+1,len_tot+i," ");
	      }
	    }
	    // Shell.
	    len_tot=16+len_uname+len_gname+len_dir;
            if((env->cols-len_tot)>len_sh) {
              mvwprintw(env->body,row+1,len_tot,"%s",u->sh);
	      for(i=strlen(u->sh);i<len_sh;i++) {
                mvwprintw(env->body,row+1,len_tot+i," ");
	      }
	    }
	    break;

	  case 2:
	    // Secondary groups.
	    s=ufu_alloc_string(env,UFU_MAX_TEXT);
	    len_tot=15+len_uname+len_gname;
	    len_tmp=0;
	    c=0;
            g=env->gfirst;
	    while(g!=NULL) {
              gu=g->fgrpusr;
	      while(gu!=NULL) {
                if(strcmp(gu->uname,u->uname)==0) {
                  if(c>0) {
                    strcat(s,",");
		    strcat(s,g->gname);
		    len_tmp+=strlen(g->gname)+1;
		  }
		  else {
                    strcpy(s,g->gname);
		    len_tmp=strlen(g->gname);
		  }
		  c++;
		}
		gu=gu->next;
              }
              g=g->next;
	    }

            if((env->cols-len_tot)>len_tmp) {
              mvwprintw(env->body,row+1,len_tot,"%s",s+offset);
              for(i=len_tot+len_tmp-offset;i<(env->cols-1);i++) {
                if((len_tmp-offset)>0) {
                  mvwprintw(env->body,row+1,i," ");
                }
              }
	    }
	    else {
              if(u==cos) wattroff(env->body,A_REVERSE);
              mvwprintw(env->body,row+1,len_tot-1,"!",s);
              if(u==cos)  wattron(env->body,A_REVERSE);
              for(i=0;i<env->cols-len_tot-1;i++) {
                mvwprintw(env->body,row+1,len_tot+i,"%c",s[i+offset]);
              }
              for(i=len_tot+len_tmp-offset;i<(env->cols-1);i++) {
                if(offset<len_tmp) {
                  mvwprintw(env->body,row+1,i," ");
                }
              }
	    }

	    ufu_free_string(env,s);
	    break;

	}

        if(u==cos) wattroff(env->body,A_REVERSE);

        u=u->next;

      }

      row++;

    }

    ufu_wrefresh(env->body);

    key=ufu_get_key(env,UFU_NO_TEXT,NULL);

    switch(key) {

      case UFU_KEY_QUIT:
        env->hist_u=cos->seqno;
        again=FALSE;
        break;

      case UFU_KEY_0:
        sort_uid=!sort_uid;
        ufu_sort_u(env,sort_uid);
        cos=env->ufirst;
        tos=env->ufirst;
        seqno=env->ufirst->seqno;
        break;

      case UFU_KEY_1:
	// Display Directory/Shell.
	header=1;
	redraw=TRUE;
	break;

      case UFU_KEY_2:
	// Display Secondary groups.
	header=2;
	redraw=TRUE;
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

      case UFU_KEY_LEFT:
        env->key_left++;
        if((header==2)&&(offset>0)) {
          offset--;
        }
        break;

      case UFU_KEY_RIGHT:
        env->key_right++;
        if((header==2)&&(offset<(len_tot-1))) {
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
        tos=env->ufirst;
        cos=env->ufirst;
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

  ufu_free_string(env,gname);

}

void ufu_sort_u(struct s_env *env,int sort) {

  struct s_usr *u1,*u2;

  strcpy(env->fun,"ufu_sort_u");
  ufu_wai(env);

  sprintf(env->msg,"Start sorting users");
  ufu_log(env);

  u1=env->ufirst;
  while(u1!=NULL) {

    u2=env->ufirst;
    while((u2!=NULL)&&(u2->seqno<u1->seqno)) {

      if(sort) {
        if(u2->uid>u1->uid) {
          ufu_sort_u_swap(env,u1,u2);
        }
      }
      else {
        if(strcmp(u2->uname,u1->uname)>0) {
          ufu_sort_u_swap(env,u1,u2);
        }
      }

      u2=u2->next;

    }

    u1=u1->next;
  }

  sprintf(env->msg,"Finished sorting users");
  ufu_log(env);

}

void ufu_sort_u_swap(struct s_env *env,struct s_usr *u1,struct s_usr *u2) {

  struct s_usr *t;

  strcpy(env->fun,"ufu_sort_u_swap");
  ufu_wai(env);

  t=ufu_alloc_usr(env);

  t->uid=u2->uid;
  t->gid=u2->gid;
  strcpy(t->uname,u2->uname);
  strcpy(t->dir,u2->dir);
  strcpy(t->sh,u2->sh);

  u2->uid=u1->uid;
  u2->gid=u1->gid;
  strcpy(u2->uname,u1->uname);
  strcpy(u2->dir,u1->dir);
  strcpy(u2->sh,u1->sh);

  u1->uid=t->uid;
  u1->gid=t->gid;
  strcpy(u1->uname,t->uname);
  strcpy(u1->dir,t->dir);
  strcpy(u1->sh,t->sh);

  ufu_free_usr(env,t);

}

void ufu_show_g(struct s_env *env) {

  int rem,again,found,redraw,key,len,row,col,rows,seqno,seq_tos,seq_bos,i,r;
  int len_gname,len_text,len_tot,len_tmp,c,sort_gid,offset,pos;
  char *s;
  struct s_usr *u;
  struct s_grp *g,*cos,*tos;
  struct s_grpusr *gu;

  strcpy(env->fun,"ufu_show_g");
  ufu_wai(env);

  sort_gid=TRUE;
  ufu_sort_g(env,sort_gid);

  s=ufu_alloc_string(env,UFU_MAX_TEXT);
  offset=0;

  sprintf(env->msg,"Scanning fieldlengths");
  ufu_log(env);

  len_gname=0;
  g=env->gfirst;
  while(g!=NULL) {
    if(strlen(g->gname)>len_gname) {
      len_gname=strlen(g->gname);
      sprintf(env->msg,"Max length groupname: %d",len_gname);
      ufu_log(env);
    }
    g=g->next;
  }

  tos=env->gfirst;
  cos=env->gfirst;

  g=env->gfirst;

  rows=env->rows-6;

  if(env->hist_g>0) {

    seqno=env->hist_g;
    g=env->gfirst;
    while((g!=NULL)&&(g->seqno!=seqno)) {
      g=g->next;
    }

    if(g!=NULL) {
      cos=g;
      tos=g;

      r=rows/2;
      while((g->prev!=NULL)&&(r>0)) {
        r--;
        tos=g;
        g=g->prev;
      }
    }
    else {
      g=env->gfirst;
    }

  }

  redraw=TRUE;
  again=(g!=NULL);

  while(again) {

    if(redraw) {

      ufu_wclear(env->top);
      mvwprintw(env->top,0,0,"[%s] Show groups",env->nodename);
      if(offset>0) {
        mvwprintw(env->top,0,env->cols-12,"Offset: %d",offset);
      }

      ufu_wrefresh(env->top);

      ufu_wclear(env->bottom);
      ufu_wrefresh(env->bottom);

      ufu_wclear(env->body);

      // GID.
      mvwprintw(env->body,0,1,"  GID");
      mvwprintw(env->body,1,1,"-----");
      // Groupname.
      mvwprintw(env->body,0,7,"Group");
      for(i=0;i<len_gname;i++) {
        mvwprintw(env->body,1,7+i,"-");
      }

      // Users.
      len_tot=8+len_gname;;
      if((env->cols-len_tot)>15) {
        mvwprintw(env->body,0,len_tot,"Users");
        for(i=0;i<(env->cols-len_tot-1);i++) {
          mvwprintw(env->body,1,len_tot+i,"-");
        }
      }

      ufu_wrefresh(env->body);

    }

    g=tos;
    row=1;
    pos=len_tot-1;
    len_text=0;

    while(row<=rows) {

      if(g!=NULL) {

        ufu_clear_string(env,s,UFU_MAX_TEXT);

        if(g==cos)  wattron(env->body,A_REVERSE);

        mvwprintw(env->body,row+1,1,"%5d",g->gid);

	// Groupname.
        mvwprintw(env->body,row+1,7,"%s",g->gname);
        for(i=strlen(g->gname);i<len_gname;i++) {
          mvwprintw(env->body,row+1,7+i," ");
        }

        // Users.
        c=0;
        u=env->ufirst;
        while(u!=NULL) {
          if(u->gid==g->gid) {
            if(c>0) {
              strcat(s,",");
              strcat(s,u->uname);
            }
            else {
              strcpy(s,u->uname);
            }
            strcat(s,"!");
            c++;
          }
          u=u->next;
        }

        gu=g->fgrpusr;
        while(gu!=NULL) {
          if(c>0) {
            strcat(s,",");
            strcat(s,gu->uname);
          }
          else {
            strcpy(s,gu->uname);
          }
          c++;
          gu=gu->next;
        }

        if(strlen(s)>len_text) {
          len_text=strlen(s);
        }

        if(strlen(s)<(env->cols-pos)) {
          if(offset<(strlen(s)-1)) {
            mvwprintw(env->body,row+1,pos+1,"%s",s+offset);
            for(i=pos-offset+strlen(s)+1;i<(env->cols-1);i++) {
              if(i>=pos) {
                mvwprintw(env->body,row+1,i," ");
              }
            }
          }
        }
        else {
          if(g==cos)  wattroff(env->body,A_REVERSE);
          mvwprintw(env->body,row+1,pos,"!");
          if(g==cos)  wattron(env->body,A_REVERSE);
          if(offset<(strlen(s)-1)) {
            for(i=pos+1;i<(env->cols-1);i++) {
              mvwprintw(env->body,row+1,i,"%c",s[i-pos-1+offset]);
            }
          }
        }

        if(g==cos)  wattroff(env->body,A_REVERSE);

        g=g->next;

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

      case UFU_KEY_0:
        sort_gid=!sort_gid;
        ufu_sort_g(env,sort_gid);
        cos=g=env->gfirst;
        tos=g=env->gfirst;
        seqno=env->gfirst->seqno;
        break;

      case UFU_KEY_LOG:
        env->key_view++;
	ufu_view_log(env);
        break;

      case UFU_KEY_HELP:
      case UFU_KEY_F1:
        env->key_help++;
        ufu_help(env,env->mpanel+1,UFU_HELP_G);
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
        tos=env->gfirst;
        cos=env->gfirst;
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

void ufu_sort_g(struct s_env *env, int sort) {

  struct s_grp *g1,*g2;

  strcpy(env->fun,"ufu_sort_g");
  ufu_wai(env);

  sprintf(env->msg,"Start sorting groups");
  ufu_log(env);

  g1=env->gfirst;
  while(g1!=NULL) {

    g2=env->gfirst;
    while((g2!=NULL)&&(g2->seqno<g1->seqno)) {

      if(sort) {
        if(g2->gid>g1->gid) {
          ufu_sort_g_swap(env,g1,g2);
        }
      }
      else {
        if(strcmp(g2->gname,g1->gname)>0) {
          ufu_sort_g_swap(env,g1,g2);
        }
      }

      g2=g2->next;

    }

    g1=g1->next;
  }

  sprintf(env->msg,"Finished sorting groups");
  ufu_log(env);

}

void ufu_sort_g_swap(struct s_env *env,struct s_grp *g1,struct s_grp *g2) {

  struct s_grp *t;

  strcpy(env->fun,"ufu_sort_g_swap");
  ufu_wai(env);

  t=ufu_alloc_grp(env);

  t->gid=g2->gid;
  strcpy(t->gname,g2->gname);
  // t->fgrpusr=g2->fgrpusr;

  g2->gid=g1->gid;
  strcpy(g2->gname,g1->gname);
  // g2->fgrpusr=g1->fgrpusr;

  g1->gid=t->gid;
  strcpy(g1->gname,t->gname);
  // g1->fgrpusr=g2->fgrpusr;

  ufu_free_grp(env,t);

}
