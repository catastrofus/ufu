
#include "ufu.h"
#include "ufu.p"

void ufu_add_hist(struct s_env *env,char what,char *dname,int seqno) {

  int found,count;
  struct s_hist *h,*h1;

  strcpy(env->fun,"ufu_add_hist");
  ufu_wai(env);

  count=0;

  switch(what) {
    case UFU_HIST_ENTRY :

      found=FALSE;
      h=env->hfirst;

      while((h!=NULL)&&(!found)) {
        count++;
        found=(strcmp(h->dname,dname)==0);
        if(found) {
          sprintf(env->msg,"Updated ENTRY (%d/%d) seqno (%d) to %d for directory \"%s\".",count,env->history_max,h->seqno,seqno,h->dname);
          ufu_log(env);
          h->seqno=seqno;
        }
        else {
          h=h->next;
        }
      }

      if(!found) {

        sprintf(env->msg,"Adding ENTRY (%d/%d) seqno %d for directory \"%s\".",count+1,env->history_max,seqno,dname);
        ufu_log(env);

        h=ufu_alloc_hist(env);

        h->what=what;
        strcpy(h->dname,dname);
        h->seqno=seqno;

        if(env->hfirst!=NULL) {

          h1=env->hfirst;
          while(h1->next!=NULL) {
            h1=h1->next;
          }
          h1->next=h;
          h->prev=h1;

        }
        else {

          env->hfirst=h;

        }

      }

      break;

    case UFU_HIST_PANEL:

      env->hist_panel=seqno;

      sprintf(env->msg,"Adding PANEL seqno %d to history.",env->hist_panel);
      ufu_log(env);

      break;

    case UFU_HIST_U:

      env->hist_u=seqno;

      sprintf(env->msg,"Adding UID seqno %d to history.",env->hist_u);
      ufu_log(env);

      break;

    case UFU_HIST_G:

      env->hist_g=seqno;

      sprintf(env->msg,"Adding GID seqno %d to history.",env->hist_g);
      ufu_log(env);

      break;

    case UFU_HIST_MARK:

      env->hist_mark=seqno;

      sprintf(env->msg,"Adding MARK seqno %d to history.",env->hist_mark);
      ufu_log(env);

      break;

    case UFU_HIST_MARK_ACTION:

      env->hist_mark_action=seqno;

      sprintf(env->msg,"Adding MARK_ACTION seqno %d to history.",env->hist_mark_action);
      ufu_log(env);

      break;

    case UFU_HIST_UCMD:

      env->hist_ucmd=seqno;

      sprintf(env->msg,"Adding UCMD seqno %d to history.",env->hist_ucmd);
      ufu_log(env);

      break;

    case UFU_HIST_XINFO:

      env->hist_xinfo=seqno;

      sprintf(env->msg,"Adding XINFO seqno %d to history.",env->hist_xinfo);
      ufu_log(env);

      break;

    case UFU_HIST_REMOTE:

      env->hist_remote=seqno;

      sprintf(env->msg,"Adding REMOTE seqno %d to history.",env->hist_remote);
      ufu_log(env);

      break;

    case UFU_HIST_SETUP:

      env->hist_setup=seqno;

      sprintf(env->msg,"Adding SETUP seqno %d to history.",env->hist_setup);
      ufu_log(env);

      break;

    case UFU_HIST_TC:

      env->hist_tc=seqno;

      sprintf(env->msg,"Adding TRASH seqno %d to history.",env->hist_tc);
      ufu_log(env);

      break;

  }

}

