
#include "ufu.h"
#include "ufu.p"

char *ufu_rl2(struct s_env *env,WINDOW *w,int c,int r,int size,int visible,char *s) {

  int i;
  int quit,key,col,len;

  strcpy(env->fun,"ufu_rl2");
  ufu_wai(env);

  if(strlen(s)>0) {
    col=strlen(s);
  }
  else {
    col=0;
  }

  quit=FALSE;
  while(!quit) {

    len=strlen(s);

    i=0;
    while(i<size) {

      if(i==col) {
        wattron(w,A_REVERSE);
      }

      if(visible) {
        if(i<len) {
          mvwprintw(w,r,c+i,"%c",s[i]);
        }
        else {
          mvwprintw(w,r,c+i," ");
        }
      }
      else {
        if(i<len) {
          mvwprintw(w,r,c+i,"*");
	}
	else {
          mvwprintw(w,r,c+i," ");
        }
      }

      if(i==col) {
        wattroff(w,A_REVERSE);
      }

      i++;

    }

    wrefresh(w);

    mvwprintw(env->top,0,env->cols-12,"Pos         ");
    mvwprintw(env->top,0,env->cols-12,"Pos %d/%d",col+1,size);
    wrefresh(env->top);

    key=ufu_get_key(env,UFU_NO_TEXT,NULL);

    switch(key) {

      case UFU_KEY_RL_SELECT:
      case UFU_KEY_RL_ENTER:
	// <enter>.
        quit=TRUE;

	// Get rid of (block)cursor.
        i=0;
        while(i<size) {
          if(visible) {
            if(i<len) {
              mvwprintw(w,r,c+i,"%c",s[i]);
            }
            else {
              mvwprintw(w,r,c+i," ");
            }
          }
          else {
            if(i<len) {
              mvwprintw(w,r,c+i,"*");
	    }
	    else {
              mvwprintw(w,r,c+i," ");
            }
          }
          i++;
        }
        wrefresh(w);

        break;

//    case UFU_KEY_HELP:
//      ufu_help(env,env->mpanel+1,UFU_HELP_RL);
//      break;

      case UFU_KEY_LEFT:
	// Key left.
	sprintf(env->msg," Toets LINKS");
	ufu_log(env);
	if(col>0) {
          col--;
	}

	break;

      case UFU_KEY_RIGHT:
	// Key right.
	sprintf(env->msg," Toets RECHTS");
	ufu_log(env);
        if(col<len) {
          if(len<size) {
            col++;
	  }
        }

	break;

      case UFU_KEY_BACKSPACE:
        // Delete char before cursor.
	if(col>0) {
          for(i=col;i<len;i++) {
            s[i-1]=s[i];
	  }
	  s[len-1]='\0';
	  col--;
	}

        break;

      case UFU_KEY_DELETE:
        // Delete char under cursor.
        for(i=col+1;i<len;i++) {
          s[i-1]=s[i];
        }
        s[len-1]='\0';

	break;

      default:
        // Insert char at current position.
	if(len<size) {
          for(i=len;i>col;i--) {
            s[i]=s[i-1];
	  }
	  s[len+1]='\0';
	  s[col]=key;
	  col++;
	}
	else {
          col=len-1;
        }

	break;

    }

  }

  if(strlen(s)>0) {
    return(s);
  }
  else {
    return(NULL);
  }

}

