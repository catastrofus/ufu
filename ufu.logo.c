
#include "ufu.h"
#include "ufu.p"

void ufu_logo_name(struct s_env *env,int row) {

  char *s;

  strcpy(env->fun,"ufu_logo_name");
  ufu_wai(env);

  ufu_clear(env);

  mvwprintw(env->top,0,(env->cols-20)/2,"an Unix File Utility");

  mvwprintw(env->body,row,(env->cols-10)/2,"Welcome to");

  wattron(env->body,A_BLINK);
  wattron(env->body,A_BOLD);

  mvwprintw(env->body,row+2,(env->cols-17)/2,"U   U FFFFF U   U");
  mvwprintw(env->body,row+3,(env->cols-17)/2,"U   U F     U   U");
  mvwprintw(env->body,row+4,(env->cols-17)/2,"U   U F     U   U");
  mvwprintw(env->body,row+5,(env->cols-17)/2,"U   U FFF   U   U");
  mvwprintw(env->body,row+6,(env->cols-17)/2,"U   U F     U   U");
  mvwprintw(env->body,row+7,(env->cols-17)/2,"U   U F     U   U");
  mvwprintw(env->body,row+8,(env->cols-17)/2," UUU  F      UUU");

  wattroff(env->body,A_BOLD);
  wattroff(env->body,A_BLINK);

  s=ufu_alloc_string(env,strlen(UFU_COPYRIGHT));
  strcpy(s,UFU_COPYRIGHT);
  ufu_lower(env,s);
  mvwprintw(env->body,row+10,(env->cols-strlen(s))/2,s);
  ufu_free_string(env,s);

  s=ufu_alloc_string(env,strlen(UFU_VERSION));
  strcpy(s,UFU_VERSION);
  ufu_lower(env,s);
  mvwprintw(env->body,row+12,(env->cols-strlen(s))/2,s);
  ufu_free_string(env,s);

  s=ufu_alloc_string(env,strlen(UFU_BUILDDATE));
  strcpy(s,UFU_BUILDDATE);
  ufu_lower(env,s);
  mvwprintw(env->body,row+14,(env->cols-strlen(s))/2,s);
  ufu_free_string(env,s);

}

void ufu_logo(struct s_env *env) {

  int row;

  strcpy(env->fun,"ufu_logo");
  ufu_wai(env);

  row=(env->rows-18)/2-1;
  if(row>8)  row=8;

  ufu_logo_name(env,row);

  mvwprintw(env->body,row+17,(env->cols-45)/2,"Remember, you can always use '%c' to get help!",UFU_KEY_QMARK);

  ufu_wrefresh(env->top);
  ufu_wrefresh(env->body);

  // Read the defined panels while the user is looking at the logo... :-)
  ufu_read_panels(env);

  sprintf(env->msg,"Press <y> if you want to look at the copyright notice: ");
  if(ufu_get_y(env)) {

    ufu_logo_name(env,row);

    mvwprintw(env->body,row+10,(env->cols-strlen(UFU_CR_LINE1))/2,"%s",UFU_CR_LINE1);
    mvwprintw(env->body,row+11,(env->cols-strlen(UFU_CR_LINE2))/2,"%s",UFU_CR_LINE2);
    mvwprintw(env->body,row+12,(env->cols-strlen(UFU_CR_LINE3))/2,"%s",UFU_CR_LINE3);

    mvwprintw(env->body,row+14,(env->cols-strlen(UFU_CR_LINE4))/2,"%s",UFU_CR_LINE4);
    mvwprintw(env->body,row+15,(env->cols-strlen(UFU_CR_LINE5))/2,"%s",UFU_CR_LINE5);
    mvwprintw(env->body,row+16,(env->cols-strlen(UFU_CR_LINE6))/2,"%s",UFU_CR_LINE6);

    ufu_wrefresh(env->top);
    ufu_wrefresh(env->body);

    ufu_any_key(env);

  }

}