int ufu_find_hist(struct s_env *env,char what,char *dname) {

  int found,seqno;
  struct s_hist *h;

  strcpy(env->fun,"ufu_find_hist");
  ufu_wai(env);

  switch(what) {
    case UFU_HIST_ENTRY :

      found=FALSE;
      seqno=-1;
      h=env->hfirst;

      while((h!=NULL)&&(!found)) {
        found=(strcmp(h->dname,dname)==0);
        if(found) {
          seqno=h->seqno;
          sprintf(env->msg,"Found ENTRY seqno %d for directory \"%s\".",h->seqno,h->dname);
          ufu_log(env);
        }
        h=h->next;
      }

      break;

    case UFU_HIST_PANEL:

      seqno=env->hist_panel;

      sprintf(env->msg,"Found PANEL seqno %d.",seqno);
      ufu_log(env);

      break;

    case UFU_HIST_MARK:

      seqno=env->hist_mark;

      sprintf(env->msg,"Found MARK seqno %d.",seqno);
      ufu_log(env);

      break;

    case UFU_HIST_MARK_ACTION:

      seqno=env->hist_mark_action;

      sprintf(env->msg,"Found MARK_ACTION seqno %d.",seqno);
      ufu_log(env);

      break;

    case UFU_HIST_UCMD:

      seqno=env->hist_ucmd;

      sprintf(env->msg,"Found UCMD seqno %d.",seqno);
      ufu_log(env);

      break;

    case UFU_HIST_XINFO:

      seqno=env->hist_xinfo;

      sprintf(env->msg,"Found XINFO seqno %d.",seqno);
      ufu_log(env);

      break;

    case UFU_HIST_REMOTE:

      seqno=env->hist_remote;

      sprintf(env->msg,"Found REMOTE seqno %d.",seqno);
      ufu_log(env);

      break;

    case UFU_HIST_TC:

      seqno=env->hist_tc;

      sprintf(env->msg,"Found TRASH seqno %d.",seqno);
      ufu_log(env);

      break;

    case UFU_HIST_SETUP:

      seqno=env->hist_setup;

      sprintf(env->msg,"Found SETUP seqno %d.",seqno);
      ufu_log(env);

      break;

  }

  return(seqno);

}

void ufu_clean_hist(struct s_env *env) {

  int i,n_entry,n_panel,total,count,sum;
  struct s_hist *h,*h1,*h2;

  strcpy(env->fun,"ufu_clean_hist");
  ufu_wai(env);

  n_entry=0;
  n_panel=0;

  h=env->hfirst;

  while(h!=NULL) {

    switch(h->what) {
      case UFU_HIST_ENTRY:
        n_entry++;
        break;
      case UFU_HIST_PANEL:
        n_panel++;
        break;
    }

    h=h->next;

  }

  total=n_entry+n_panel;
  sum=total-(env->history_min);

  sprintf(env->msg,"Retained history: %d/%d (%d).",env->history_min,env->history_max,total);
  ufu_log(env);

  if(total>env->history_max) {

    sprintf(env->msg,"Clean up history: E=%d, P=%d, Total=%d.",n_entry,n_panel,total);
    ufu_log(env);

    sprintf(env->msg,"Total (%d) > History (%d/%d).",total,env->history_min,env->history_max);
    ufu_log(env);

    if(sum>1)
      sprintf(env->msg,"Going to clean %d entries.",sum);
    else
      sprintf(env->msg,"Going to clean 1 entry.");
    ufu_log(env);

    i=total;
    count=0;

    while(total>env->history_min) {
      h1=env->hfirst;
      h2=env->hfirst->next;
      env->hfirst=h2;
      ufu_free_hist(env,h1);
      total--;
      count++;
    }

    if(count>1)
      sprintf(env->msg,"Cleaned %d entries.",count);
    else
      sprintf(env->msg,"Cleaned 1 entry.");
    ufu_log(env);

    sprintf(env->msg,"List of retained history:");
    ufu_log(env);

    i=0;
    h=env->hfirst;
    while(h!=NULL) {
      sprintf(env->msg,"%4d [%c] %s -> %d.",i++,h->what,h->dname,h->seqno);
      ufu_log(env);
      h=h->next;
    }

    sprintf(env->msg,"End list.");
    ufu_log(env);

  }

}

