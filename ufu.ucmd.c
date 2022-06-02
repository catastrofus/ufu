
#include "ufu.h"
#include "ufu.p"

void ufu_write_ucmd(struct s_env *env) {

  char *fname;
  struct s_ucmd *uc;
  FILE *fp;

  strcpy(env->fun,"ufu_write_ucmd");
  ufu_wai(env);

  fname=ufu_alloc_string(env,UFU_LEN_NAME);
  strcpy(fname,env->homedir);
  strcat(fname,"/");
  strcat(fname,env->sh_hfile);

  sprintf(env->msg,"Writing usercommands to file %s.",fname);
  ufu_log(env);

  if((fp=fopen(fname,"w"))!=NULL) {
    uc=env->ucfirst;
    while(uc!=NULL) {
      sprintf(env->msg,"  Seqno %04d, uc: %s",uc->seqno,uc->exec);
      ufu_log(env);
      fprintf(fp,"%s\n",uc->exec);
      uc=uc->next;
    }
    fclose(fp);
  }

  free(fname);

}

int ufu_edit_ucmd(struct s_env *env,struct s_ucmd *uc,int scope,int row,int col,int insert) {

  int ok;

  strcpy(env->fun,"ufu_edit_ucmd");
  ufu_wai(env);

  ok=ufu_rl(env,env->body,col,row,insert,UFU_LEN_UCMD,env->cols-12,uc->exec,TRUE,TRUE);

  return(ok);

}

void ufu_add_ucmd(struct s_env *env,struct s_ucmd *uc) {

  struct s_ucmd *uc1,*uc2;

  strcpy(env->fun,"ufu_add_ucmd");
  ufu_wai(env);

  uc1=ufu_alloc_ucmd(env);
  strcpy(uc1->exec,uc->exec);
  uc1->local=uc->local;
  uc1->key=uc->key;
  uc1->history=uc->history;

  if(env->ucfirst!=NULL) {

    uc2=env->ucfirst;
    while(uc2->next!=NULL) {
      uc2=uc2->next;
    }
    uc2->next=uc1;
    uc1->prev=uc2;
    uc1->next=NULL;
    env->uclast=uc1;

  }
  else {

    uc1->seqno=0;
    uc1->next=NULL;
    uc1->prev=NULL;
    env->ucfirst=uc1;
    env->uclast=uc1;

  }

  if(env->debug) {
    sprintf(env->msg,"UserCMD: Add new usercommand %d (%s).",uc1->seqno,uc1->exec);
  }

}

int ufu_rem_ucmd(struct s_env *env,struct s_ucmd *uc) {

  int seqno;

  strcpy(env->fun,"ufu_rem_ucmd");
  ufu_wai(env);

  seqno=uc->seqno;

  if(uc->prev==NULL) {
    // First entry.
    if(uc->next==NULL) {
      // And no entry left.
      env->ucfirst=NULL;
      env->uclast=NULL;
    }
    else {
      // Entry is the first of more entries.
      env->ucfirst=uc->next;
      uc->next->prev=NULL;
    }
  }
  else {
    // Not the first entry.
    if(uc->next==NULL) {
      // Last entry.
      env->uclast=uc->prev;
      uc->prev->next=NULL;
    }
    else {
      // Entry is one of more entries.
      uc->next->prev=uc->prev;
      uc->prev->next=uc->next;
    }
  }

  ufu_free_ucmd(env,uc);
  ufu_renum_ucmd(env);

  return(TRUE);
}

