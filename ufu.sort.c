
#include "ufu.h"
#include "ufu.p"

void ufu_sort(struct s_env *env,int panel) {

  char sortc,sorto;
  struct s_entry *tos;

  strcpy(env->fun,"ufu_sort");
  ufu_wai(env);

  if(strlen(env->panel[panel]->dirname)>0) {

    sprintf(env->msg," Sorting panel #%d (%s,%c%c).",panel,env->panel[panel]->dirname,sortc,sorto);
    ufu_log(env);

    tos=env->panel[panel]->tos;

    switch(env->panel[panel]->sortc) {
      case UFU_SORT_FNAME:
        strcpy(env->msg,"Sort by filename");
        sortc='f';
        break;
      case UFU_SORT_SIZE:
        strcpy(env->msg,"Sort by size");
        sortc='s';
        break;
      case UFU_SORT_LA:
        strcpy(env->msg,"Sort by access time");
        sortc='a';
        break;
      case UFU_SORT_LM:
        strcpy(env->msg,"Sort by modification time");
        sortc='m';
        break;
      case UFU_SORT_LC:
        strcpy(env->msg,"Sort by status time");
        sortc='c';
        break;
    }

    strcat(env->msg," - ");

    switch(env->panel[panel]->sorto) {
      case UFU_SORT_ASC:
        strcat(env->msg,"Ascending");
        sorto='a';
        break;
      case UFU_SORT_DSC:
        strcat(env->msg,"Descending");
        sorto='d';
        break;
    }

    // ufu_listsort(env,panel);
    ufu_presort(env,panel);

    // if(env->dirfirst) {
    //   ufu_shuffle(env,panel);
    // }

    env->panel[panel]->cos=tos;
    env->panel[panel]->tos=tos;

    sprintf(env->msg," Finished sorting panel #%d.",panel);
    ufu_log(env);

  }

}

