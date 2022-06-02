
#include "ufu.h"
#include "ufu.p"

void ufu_init_curses(struct s_env *env) {

  // Curses initialization.
  initscr();
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr,FALSE);
  keypad(stdscr,TRUE);

  strcpy(env->fun,"ufu_init_curses");
  ufu_wai(env);

  // Create the windows.
  env->top=newwin(1,env->cols,0,0);
  env->top_sep=newwin(1,env->cols,1,0);
  env->body=newwin(env->rows-4,env->cols,2,0);
  env->bottom_sep=newwin(1,env->cols,env->rows-2,0);
  env->bottom=newwin(1,env->cols,env->rows-1,0);

  keypad(env->bottom,TRUE);

}

void ufu_deinit_curses(struct s_env *env) {

  strcpy(env->fun,"ufu_deinit_curses");
  ufu_wai(env);

  // Delete the used windows.
  delwin(env->top);
  delwin(env->top_sep);
  delwin(env->body);
  delwin(env->bottom_sep);
  delwin(env->bottom);

  // Curses de-initialization.
  endwin();

}

void ufu_clear(struct s_env *env) {

  int i;

  strcpy(env->fun,"ufu_clear");
  ufu_wai(env);

  ufu_wclear(env->top);
  ufu_wclear(env->body);
  ufu_wclear(env->bottom);

  ufu_wrefresh(env->top);
  ufu_wrefresh(env->body);
  ufu_wrefresh(env->bottom);

  for(i=0;i<env->cols;i++) {
    mvwprintw(env->top_sep,0,i,"=");
    mvwprintw(env->bottom_sep,0,i,"=");
    ufu_wrefresh(env->top_sep);
    ufu_wrefresh(env->bottom_sep);
  }

}

void ufu_wclear(WINDOW *w) {

  wclear(w);

}

void ufu_wrefresh(WINDOW *w) {

  wrefresh(w);

}

WINDOW *ufu_popup(struct s_env *env,int width,int height,int c,int r,char *t) {

  int i;
  WINDOW *w;

  strcpy(env->fun,"ufu_popup");
  ufu_wai(env);

  w=newwin(height,width,r,c);

  ufu_wclear(w);
  mvwprintw(w,0,0,"+");
  mvwprintw(w,0,width-1,"+");
  mvwprintw(w,height-1,0,"+");
  mvwprintw(w,height-1,width-1,"+");

  for(i=1;i<(width-1);i++) {
    mvwprintw(w,0,i,"-");
    mvwprintw(w,height-1,i,"-");
  }

  for(i=1;i<(height-1);i++) {
    mvwprintw(w,i,0,"|");
    mvwprintw(w,i,width-1,"|");
  }

  if(strlen(t)>0) {
    mvwprintw(w,0,(width-(strlen(t)+2))/2," %s ",t);
  }

  ufu_wrefresh(w);

  return(w);

}

void ufu_popdown(struct s_env *env,WINDOW *w) {

  strcpy(env->fun,"ufu_popdown");
  ufu_wai(env);

  ufu_wrefresh(w);
  delwin(w);

  ufu_wrefresh(env->body);

}

