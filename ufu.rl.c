
#include "ufu.h"
#include "ufu.p"

int ufu_rl(struct s_env *env,WINDOW *w,int c,int r,int insert,int abssize,int relsize,char *s_init,int visible,int show_text) {

  int i,j;
  int quit,key,deliver,col,len,start;
  char *s;

  strcpy(env->fun,"ufu_rl");
  ufu_wai(env);

  s=ufu_alloc_string(env,abssize);
  strcpy(s,s_init);

  wattron(env->body,A_REVERSE);

  // Enable functionkeys.
  keypad(w,TRUE);

  start=0;
  col=0;

  quit=FALSE;
  while(!quit) {

    len=strlen(s);
    i=0;
    while(i<relsize) {

      if(visible) {
        if((i+start)<len) {
          mvwprintw(w,r,c+i+start,"%c",s[i+start]);
        }
        else {
          mvwprintw(w,r,c+i+start," ");
        }
      }
      else {
        mvwprintw(w,r,c+i+start,"*");
      }
      i++;

    }

    if(show_text) {
      if(!insert)
        sprintf(env->msg,"%s, your (editable) wish is my command: ",env->master);
      else
        sprintf(env->msg,"%s, your (insertable) wish is my command: ",env->master);
    }
    else {
      env->msg[0]='\0';
    }

    ufu_msg(env);

    mvwprintw(w,r,col+c,"");

    wrefresh(w);

    key=wgetch(w);
    // key=tolower(wgetch(w));
    len=strlen(s);

    switch(key) {

      case UFU_KEY_RL_QUIT:
        if(!insert) {
          quit=TRUE;
          deliver=FALSE;
        }
        else {
          col=ufu_rl_insert(env,s,key,col,abssize);
        }
	ufu_wrefresh(w);
        break;

      case UFU_KEY_HELP:
        ufu_help(env,env->mpanel+1,UFU_HELP_RL);
        break;

      case UFU_KEY_RL_SELECT:
      case UFU_KEY_RL_ENTER:
        if(!insert) {
          // Return...
          strcpy(s_init,s);
          quit=TRUE;
          deliver=TRUE;
        }
        else {
          insert=FALSE;
        }
        break;

      case UFU_KEY_RL_LEFT0:
      case UFU_KEY_RL_LEFT1:
        // Char left.
        if(!insert) {
          if(col>start) {
            col--;
          }
          else {
            if(start>=UFU_LEN_RL_STEP) {
              start=start-UFU_LEN_RL_STEP;
            }
          }
        }
        else {
          col=ufu_rl_insert(env,s,key,col,abssize);
        }
        break;

      case UFU_KEY_RL_RIGHT0:
      case UFU_KEY_RL_RIGHT1:
        // Char right.
        if(!insert) {
          if(col<(strlen(s)-start)) {
            col++;
          }
          else {
            if((col+start)<len) {
              if(start<(abssize-relsize-UFU_LEN_RL_STEP)) {
                start=start+UFU_LEN_RL_STEP;
              }
            }
          }
        }
        else {
          col=ufu_rl_insert(env,s,key,col,abssize);
        }
        break;

      case UFU_KEY_DOWN:
      case UFU_KEY_RL_FIRST:
        // To begin of string.
        if(!insert) {
          col=0;
        }
        else {
          col=ufu_rl_insert(env,s,key,col,abssize);
        }
        break;

      case UFU_KEY_UP:
      case UFU_KEY_RL_LAST:
        // To end of string.
        if(!insert) {
          col=strlen(s)-1;
        }
        else {
          col=ufu_rl_insert(env,s,key,col,abssize);
        }
        break;

      case UFU_KEY_RL_WLEFT:
        // Word left.
        if(!insert) {
          if((col>0)&&(s[col-1]==' ')) {
            col--;
          }
          while((col>0)&&(s[col]==' ')) {
            col--;
          }
          while((col>0)&&(s[col]!=' ')) {
            col--;
          }
          while((col<(len-1))&&(s[col]==' ')) {
            col++;
          }
        }
        else {
          col=ufu_rl_insert(env,s,key,col,abssize);
        }
        break;

      case UFU_KEY_RL_WRIGHT:
        // Word right.
        if(!insert) {
          while((col<(len-1))&&(s[col]==' ')) {
            col++;
          }
          while((col<(len-1))&&(s[col]!=' ')) {
            col++;
          }
          while((col<(len-1))&&(s[col]==' ')) {
            col++;
          }
        }
        else {
          col=ufu_rl_insert(env,s,key,col,abssize);
        }
        break;

      case UFU_KEY_RL_DELCHAR:
        // Delete char.
        if(!insert) {
          col=ufu_rl_delete(env,s,col);
        }
        else {
          col=ufu_rl_insert(env,s,key,col,abssize);
        }
        break;

      case UFU_KEY_RL_BSPACE:
        // Delete char before cursor.
        if(col>0) {
          col=ufu_rl_delete(env,s,col-1);
        }
        break;

      case UFU_KEY_RL_DEOL:
        // Delete to end of line.
        if(!insert) {
          for(j=col;j<len;j++) {
            s[j]='\0';
          }
        }
        else {
          col=ufu_rl_insert(env,s,key,col,abssize);
        }
        break;

      case UFU_KEY_RL_DBOL:
        // Delete to beginning of line.
        if(!insert) {
          for(j=col;j<len;j++) {
            s[j-col]=s[j];
          }
          for(j=col;j<len;j++) {
            s[j]='\0';
          }
          col=0;
        }
        else {
          col=ufu_rl_insert(env,s,key,col,abssize);
        }
        break;

      default:
        if(!insert) {
          if(key!=UFU_KEY_RL_INSERT)
            ufu_wrong_key(env);
          else
            insert=TRUE;
        }
        else {
          // 0x20=<space>
          // 0x7F=<del>
          if((key>=0x20)&&(key<0x7F)) {
            col=ufu_rl_insert(env,s,key,col,abssize);
            //col=ufu_rl_insert(env,s,key,col,12);
          }
        }

    }

  }

  wattroff(env->body,A_REVERSE);

  ufu_free_string(env,s);

  return(deliver);

}

int ufu_rl_insert(struct s_env *env,char *s,char c,int spot,int maxsize) {

  int i,len;

  strcpy(env->fun,"ufu_rl_insert");
  ufu_wai(env);

  len=strlen(s);

  if(len<maxsize) {
    for(i=len-1;i>=spot;i--) {
      s[i+1]=s[i];
    }
    s[spot]=c;
    s[len+1]='\0';
    spot++;
  }

  return(spot);

}

int ufu_rl_delete(struct s_env *env,char *s,int spot) {

  int j,len;

  strcpy(env->fun,"ufu_rl_delete");
  ufu_wai(env);

  len=strlen(s);
  for(j=spot+1;j<len;j++) {
    s[j-1]=s[j];
  }
  if(len>0) {
    s[len-1]='\0';
  }
  else {
    s[0]='\n';
  }

  return(spot);

}