void ufu_presort(struct s_env *env,int panel) {

  int seqno,found;
  int seqno_f,seqno_l,seqno_df,seqno_dl,seqno_rf,seqno_rl;
  struct s_entry *e,*e1,*e2;

  strcpy(env->fun,"ufu_presort");
  ufu_wai(env);

  sprintf(env->msg," Start sorting of %s",env->panel[panel]->dirname);
  ufu_log(env);

  if(env->panel[panel]->first!=NULL) {

    // Move "." to first position.
    e=env->panel[panel]->first;
    while((e!=NULL)&&(strcmp(e->fname,".")!=0)) {
      e=e->next;
    }
    if(e!=NULL) {
      ufu_sort_swap(env,env->panel[panel]->first,e);
    }
    else {
      sprintf(env->msg,"No 'current directory' found?");
      ufu_log(env);
    }

    // Move ".." to second position.
    e=env->panel[panel]->first->next;
    while((e!=NULL)&&(strcmp(e->fname,"..")!=0)) {
      e=e->next;
    }
    if(e!=NULL) {
      ufu_sort_swap(env,env->panel[panel]->first->next,e);
    }
    else {
      sprintf(env->msg,"No 'upper directory' found?");
      ufu_log(env);
    }

    if(env->dirfirst) {

      // Renumber entries.
      e=env->panel[panel]->first;
      seqno=0;
      while(e!=NULL) {
        e->seqno=seqno++;
        e=e->next;
      }

      // Move the remaining directories to the top.
      // First check the existence of non-directories.
      found=FALSE;
      e=env->panel[panel]->first;
      while((e!=NULL)&&(!found)) {
	if(!(e->is_dir)) {
	  found=TRUE;
	}
        e=e->next;
      }
      if(found) {
        e1=env->panel[panel]->first->next->next;
        while(e1!=NULL) {
//sprintf(env->msg,"(e1) %s %d %d %s",e1->fperm,e1->is_dir,e1->seqno,e1->fname);
//ufu_log(env);
          if(e1->is_dir) {
            found=FALSE;
            e2=env->panel[panel]->first->next->next;
            while((e2!=NULL)&&(!found)) {
//sprintf(env->msg,"(e2) %s %d %d %s",e2->fperm,e2->is_dir,e2->seqno,e2->fname);
//ufu_log(env);
	      if(!(e2->is_dir)) {
	        found=TRUE;
                if((e1->seqno)>(e2->seqno)) {
                  ufu_sort_swap(env,e1,e2);
		}
	      }
              e2=e2->next;
            }
          }
          e1=e1->next;
        }

      }

      seqno_df=0;
      seqno_dl=0;
      seqno_rf=0;
      seqno_rl=0;
      // Renumber entries.
      e=env->panel[panel]->first;
      seqno=0;
      while(e!=NULL) {
        e->seqno=seqno++;
	if(e->is_dir) {
          seqno_dl=e->seqno;
	}
	else {
	  if(!(e->is_dir)) {
            seqno_rl=e->seqno;
	    if(seqno_rf==0) {
              seqno_rf=e->seqno;
	    }
	  }
	}
        e=e->next;
      }
      ufu_bubblesort(env,panel,seqno_df+2,seqno_dl,TRUE);
      ufu_bubblesort(env,panel,seqno_rf,seqno_rl,FALSE);
    }
    else {
      seqno_f=2;
      seqno_l=0;
      // Renumber entries.
      e=env->panel[panel]->first;
      seqno=0;
      while(e!=NULL) {
        e->seqno=seqno++;
        seqno_l=e->seqno;
        e=e->next;
      }
      ufu_bubblesort(env,panel,seqno_f,seqno_l,FALSE);
    }

sprintf(env->msg,"DF-DL-RF-RL: %d-%d-%d-%d",seqno_df,seqno_dl,seqno_rf,seqno_rl);
ufu_log(env);

    if(env->dirfirst) {
      ufu_bubblesort(env,panel,seqno_df+2,seqno_dl,TRUE);
      ufu_bubblesort(env,panel,seqno_rf,seqno_rl,FALSE);
    }
    else {
      ufu_bubblesort(env,panel,seqno_rf,seqno_rl,FALSE);
    }

    // Renumber entries.
    sprintf(env->msg,"  Start renumbering after sort.");
    ufu_log(env);
    e=env->panel[panel]->first;
    seqno=0;
    while(e!=NULL) {
      e->seqno=seqno++;
      e=e->next;
    }
    sprintf(env->msg,"  Finished renumbering after sort.");
    ufu_log(env);
  }

  sprintf(env->msg," Finished sorting of %s",env->panel[panel]->dirname);
  ufu_log(env);

}

void ufu_bubblesort(struct s_env *env,int panel,int seqno_f,int seqno_l,int is_dir) {

  int done;
  struct s_entry *e,*e1,*e2,*e_f,*e_l;

  strcpy(env->fun,"ufu_bubblesort");
  ufu_wai(env);

sprintf(env->msg,"(bs) F %d, L %d",seqno_f,seqno_l);
ufu_log(env);

  if(seqno_f<seqno_l) {

    e_f=NULL;
    e_l=NULL;

    done=FALSE;
    e=env->panel[panel]->first;
    while((e!=NULL)&&(!done)) {
      if(e->seqno==seqno_f) {
        e_f=e;
      }
      if(e->seqno==seqno_l) {
        e_l=e;
      }
      done=((e_f!=NULL)&&(e_l!=NULL));
      e=e->next;
    }

    e1=e_f;
    while((e1!=NULL)&&(e1->seqno<=seqno_l)) {

      e2=e_f;
      while((e2!=NULL)&&(e2->seqno<e1->seqno)) {

        if(ufu_bubblesort_entries(env,panel,e1,e2)) {
          ufu_sort_swap(env,e1,e2);
	}

        e2=e2->next;
      }

      e1=e1->next;
    }


  }

}

