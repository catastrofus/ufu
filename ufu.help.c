
#include "ufu.h"
#include "ufu.p"

void ufu_help(struct s_env *env,int panel,int tag) {

  int found,again,redraw,offset;
  int key,i,pos,len_txt,row,rows;
  int seq_tos,seq_bos;
  struct s_hlptag *a;
  struct s_hlptxt *x,*cos,*tos,*t;

  strcpy(env->fun,"ufu_help");
  ufu_wai(env);

  sprintf(env->msg,"Looking for tag %d.",tag);
  ufu_log(env);

  if(env->tagfirst!=NULL) {

    // Find tag.
    found=FALSE;
    a=env->tagfirst;
    while((a!=NULL)&&(!found)) {
      found=((a->tag)==tag);
      if(!found) {
        a=a->next;
      }
    }
    if(found) {

      sprintf(env->msg,"Found tag %d.",tag);
      ufu_log(env);

      // Txt available?
      if(a->fhlptxt!=NULL) {

        sprintf(env->msg,"Found txt for tag %d.",tag);
        ufu_log(env);

        x=a->fhlptxt;
	offset=0;

        tos=x;
        cos=x;

        t=x;

        rows=env->rows-6;

        redraw=TRUE;
        again=(x!=NULL);

        while(again) {

          if(redraw) {

            ufu_wclear(env->top);
            mvwprintw(env->top,0,0,"[%s] %s",env->nodename,a->txt);
            if(offset>0) {
              mvwprintw(env->top,0,env->cols-12,"Offset: %d",offset);
            }

            ufu_wrefresh(env->top);

            ufu_wclear(env->bottom);
            ufu_wrefresh(env->bottom);

            ufu_wclear(env->body);
            ufu_wrefresh(env->body);

            pos=0;

          }

          t=tos;
          row=0;

          while(row<=rows) {

            if(t!=NULL) {

              if(t==cos)  wattron(env->body,A_REVERSE);

              pos=0;

              // Text.
              if(strlen(t->txt)<(env->cols-pos)) {
                if(offset<(strlen(t->txt)-1)) {
                  mvwprintw(env->body,row,pos+1,"%s",t->txt+offset);
                  for(i=pos-offset+strlen(t->txt)+1;i<(env->cols-1);i++) {
                    mvwprintw(env->body,row,i," ");
                  }
                }
              }
              else {
                if(t==cos)  wattroff(env->body,A_REVERSE);
                mvwprintw(env->body,row,pos,"!");
                if(t==cos)  wattron(env->body,A_REVERSE);
                if(offset<(strlen(t->txt)-1)) {
                  for(i=pos+1;i<(env->cols-1);i++) {
                    mvwprintw(env->body,row,i,"%c",t->txt[i-pos-1+offset]);
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
            case UFU_KEY_MARK:
              again=FALSE;
              break;

            case UFU_KEY_LEFT:
              env->key_left++;
              if(offset>0) {
                offset--;
              }
              break;

            case UFU_KEY_RIGHT:
              env->key_right++;
              if(offset<(len_txt-1)) {
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
              tos=a->fhlptxt;
              cos=a->fhlptxt;
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

            default:
              ufu_wrong_key(env);
              break;

          }

        }

      }
      else {
        sprintf(env->msg,"No txt found for tag %d!",tag);
        ufu_log(env);
      }
    }
    else {
      sprintf(env->msg,"Tag %d NOT found!",tag);
      ufu_log(env);
    }
  }

}

void ufu_read_hlp(struct s_env *env) {

  int tag,found,seqno_tag,seqno_txt;
  char th,*txt,*line1,*line2;
  FILE *fp;
  struct s_hlptag *a,*b;
  struct s_hlptxt *x;

  strcpy(env->fun,"ufu_read_hlp");
  ufu_wai(env);

  if((fp=fopen(UFU_GLOBAL_HELP,"r"))!=NULL) {

    sprintf(env->msg,"Cleaning structures");
    ufu_log(env);

    a=env->tagfirst;
    while(a!=NULL) {
      b=a->next;
      ufu_free_hlptag(env,a);
      a=b;
    }
    env->tagfirst=NULL;

    sprintf(env->msg,"Reading helpfile (%s).",UFU_GLOBAL_HELP);
    ufu_log(env);

    seqno_tag=0;
    seqno_txt=0;

    line1=ufu_alloc_string(env,UFU_LEN_MSG);

    while(fgets(line1,UFU_LEN_MSG,fp)!=NULL) {

      if((line1[0]!='#')&&(line1[0]!='\n')) {

        line2=strtok(line1,":");

        tag=atoi(line2);

        line2=strtok(NULL,":");
        th=line2[0];

        txt=strtok(NULL,":");
	txt[strlen(txt)-1]='\0';

        switch(th) {
          case 't':
            sprintf(env->msg," Found tag '%d' (%s)(%d)",tag,txt,seqno_tag);
	    ufu_log(env);
            a=env->tagfirst;
            if(a!=NULL) {
	      while(a->next!=NULL) {
	        a=a->next;
	      }
	      a->next=ufu_alloc_hlptag(env,strlen(txt)+2);
	      a->next->seqno=seqno_tag++;
	      a->next->tag=tag;
	      strcpy(a->next->txt,txt);
	      a->next->prev=a;
            }
            else {
	      a=ufu_alloc_hlptag(env,strlen(txt)+2);
	      a->seqno=seqno_tag++;
	      a->tag=tag;
	      strcpy(a->txt,txt);
	      env->tagfirst=a;
            }
            break;
	  case 'h':
            sprintf(env->msg,"  Found text \"%s\" (%d)(%d)",txt,tag,seqno_txt);
	    ufu_log(env);
            found=FALSE;
            a=env->tagfirst;
            if(a!=NULL) {
	      while((a!=NULL)&&(!found)) {
	        found=((a->tag)==tag);
		if(!found) {
		  a=a->next;
		}
	      }
              if(found) {
                x=a->fhlptxt;
	        if(x!=NULL) {
                  while(x->next!=NULL) {
                    x=x->next;
		  }
                  x->next=ufu_alloc_hlptxt(env,strlen(txt)+2);
		  x->next->seqno=seqno_txt++;
		  strcpy(x->next->txt,txt);
		  x->next->prev=x;
                }
	        else {
                  x=ufu_alloc_hlptxt(env,strlen(txt)+2);
		  x->seqno=seqno_txt++;
		  strcpy(x->txt,txt);
		  a->fhlptxt=x;
                }
              }
            }
	    break;
	  default:
	    sprintf(env->msg,"Huh? No 't' or 'h'?");
	    ufu_log(env);
	    break;
        }
      }
      else {
        line1[strlen(line1)-1]='\0';
        sprintf(env->msg,"  Skipping line \"%s\".",line1);
        ufu_log(env);
      }
    }

    ufu_free_string(env,line1);

    fclose(fp);

    sprintf(env->msg,"Reading helpfile finished.");
    ufu_log(env);

    sprintf(env->msg,"Summary helpfile:");
    ufu_log(env);
    a=env->tagfirst;
    while(a!=NULL) {
      sprintf(env->msg,"Tag %d: %s",a->tag,a->txt);
      ufu_log(env);
      x=a->fhlptxt;
      while(x!=NULL) {
	sprintf(env->msg,"Tag %d, seqno %d: %s",a->tag,x->seqno,x->txt);
	ufu_log(env);
	x=x->next;
      }
      a=a->next;
    }

  }
  else {

    sprintf(env->msg,"Unable to open help \"%s\"!",UFU_GLOBAL_HELP);
    ufu_log(env);

  }

}