void ufu_exec_ucmd(struct s_env *env,struct s_ucmd *uc,int panel,char *file) {

  int replaced;
  char *exec,*exec_tmp,*fn,*p;

  strcpy(env->fun,"ufu_exec_ucmd");
  ufu_wai(env);

  exec_tmp=ufu_alloc_string(env,UFU_LEN_UCMD);
  exec=ufu_alloc_string(env,UFU_LEN_UCMD);
  fn=ufu_alloc_string(env,UFU_LEN_NAME);

  replaced=0;

  strcpy(exec,uc->exec);

  // Construct absolute filename.
  ufu_concat_dir(env,fn,env->panel[panel]->dirname,file);

  if(strstr(exec,"{A}")!=NULL) {
    sprintf(env->msg,"UserCMD {A} replacement, command is: %s",exec);
    ufu_log(env);
    // ABSOLUTE filename replacement.
    while((p=strstr(exec,"{A}"))!=NULL) {
      replaced++;
      // Init var 'exec_tmp' with \0.
      ufu_clear_string(env,exec_tmp,UFU_LEN_UCMD);
      if(p!=exec) {
        strncat(exec_tmp,exec,(p-exec));
        strcat(exec_tmp,fn);
        strcat(exec_tmp,p+3);
      }
      else {
        strcat(exec_tmp,fn);
        strcat(exec_tmp,p+3);
      }
      sprintf(env->msg," UserCMD {A} replacement: %s",exec_tmp);
      ufu_log(env);
      strcpy(exec,exec_tmp);
    }
    sprintf(env->msg,"UserCMD {A} replacement, command is: %s",exec);
    ufu_log(env);
  }

  if(strstr(exec,"{R}")!=NULL) {
    sprintf(env->msg,"UserCMD {R} replacement, command is: %s",exec);
    ufu_log(env);
    // RELATIVE filename replacement.
    while((p=strstr(exec,"{R}"))!=NULL) {
      replaced++;
      // Init var 'exec_tmp' with \0.
      ufu_clear_string(env,exec_tmp,UFU_LEN_UCMD);
      if(p!=exec) {
        strncat(exec_tmp,exec,(p-exec));
        strcat(exec_tmp,file);
        strcat(exec_tmp,p+3);
      }
      else {
        strcat(exec_tmp,file);
        strcat(exec_tmp,p+3);
      }
      sprintf(env->msg," UserCMD {R} replacement: %s",exec_tmp);
      ufu_log(env);
      strcpy(exec,exec_tmp);
    }
    sprintf(env->msg,"UserCMD {R} replacement, command is: %s",exec);
    ufu_log(env);
  }

  if(strstr(exec,"{B}")!=NULL) {
    sprintf(env->msg,"UserCMD {B} replacement, command is: %s",exec);
    ufu_log(env);
    // BASENAME replacement.
    while((p=strstr(exec,"{B}"))!=NULL) {
      replaced++;
      // Init var 'exec_tmp' with \0.
      ufu_clear_string(env,exec_tmp,UFU_LEN_UCMD);
      if(p!=exec) {
        strncat(exec_tmp,exec,(p-exec));
        strcat(exec_tmp,env->panel[panel]->dirname);
        strcat(exec_tmp,p+3);
      }
      else {
        strcat(exec_tmp,env->panel[panel]->dirname);
        strcat(exec_tmp,p+3);
      }
      sprintf(env->msg," UserCMD {B} replacement: %s",exec_tmp);
      ufu_log(env);
      strcpy(exec,exec_tmp);
    }
    sprintf(env->msg,"UserCMD {B} replacement, command is: %s",exec);
    ufu_log(env);
  }

  if(replaced>0) {
    sprintf(env->msg,"UserCMD, %d replacements!",replaced);
    ufu_log(env);
  }
  else {
    sprintf(env->msg,"UserCMD, no replacements.");
    ufu_log(env);
    strcpy(exec,uc->exec);
    //strcat(cmd,"; read xyz");
  }

  sprintf(env->msg,"Executing usercommand from panel %d: %s.",panel,exec);
  ufu_log(env);

  ufu_cmd_exec(env,panel,exec);

  ufu_free_string(env,fn);
  ufu_free_string(env,exec);
  ufu_free_string(env,exec_tmp);

}

void ufu_cmd_exec_key(struct s_env *env,int key,int panel,char *file) {

  struct s_ucmd *uc;

  strcpy(env->fun,"ufu_cmd_exec_key");
  ufu_wai(env);

  uc=env->ucfirst;

  while(uc!=NULL) {
    if(uc->seqno==key) {
      if(strlen(uc->exec)>0) {
        ufu_exec_ucmd(env,uc,panel,file);
      }
    }
    uc=uc->next;
  }

}