int ufu_bubblesort_entries(struct s_env *env,int panel,struct s_entry *x,struct s_entry *y) {

  int swap;
  char *sx,*sy;

  swap=FALSE;

  switch(env->panel[panel]->sortc) {

    case UFU_SORT_FNAME:
      sx=ufu_alloc_string(env,strlen(x->fname)+2);
      strcpy(sx,x->fname);
      ufu_lower(env,sx);
      sy=ufu_alloc_string(env,strlen(y->fname)+2);
      strcpy(sy,y->fname);
      ufu_lower(env,sy);
      switch(env->panel[panel]->sorto) {
        case UFU_SORT_ASC:
          if(strcmp(sx,sy)<0)
            swap=TRUE;
          break;
        case UFU_SORT_DSC:
          if(strcmp(sx,sy)>0)
            swap=TRUE;
          break;
      }
      ufu_free_string(env,sx);
      ufu_free_string(env,sy);
      break;

    case UFU_SORT_SIZE:
      switch(env->panel[panel]->sorto) {
        case UFU_SORT_ASC:
          if((x->size)<(y->size))
            swap=TRUE;
          break;
        case UFU_SORT_DSC:
          if((x->size)>(y->size))
            swap=TRUE;
          break;
      }
      break;

    case UFU_SORT_LA:
      switch(env->panel[panel]->sorto) {
        case UFU_SORT_ASC:
          if(x->fatime<y->fatime)
            swap=TRUE;
          break;
        case UFU_SORT_DSC:
          if(x->fatime>y->fatime)
            swap=TRUE;
          break;
      }
      break;

    case UFU_SORT_LM:
      switch(env->panel[panel]->sorto) {
        case UFU_SORT_ASC:
          if(x->fmtime<y->fmtime)
            swap=TRUE;
          break;
        case UFU_SORT_DSC:
          if(x->fmtime>y->fmtime)
            swap=TRUE;
          break;
      }
      break;

    case UFU_SORT_LC:
      switch(env->panel[panel]->sorto) {
        case UFU_SORT_ASC:
          if(x->fctime<y->fctime)
            swap=TRUE;
          break;
        case UFU_SORT_DSC:
          if(x->fctime>y->fctime)
            swap=TRUE;
          break;
      }
      break;

  }

  return(swap);

}

