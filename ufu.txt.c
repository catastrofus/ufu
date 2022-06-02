
#include "ufu.h"
#include "ufu.p"

void ufu_show_txt(struct s_env *env,int panel,char *s) {

  int rem,again,changed,redraw,key,len,resizelen,start,row,col,rows,seqno,seq_tos,seq_bos,i,r;
  char *s1,*s2;
  struct s_txt *uc,*cos,*tos,*uc1;

  strcpy(env->fun,"ufu_show_txt");
  ufu_wai(env);

  resizelen=(env->cols-33-strlen(env->nodename))/2;
  s1=ufu_alloc_string(env,resizelen+1);
  s2=ufu_alloc_string(env,resizelen+1);
  ufu_resize_string(env,env->panel[panel]->dirname,resizelen,s1);
  ufu_resize_string(env,s,resizelen,s2);

  tos=env->tfirst;
  cos=env->tfirst;

  uc=env->tfirst;

  rows=env->rows-4;

  uc=env->tfirst;

  changed=FALSE;
  redraw=TRUE;
  again=(uc!=NULL);

  start=0;

  while(again) {

    if(redraw) {

      ufu_wclear(env->top);
      mvwprintw(env->top,0,0,"[%s] Show output usercommand: %s (%s)",env->nodename,s2,s1);

      ufu_wrefresh(env->top);
      ufu_wrefresh(env->body);

    }

    uc=tos;
    row=0;

    ufu_wclear(env->body);

    while(row<rows) {

      if(uc!=NULL) {

        if(uc==cos)  wattron(env->body,A_REVERSE);

        len=strlen(uc->line);
        i=0;
        while(i<(env->cols-2)) {

          if((i+start)<len) {
            mvwprintw(env->body,row,1+i,"%c",uc->line[i+start]);
          }
          else {
            mvwprintw(env->body,row,1+i," ");
          }

          i++;

        }

        if(uc==cos)  wattroff(env->body,A_REVERSE);

        uc=uc->next;

      }

      row++;

    }

    ufu_wrefresh(env->body);

    key=ufu_get_key(env,UFU_NO_TEXT,NULL);

    switch(key) {

      case UFU_KEY_QUIT:
        again=FALSE;
        break;

      case UFU_KEY_LOG:
        env->key_view++;
	ufu_view_log(env);
        break;

      case UFU_KEY_HELP:
      case UFU_KEY_F1:
        env->key_help++;
        ufu_help(env,env->mpanel+1,UFU_HELP_UCMD);
        redraw=TRUE;
        break;

      case UFU_KEY_DOWN:
        env->key_down++;
        if(cos->next!=NULL) {
          seq_bos=seq_tos+rows-6;
          cos=cos->next;
	  if((cos->seqno-tos->seqno)>=rows) {
	    tos=cos;
	  }
        }
        redraw=TRUE;
        break;

      case UFU_KEY_UP:
        env->key_up++;
        if(cos->prev!=NULL) {
          cos=cos->prev;
	  if(cos->seqno<tos->seqno) {
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
        tos=env->tfirst;
        cos=env->tfirst;
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

      case UFU_KEY_SETTING:
      case UFU_KEY_F3:
        env->key_setting++;
        ufu_show_setting(env);
        redraw=TRUE;
        break;

      case UFU_KEY_CONFIG:
      case UFU_KEY_F4:
        env->key_config++;
        ufu_show_config(env);
        redraw=TRUE;
        break;

      default:
        ufu_wrong_key(env);
        break;

    }

  }

  ufu_free_string(env,s1);
  ufu_free_string(env,s2);

}