void ufu_show_ucmd(struct s_env *env,int panel,char *file) {

  int rem,again,changed,redraw,key,len,start,row,col,rows,seqno,seq_tos,seq_bos,i,r;
  struct s_ucmd *uc,*cos,*tos,*uc1;

  strcpy(env->fun,"ufu_show_ucmd");
  ufu_wai(env);

  tos=env->ucfirst;
  cos=env->ucfirst;

  uc=env->ucfirst;

  rows=env->rows-6;

  seqno=env->hist_ucmd;
  uc=env->ucfirst;
  while((uc!=NULL)&&(uc->seqno!=seqno)) {
    uc=uc->next;
  }

  if(uc!=NULL) {
    cos=uc;
    tos=uc;

    r=rows/2;
    while((uc->prev!=NULL)&&(r>0)) {
      r--;
      tos=uc;
      uc=uc->prev;
    }
 
  }
  else {
    uc=env->ucfirst;
  }

  changed=FALSE;
  redraw=TRUE;
  again=(uc!=NULL);

  start=0;

  while(again) {

    if(redraw) {

      ufu_wclear(env->top);
      mvwprintw(env->top,0,0,"[%s] Show usercommands",env->nodename);

      ufu_wrefresh(env->top);

      ufu_wclear(env->bottom);
      ufu_wrefresh(env->bottom);

      ufu_wclear(env->body);
      mvwprintw(env->body,0,1,"SeqNo Key Command");
      mvwprintw(env->body,1,1,"----- ---");
      i=0;
      while(i<(env->cols-12)) {
        mvwprintw(env->body,1,11+i,"-");
        i++; 
      }
      ufu_wrefresh(env->body);

    }

    uc=tos;
    row=1;

    while(row<=rows) {

      if(uc!=NULL) {

        if(uc==cos)  wattron(env->body,A_REVERSE);

        mvwprintw(env->body,row+1,1,"%5d",uc->seqno);

        if(uc->key!=' ') {
          mvwprintw(env->body,row+1,8,"%c",uc->key);
        }

        len=strlen(uc->exec);
        i=0;
        while(i<(env->cols-12)) {

          if((i+start)<len) {
            mvwprintw(env->body,row+1,11+i,"%c",uc->exec[i+start]);
          }
          else {
            mvwprintw(env->body,row+1,11+i," ");
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
        if(cos->key==key) {
          sprintf(env->msg,"UserCMD %c reset, seqno %d, command \"%s\"",cos->key,cos->seqno,cos->exec);
          ufu_log(env);
          cos->key=' ';
        }
        else {
          uc1=env->ucfirst;
          while(uc1!=NULL) {
            sprintf(env->msg,"%4d-%c-%s",uc1->seqno,uc1->key,uc1->exec);
            ufu_log(env);
            if(uc1->key==key) {
              uc1->key=' ';
            }
            uc1=uc1->next;
          }
          cos->key=key;
          sprintf(env->msg,"UserCMD %c, seqno %d, command \"%s\"",cos->key,cos->seqno,cos->exec);
          ufu_log(env);
        }
        break;

      case UFU_KEY_QUIT:
        ufu_add_hist(env,UFU_HIST_UCMD,NULL,cos->seqno);
        again=FALSE;
        if(changed) {
            ufu_write_config(env);
        }
        ufu_write_ucmd(env);
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
        break;

      case UFU_KEY_SELECT:
      case UFU_KEY_ENTER:
        env->key_select++;
        if(cos->seqno>0) {
          ufu_add_hist(env,UFU_HIST_UCMD,NULL,cos->seqno);
          // Execute usercommand.
          ufu_cmd_exec_key(env,cos->seqno,panel,file);
        }
        break;

      case UFU_KEY_HELP:
      case UFU_KEY_F1:
        env->key_help++;
        ufu_help(env,env->mpanel+1,UFU_HELP_UCMD);
        redraw=TRUE;
        break;

      case UFU_KEY_LEFT:
        env->key_left++;
        if(start>=UFU_LEN_RL_STEP) {
          start=start-UFU_LEN_RL_STEP;
        }
        break;

      case UFU_KEY_RIGHT:
        env->key_right++;
        if(start<(UFU_LEN_UCMD-env->cols-12-UFU_LEN_RL_STEP)) {
          start=start+UFU_LEN_RL_STEP;
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
        tos=env->ucfirst;
        cos=env->ucfirst;
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

      case UFU_KEY_DELETE:
      case UFU_KEY_ERASE:
        env->key_rem_ucmd++;
        if(cos->seqno>0) {
          // Delete usercommand.
          rem=!env->confirmremove;
          if(env->confirmremove) {
            sprintf(env->msg,"%s, are you sure to remove this humble (%d) entry? ",env->master,cos->seqno);
            rem=(ufu_get_yn(env));
          }
          if(rem) {
            changed=TRUE;
            uc=cos;
            if(cos->next!=NULL) {
              seq_tos=tos->seqno;
              seq_bos=seq_tos+rows-1;
              if(seq_bos>env->uclast->seqno) {
                seq_bos=env->uclast->seqno;
              }
              if(tos==cos) {
                cos=cos->next;
                tos=cos;
                seq_tos=tos->seqno;
              }
              else {
                cos=cos->next;
              }
              if(cos->seqno>seq_bos) {
                tos=cos;
              }
            }
            else {
              if(cos->prev!=NULL) {
                cos->prev->next=NULL;
                cos=env->ucfirst;
                while(cos->next!=NULL) {
                  cos=cos->next;
                }
                tos=cos;
                i=0;
                while((i<(rows-1))&&(tos->prev!=NULL)) {
                  tos=tos->prev;
                  i++;
                }
              }
              else {
                again=FALSE;
              }
            }
            ufu_rem_ucmd(env,uc);
            if((changed)&&(!again)) {
              ufu_write_config(env);
            }
            ufu_write_ucmd(env);
          }
        }
        break;

      case UFU_KEY_INSERT:
      case UFU_KEY_APPEND:
        env->key_add_ucmd++;
        // Add new usercommand.
        redraw=TRUE;
        uc=ufu_alloc_ucmd(env);
        strcpy(uc->exec,"");
        uc->local=TRUE;
        uc->key=' ';
        uc->history=FALSE;
        ufu_add_ucmd(env,uc);
        ufu_free_ucmd(env,uc);
        ufu_renum_ucmd(env);

        cos=env->uclast;
        tos=cos;
        i=0;
        while((i<(rows-1))&&(tos->prev!=NULL)) {
          tos=tos->prev;
          i++;
        }

        uc=tos;
        row=1;
        while(row<=rows) {
          if(uc!=NULL) {
            mvwprintw(env->body,row+1,1,"%5d",uc->seqno);
            if(uc==cos)  wattron(env->body,A_REVERSE);
            len=strlen(uc->exec);
            i=0;
            while(i<(env->cols-12)) {
              if((i+start)<len) {
                mvwprintw(env->body,row+1,7+i,"%c",uc->exec[i+start]);
              }
              else {
                mvwprintw(env->body,row+1,7+i," ");
              }
              i++;
            }
            if(uc==cos)  wattroff(env->body,A_REVERSE);
            uc=uc->next;
          }
          row++;
        }
        ufu_wrefresh(env->body);

        col=7;
        changed=ufu_edit_ucmd(env,cos,cos->local,2+(cos->seqno)-(tos->seqno),col,TRUE);
        if(strlen(cos->exec)==0) {
          uc=cos;
          if(uc->next!=NULL) {
            uc=uc->next;
          }
          else {
            if(uc->prev!=NULL) {
              uc=uc->prev;
            }
          }
          ufu_rem_ucmd(env,cos);
          cos=uc;
          changed=FALSE;
        }
        ufu_write_ucmd(env);

        break;

      case UFU_KEY_EDIT:
        env->key_edit++;
        if(cos->seqno>0) {
          // Edit usercommand.
          if(cos->local) {
            col=11;
            ufu_edit_ucmd(env,cos,cos->local,2+(cos->seqno)-(tos->seqno),col,FALSE);
            if(strlen(cos->exec)==0) {
              ufu_rem_ucmd(env,cos);
            }
            changed=TRUE;
            ufu_write_ucmd(env);
          }
        }
        break;

      default:
        ufu_wrong_key(env);
        break;

    }

  }

}

void ufu_renum_ucmd(struct s_env *env) {

  int c,k;
  struct s_ucmd *uc;

  sprintf(env->msg," Renumbering usercommands.");
  ufu_log(env);
  c=0;
  k=0;
  uc=env->ucfirst;
  while(uc!=NULL) {
    uc->seqno=c++;
    sprintf(env->msg,"  Seqno %04d, uc: %s",uc->seqno,uc->exec);
    strcpy(env->fun,"ufu_renum_ucmd");
    ufu_log(env);
    if(uc->next==NULL) {
      env->uclast=uc;
    }
    if(uc->key!=' ') {
      sprintf(env->msg,"  Key %c, uc: %s",uc->key,uc->exec);
      ufu_log(env);
      k++;
    }
    uc=uc->next;
  }
  sprintf(env->msg," Total %d usercommands, %d with key.",c-1,k);
  ufu_log(env);
  sprintf(env->msg," Finished renumbering usercommands.");
  ufu_log(env);

}