void ufu_sort_swap(struct s_env *env,struct s_entry *x,struct s_entry *y) {

  struct s_entry *t;

  strcpy(env->fun,"ufu_sort_swap");
  ufu_wai(env);

  if(env->debug) {
    sprintf(env->msg,"Swapping entry %s (%d) with entry %s (%d)",x->fname,x->seqno,y->fname,y->seqno);
    ufu_log(env);
  }

  t=ufu_alloc_entry(env);

  ufu_free_string(env,t->fname);
  ufu_free_string(env,t->fperm);
  ufu_free_string(env,t->lname);
  ufu_free_string(env,t->lperm);

  ufu_free_string(env,t->fusrname);
  ufu_free_string(env,t->fgrpname);
  ufu_free_string(env,t->lusrname);
  ufu_free_string(env,t->lgrpname);

  //t->seqno=x->seqno;
  t->fstat=x->fstat;
  t->lstat=x->lstat;
  t->fname=x->fname;
  t->fperm=x->fperm;
  t->lname=x->lname;
  t->lperm=x->lperm;
  t->fusrname=x->fusrname;
  t->fgrpname=x->fgrpname;
  t->lusrname=x->lusrname;
  t->lgrpname=x->lgrpname;
  t->uid=x->uid;
  t->gid=x->gid;
  t->fatime=x->fatime;
  t->fmtime=x->fmtime;
  t->fctime=x->fctime;
  t->latime=x->latime;
  t->lmtime=x->lmtime;
  t->lctime=x->lctime;
  t->u_read=x->u_read;
  t->u_write=x->u_write;
  t->u_exec=x->u_exec;
  t->u_stick=x->u_stick;
  t->g_read=x->g_read;
  t->g_write=x->g_write;
  t->g_exec=x->g_exec;
  t->g_stick=x->g_stick;
  t->o_read=x->o_read;
  t->o_write=x->o_write;
  t->o_exec=x->o_exec;
  t->o_stick=x->o_stick;
  t->size=x->size;
  t->is_reg=x->is_reg;
  t->is_dir=x->is_dir;
  t->is_lnk=x->is_lnk;
  t->is_chr=x->is_chr;
  t->is_blk=x->is_blk;
  t->is_fifo=x->is_fifo;
  t->is_sock=x->is_sock;
  t->blocks=x->blocks;
  t->fmode=x->fmode;
  t->lmode=x->lmode;
  t->fino=x->fino;
  t->lino=x->lino;
  t->marked=x->marked;

  //x->seqno=y->seqno;
  x->fstat=y->fstat;
  x->lstat=y->lstat;
  x->fname=y->fname;
  x->fperm=y->fperm;
  x->lname=y->lname;
  x->lperm=y->lperm;
  x->fusrname=y->fusrname;
  x->fgrpname=y->fgrpname;
  x->lusrname=y->lusrname;
  x->lgrpname=y->lgrpname;
  x->uid=y->uid;
  x->gid=y->gid;
  x->fatime=y->fatime;
  x->fmtime=y->fmtime;
  x->fctime=y->fctime;
  x->latime=y->latime;
  x->lmtime=y->lmtime;
  x->lctime=y->lctime;
  x->u_read=y->u_read;
  x->u_write=y->u_write;
  x->u_exec=y->u_exec;
  x->u_stick=y->u_stick;
  x->g_read=y->g_read;
  x->g_write=y->g_write;
  x->g_exec=y->g_exec;
  x->g_stick=y->g_stick;
  x->o_read=y->o_read;
  x->o_write=y->o_write;
  x->o_exec=y->o_exec;
  x->o_stick=y->o_stick;
  x->size=y->size;
  x->is_reg=y->is_reg;
  x->is_dir=y->is_dir;
  x->is_lnk=y->is_lnk;
  x->is_chr=y->is_chr;
  x->is_blk=y->is_blk;
  x->is_fifo=y->is_fifo;
  x->is_sock=y->is_sock;
  x->blocks=y->blocks;
  x->fmode=y->fmode;
  x->lmode=y->lmode;
  x->fino=y->fino;
  x->lino=y->lino;
  x->marked=y->marked;

  //y->seqno=t->seqno;
  y->fstat=t->fstat;
  y->lstat=t->lstat;
  y->fname=t->fname;
  y->fperm=t->fperm;
  y->lname=t->lname;
  y->lperm=t->lperm;
  y->fusrname=t->fusrname;
  y->fgrpname=t->fgrpname;
  y->lusrname=t->lusrname;
  y->lgrpname=t->lgrpname;
  y->uid=t->uid;
  y->gid=t->gid;
  y->fatime=t->fatime;
  y->fmtime=t->fmtime;
  y->fctime=t->fctime;
  y->latime=t->latime;
  y->lmtime=t->lmtime;
  y->lctime=t->lctime;
  y->u_read=t->u_read;
  y->u_write=t->u_write;
  y->u_exec=t->u_exec;
  y->u_stick=t->u_stick;
  y->g_read=t->g_read;
  y->g_write=t->g_write;
  y->g_exec=t->g_exec;
  y->g_stick=t->g_stick;
  y->o_read=t->o_read;
  y->o_write=t->o_write;
  y->o_exec=t->o_exec;
  y->o_stick=t->o_stick;
  y->size=t->size;
  y->is_reg=t->is_reg;
  y->is_dir=t->is_dir;
  y->is_lnk=t->is_lnk;
  y->is_chr=t->is_chr;
  y->is_blk=t->is_blk;
  y->is_fifo=t->is_fifo;
  y->is_sock=t->is_sock;
  y->blocks=t->blocks;
  y->fino=t->fino;
  y->lino=t->lino;
  y->fmode=t->fmode;
  y->lmode=t->lmode;
  y->marked=t->marked;

  t->fstat=NULL;
  t->lstat=NULL;
  t->next=NULL;
  t->prev=NULL;

  free(t);
  // ufu_free_entry(env,t);

}
