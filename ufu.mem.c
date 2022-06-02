
#include "ufu.h"
#include "ufu.p"

void ufu_mod_lmem(struct s_env *env,long int memval) {

  env->lmem+=memval;

}

struct s_env *ufu_alloc_env(int argc,char **argv) {

  int i,p,debug,winsize;
  char *spid,*s;

  struct s_env *env;
  struct utsname u;

  debug=ufu_get_arg(argc,argv);

  if((env=(struct s_env *)malloc(sizeof(struct s_env)))==NULL) {

    exit(9);

  }

  env->lmem=(long) 0;
  ufu_mod_lmem(env,sizeof(struct s_env));

  env->msgglobal=ufu_alloc_string(env,UFU_LEN_NAME);
  env->cfgglobal=ufu_alloc_string(env,UFU_LEN_NAME);
  env->cfglocal=ufu_alloc_string(env,UFU_LEN_NAME);
  env->logname=ufu_alloc_string(env,UFU_LEN_NAME);
  env->msg=ufu_alloc_string(env,UFU_LEN_MSG);
  env->fun=ufu_alloc_string(env,UFU_LEN_FUN);
  env->gname=ufu_alloc_string(env,UFU_LEN_GROUP);
  env->editor=ufu_alloc_string(env,UFU_LEN_NAME);
  env->viewer=ufu_alloc_string(env,UFU_LEN_NAME);

  env->nodename=ufu_alloc_string(env,UFU_LEN_UTS);
  env->sysname=ufu_alloc_string(env,UFU_LEN_UTS);
  env->release=ufu_alloc_string(env,UFU_LEN_UTS);

  env->master=ufu_alloc_string(env,UFU_LEN_MASTER);

  env->tcdir=ufu_alloc_string(env,UFU_LEN_DNAME);
  env->tcfile=ufu_alloc_string(env,UFU_LEN_NAME);

  env->sh_hfile=ufu_alloc_string(env,UFU_LEN_NAME);

  env->cpanel=UFU_MIN_PANEL;
  env->mpanel=UFU_MAX_PANEL;

  env->cols=0;
  env->rows=0;

  env->cfg_cols=0;
  env->cfg_rows=0;

  env->debug=debug;

  uname(&u);
  strcpy(env->nodename,u.nodename);
  strcpy(env->sysname,u.sysname);
  strcpy(env->release,u.release);

  env->pid=getpid();
  env->ppid=getppid();

  strcpy(env->editor,"\0");
  strcpy(env->viewer,"\0");

  strcpy(env->sh_hfile,"\0");

  env->homedir=getenv("HOME");
  env->workdir=getenv("PWD");
  env->path=getenv("PATH");

  env->uname=getenv("LOGNAME");

  // Get windowsize from environment.
  env->rows=ufu_get_rows(env);
  env->cols=ufu_get_cols(env);

  winsize=TRUE;
  if(env->rows<UFU_MIN_ROWS) {
    printf("Number of rows (%d) less than %d!\n",env->rows,UFU_MIN_ROWS);
    winsize=FALSE;
  }
  if(env->cols<UFU_MIN_COLS) {
    printf("Number of columns (%d) less than %d!\n",env->cols,UFU_MIN_COLS);
    winsize=FALSE;
  }
  if(!winsize) {
    printf("Window size too small!\n");
    exit(1);
  }

  env->confirmquit=TRUE;
  env->confirmcopy=TRUE;
  env->confirmmove=TRUE;
  env->confirmremove=TRUE;
  env->confirmaction=TRUE;
  env->savemarked=FALSE;
  env->ignoreerror=FALSE;
  env->incsubdir=FALSE;
  env->dirfirst=TRUE;
  env->removelog=TRUE;
  env->writeconfig=TRUE;
  env->removelog=TRUE;

  env->rb=FALSE;
  env->rb_size=64;

  // History.
  env->history_min=99;
  env->history_max=99;

  // Limit memory usage.
  env->max_to_mark=UFU_MAX_MARK;

  env->hist_panel=0;
  env->hist_mark=0;
  env->hist_mark_action=0;
  env->hist_ucmd=0;
  env->hist_xinfo=0;
  env->hist_remote=0;
  env->hist_tc=0;
  env->hist_u=0;
  env->hist_g=0;
  env->hist_setup=0;

  // Timein & Timeout :-)
  env->timeout=100;
  env->timeout_retry=4;
  env->timein=100;
  env->timein_retry=4;

  // Concat global msgfile.
  strcpy(env->msgglobal,UFU_GLOBAL_MSG);

  // Concat global configname.
  strcpy(env->cfgglobal,UFU_GLOBAL_CONFIG);

  // Concat homedir & local configname.
  ufu_concat_dir(env,env->cfglocal,env->homedir,UFU_CFGDIR);
  ufu_concat_dir(env,env->cfglocal,env->cfglocal,UFU_LOCAL_CONFIG);

  s=ufu_alloc_string(env,UFU_LEN_NAME);

  ufu_concat_dir(env,s,env->homedir,UFU_CFGDIR);
  mkdir(s,0700);

  ufu_concat_dir(env,s,env->homedir,UFU_CFGDIR);
  ufu_concat_dir(env,s,s,UFU_LOGDIR);
  mkdir(s,0700);

  ufu_concat_dir(env,s,env->homedir,UFU_CFGDIR);
  ufu_concat_dir(env,s,s,UFU_TCDIR);
  mkdir(s,0700);

  ufu_free_string(env,s);

  // Concat homedir & local configname.
  // ufu_concat_dir(env,env->cfglocal,env->homedir,UFU_LOCAL_CONFIG);

  // Concat homedir & logfilename.
  ufu_concat_dir(env,env->logname,env->homedir,UFU_CFGDIR);
  ufu_concat_dir(env,env->logname,env->logname,UFU_LOGDIR);

  // Concat homedir & <tcdir|tcfile>.
  ufu_concat_dir(env,env->tcdir,env->homedir,UFU_CFGDIR);
  ufu_concat_dir(env,env->tcdir,env->tcdir,UFU_TCDIR);
  ufu_concat_dir(env,env->tcfile,env->tcdir,UFU_TCFILE);

  strcat(env->logname,"/");

  spid=ufu_alloc_string(env,16);
  sprintf(spid,"%05d",env->pid);

  strcat(env->logname,spid);
  strcat(env->logname,".");
  strcat(env->logname,UFU_LOGFILE);

  ufu_free_string(env,spid);

  // Pointer for logfile.
  env->lognamefp=NULL;

  ufu_open_log(env);

  sprintf(env->msg,"UFU started as executable '%s'.",argv[0]);
  ufu_log(env);

  if(env->debug) {
    sprintf(env->msg,"DEBUG turned on due to the commandline argument '-d'.");
    ufu_log(env);
  }

  strcpy(env->fun,"ufu_alloc_env");
  ufu_wai(env);

  sprintf(env->msg,"Reading HOMEDIR (%s) from environment.",env->homedir);
  ufu_log(env);

  sprintf(env->msg,"Reading WORKDIR (%s) from environment.",env->workdir);
  ufu_log(env);

  sprintf(env->msg,"Reading LINES (%d) from environment.",env->rows);
  ufu_log(env);
  sprintf(env->msg,"Reading COLUMNS (%d) from environment.",env->cols);
  ufu_log(env);

  // Read user & group file.
  env->ufirst=NULL;
  env->gfirst=NULL;

  env->mfirst=NULL;
  env->mlast=NULL;

  env->ucfirst=NULL;
  env->uclast=NULL;

  env->rhfirst=NULL;
  env->rhlast=NULL;

  env->sfirst=NULL;

  env->tcfirst=NULL;

  env->tfirst=NULL;

  ufu_read_users(env);
  ufu_read_groups(env);

  // Find uid, gid & groupname for user.
  sprintf(env->msg,"Finding uid/gid for user \"%s\".",env->uname);
  ufu_log(env);
  env->uid=ufu_search_uid(env,env->uname);
  env->gid=ufu_search_user_gid(env,env->uid);
  strcpy(env->gname,ufu_search_group(env,env->gid));
  sprintf(env->msg,"User \"%s\": uid %d, gid %d (%s).",env->uname,env->uid,env->gid,ufu_search_group(env,env->gid));
  ufu_log(env);

  for(i=UFU_MIN_PANEL;i<=UFU_MAX_PANEL;i++) {
    env->panel[i]=ufu_alloc_panel(env,i);
  }

  // Read global/local configfile.
  p=ufu_read_config(env,UFU_MIN_PANEL,env->cfgglobal,argc,argv);
  ufu_read_config(env,p,env->cfglocal,0,NULL);

  ufu_findext(env,"vi",env->editor);
  ufu_findext(env,"view",env->viewer);

  // Windowsize from environment or from config?
  if((env->rows>0)&&(env->cols>0)) {
    sprintf(env->msg,"Using ROWS/COLUMNS from environment.");
    ufu_log(env);
  }
  else {
    if((env->cfg_cols>0)&&(env->cfg_rows>0)) {
      sprintf(env->msg,"Using ROWS/COLUMNS from config.");
      ufu_log(env);
      env->cols=env->cfg_cols;
      env->rows=env->cfg_rows;
    }
  }

  // Trash.
  ufu_read_tc(env);

  // Check for minimum values for windowsize.
  if(env->cols<80)  env->cols=80;
  if(env->rows<24)  env->rows=24;

  strcpy(env->master,env->uname);
  strcat(env->master," from ");
  strcat(env->master,env->nodename);

  env->hfirst=NULL;

  env->key_help=0;
  env->key_info=0;
  env->key_up=0;
  env->key_down=0;
  env->key_scroll_up=0;
  env->key_scroll_down=0;
  env->key_left=0;
  env->key_right=0;
  env->key_next_page=0;
  env->key_prev_page=0;
  env->key_panel=0;
  env->key_next_panel=0;
  env->key_prev_panel=0;
  env->key_setting=0;
  env->key_dir_up=0;
  env->key_dir_root=0;
  env->key_dir_home=0;
  env->key_dir_prev=0;
  env->key_dir_work=0;
  env->key_first=0;
  env->key_last=0;
  env->key_select=0;
  env->key_edit=0;
  env->key_view=0;
  env->key_expunge=0;
  env->key_show_mark=0;
  env->key_show_mark_action=0;
  env->key_add_mark=0;
  env->key_rem_mark=0;
  env->key_add_panel=0;
  env->key_rem_panel=0;
  env->key_add_ucmd=0;
  env->key_rem_ucmd=0;
  env->key_header=0;
  env->key_sort_col=0;
  env->key_sort_order=0;
  env->key_go=0;
  env->key_tc=0;
  env->key_u=0;
  env->key_g=0;
  env->key_search=0;
  env->key_config_1=0;
  env->key_config_2=0;
  env->key_config_3=0;
  env->key_config_4=0;
  env->key_config_5=0;
  env->key_config_6=0;
  env->key_config_7=0;
  env->key_ucmd_0=0;
  env->key_ucmd_1=0;
  env->key_ucmd_2=0;
  env->key_ucmd_3=0;
  env->key_ucmd_4=0;
  env->key_ucmd_5=0;
  env->key_ucmd_6=0;
  env->key_ucmd_7=0;
  env->key_ucmd_8=0;
  env->key_ucmd_9=0;

  return(env);

}

struct s_panel *ufu_alloc_panel(struct s_env *env,int panel) {

  struct s_panel *p;

  strcpy(env->fun,"ufu_alloc_panel");
  ufu_wai(env);

  if((p=(struct s_panel *)malloc(sizeof(struct s_panel)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_panel'!");
    ufu_log(env);
    sprintf(env->msg,"Needed: %ld bytes.",sizeof(struct s_panel));
    ufu_log(env);

    exit(1);

  }

  if(env->debug) {
    sprintf(env->msg,"Allocating memory for panel #%d.",panel);
    ufu_log(env);
  }

  p->dirname=ufu_alloc_string(env,UFU_LEN_NAME);
  p->dirnameprev=ufu_alloc_string(env,UFU_LEN_NAME);

  p->nodename=ufu_alloc_string(env,UFU_LEN_UTS);
  strcpy(p->nodename,env->nodename);

  p->header=UFU_HDR_STD;

  p->local=TRUE;
  p->remote=FALSE;

  p->ndirs=0;
  p->nfiles=0;
  p->nlinks=0;
  p->ntotal=0;

  p->size=(long) 0;

  p->sortc=UFU_SORT_FNAME;
  p->sorto=UFU_SORT_ASC;

  // Entry used for 'statting' this panel.
  p->info=ufu_alloc_entry(env);
  // Top of screen entry.
  p->tos=NULL;
  // Current on screen entry.
  p->cos=NULL;
  // First entry on list.
  p->first=NULL;
  // Last entry on list.
  p->last=NULL;

  ufu_mod_lmem(env,sizeof(struct s_panel));

  return(p);

}

struct s_entry *ufu_alloc_entry(struct s_env *env) {

  struct s_entry *e;

  strcpy(env->fun,"ufu_alloc_entry");
  ufu_wai(env);

  if((e=(struct s_entry *)malloc(sizeof(struct s_entry)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_entry'!");
    ufu_log(env);
    sprintf(env->msg,"Needed: %ld bytes.",sizeof(struct s_entry));
    ufu_log(env);

    exit(1);

  }

  e->seqno=0;

  e->fstat=NULL;
  e->lstat=NULL;

  e->size=(long)0;

  e->fname=ufu_alloc_string(env,UFU_LEN_NAME);
  e->fperm=ufu_alloc_string(env,UFU_LEN_PERM);
  e->lname=ufu_alloc_string(env,UFU_LEN_NAME);
  e->lperm=ufu_alloc_string(env,UFU_LEN_PERM);

  e->fusrname=ufu_alloc_string(env,UFU_LEN_UNAME);
  e->fgrpname=ufu_alloc_string(env,UFU_LEN_GROUP);
  e->lusrname=ufu_alloc_string(env,UFU_LEN_UNAME);
  e->lgrpname=ufu_alloc_string(env,UFU_LEN_GROUP);

  e->is_dir=FALSE;
  e->is_lnk=FALSE;

  e->marked=FALSE;

  e->status_action=UFU_MARK_ACTION_NONE;

  ufu_mod_lmem(env,sizeof(struct s_entry));

  return(e);

}

struct s_split *ufu_alloc_split(struct s_env *env,int len) {

  struct s_split *s;

  strcpy(env->fun,"ufu_alloc_split");
  ufu_wai(env);

  if((s=(struct s_split *)malloc(sizeof(struct s_split)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_split'!");
    ufu_log(env);
    sprintf(env->msg,"Needed: %ld bytes.",sizeof(struct s_split));
    ufu_log(env);

    exit(1);

  }

  s->name=ufu_alloc_string(env,len);
  s->seqno=0;

  s->prev=NULL;

  ufu_mod_lmem(env,sizeof(struct s_split));

  return(s);

}

struct s_usr *ufu_alloc_usr(struct s_env *env) {

  struct s_usr *u;

  strcpy(env->fun,"ufu_alloc_usr");
  ufu_wai(env);

  if((u=(struct s_usr *)malloc(sizeof(struct s_usr)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_usr'!");
    ufu_log(env);
    sprintf(env->msg,"Needed: %ld bytes.",sizeof(struct s_usr));
    ufu_log(env);

    exit(1);

  }

  u->uid=-1;
  u->gid=-1;
  u->uname=ufu_alloc_string(env,UFU_LEN_UNAME);
  u->pword=ufu_alloc_string(env,UFU_LEN_PWORD);
  u->dir=ufu_alloc_string(env,UFU_LEN_UDIR);
  u->sh=ufu_alloc_string(env,UFU_LEN_USH);

  ufu_mod_lmem(env,sizeof(struct s_usr));

  return(u);

}

struct s_grp *ufu_alloc_grp(struct s_env *env) {

  struct s_grp *g;

  strcpy(env->fun,"ufu_alloc_grp");
  ufu_wai(env);

  if((g=(struct s_grp *)malloc(sizeof(struct s_grp)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_grp'!");
    ufu_log(env);
    sprintf(env->msg,"Needed: %ld bytes.",sizeof(struct s_grp));
    ufu_log(env);

    exit(1);

  }

  g->gid=-1;
  g->gname=ufu_alloc_string(env,UFU_LEN_GROUP);
  g->fgrpusr=NULL;

  ufu_mod_lmem(env,sizeof(struct s_grp));

  return(g);

}

struct s_grpusr *ufu_alloc_grpusr(struct s_env *env) {

  struct s_grpusr *gu;

  strcpy(env->fun,"ufu_alloc_grpusr");
  ufu_wai(env);

  if((gu=(struct s_grpusr *)malloc(sizeof(struct s_grpusr)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_grpusr'!");
    ufu_log(env);
    sprintf(env->msg,"Needed: %ld bytes.",sizeof(struct s_grpusr));
    ufu_log(env);

    exit(1);

  }

  gu->uname=ufu_alloc_string(env,UFU_LEN_UNAME);

  ufu_mod_lmem(env,sizeof(struct s_grpusr));

  return(gu);

}

struct s_msg *ufu_alloc_msg(struct s_env *env) {

  struct s_msg *m;

  strcpy(env->fun,"ufu_alloc_msg");
  ufu_wai(env);

  if((m=(struct s_msg *)malloc(sizeof(struct s_msg)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_msg'!");
    ufu_log(env);
    sprintf(env->msg,"Needed: %ld bytes.",sizeof(struct s_msg));
    ufu_log(env);

    exit(1);

  }

  m->seqno=0;
  m->id=ufu_alloc_string(env,UFU_LEN_MSGID);
  m->msg=ufu_alloc_string(env,UFU_LEN_MSG);

  ufu_mod_lmem(env,sizeof(struct s_msg));

  return(m);

}

struct s_log *ufu_alloc_log(struct s_env *env,int len) {

  struct s_log *t;

  if((t=(struct s_log *)malloc(sizeof(struct s_log)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_log'!");
    ufu_log(env);
    sprintf(env->msg,"Needed: %ld bytes.",sizeof(struct s_log));
    ufu_log(env);

    exit(1);

  }

  t->ts=ufu_alloc_string(env,UFU_LEN_TS+4);
  t->text=ufu_alloc_string(env,len);
  t->next=NULL;
  t->prev=NULL;

  ufu_mod_lmem(env,sizeof(struct s_log));

  return(t);

}

struct s_txt *ufu_alloc_txt(struct s_env *env) {

  struct s_txt *t;

  strcpy(env->fun,"ufu_alloc_txt");
  ufu_wai(env);

  if((t=(struct s_txt *)malloc(sizeof(struct s_txt)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_txt'!");
    ufu_log(env);
    sprintf(env->msg,"Needed: %ld bytes.",sizeof(struct s_txt));
    ufu_log(env);

    exit(1);

  }

  t->line=ufu_alloc_string(env,UFU_MAX_TEXT);
  t->next=NULL;
  t->prev=NULL;

  ufu_mod_lmem(env,sizeof(struct s_txt));

  return(t);

}

struct dirent *ufu_alloc_dirent(struct s_env *env) {

  struct dirent *d;

  strcpy(env->fun,"ufu_alloc_dirent");
  ufu_wai(env);

  if((d=(struct dirent *)malloc(sizeof(struct dirent)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 'dirent'!");
    ufu_log(env);
    sprintf(env->msg,"Needed: %ld bytes.",sizeof(struct dirent));
    ufu_log(env);

    exit(1);

  }

  ufu_mod_lmem(env,sizeof(struct dirent));

  return(d);

}

struct stat *ufu_alloc_stat(struct s_env *env) {

  struct stat *st;

  strcpy(env->fun,"ufu_alloc_stat");
  ufu_wai(env);

  if((st=(struct stat *)malloc(sizeof(struct stat)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 'stat'!");
    ufu_log(env);
    sprintf(env->msg,"Needed: %ld bytes.",sizeof(struct stat));
    ufu_log(env);

    exit(1);

  }

  ufu_mod_lmem(env,sizeof(struct stat));

  return(st);

}

struct s_mark *ufu_alloc_mark(struct s_env *env) {

  struct s_mark *m;

  strcpy(env->fun,"ufu_alloc_mark");
  ufu_wai(env);

  if((m=(struct s_mark *)malloc(sizeof(struct s_mark)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_mark'!");
    ufu_log(env);
    sprintf(env->msg,"Needed: %ld bytes.",sizeof(struct s_mark));
    ufu_log(env);

    exit(1);

  }

  m->dname=ufu_alloc_string(env,UFU_LEN_NAME);
  m->msg=ufu_alloc_string(env,UFU_LEN_MSG);

  m->entry=NULL;

  m->next=NULL;
  m->prev=NULL;

  ufu_mod_lmem(env,sizeof(struct s_mark));

  return(m);

}

struct s_ucmd *ufu_alloc_ucmd(struct s_env *env) {

  struct s_ucmd *uc;

  strcpy(env->fun,"ufu_alloc_ucmd");
  ufu_wai(env);

  if((uc=(struct s_ucmd *)malloc(sizeof(struct s_ucmd)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_ucmd'!");
    ufu_log(env);
    sprintf(env->msg,"Needed: %ld bytes.",sizeof(struct s_ucmd));
    ufu_log(env);

    exit(1);

  }

  uc->exec=ufu_alloc_string(env,UFU_LEN_UCMD+1);
  uc->local=TRUE;
  uc->history=FALSE;
  uc->key=' ';

  uc->next=NULL;
  uc->prev=NULL;

  ufu_mod_lmem(env,sizeof(struct s_ucmd));

  return(uc);

}

struct s_remote *ufu_alloc_remote(struct s_env *env) {

  struct s_remote *rh;

  strcpy(env->fun,"ufu_alloc_remote");
  ufu_wai(env);

  if((rh=(struct s_remote *)malloc(sizeof(struct s_remote)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_remote'!");
    ufu_log(env);
    sprintf(env->msg,"Needed: %ld bytes.",sizeof(struct s_remote));
    ufu_log(env);

    exit(1);

  }

  rh->seqno=0;
  rh->port=0;

  rh->hostname=ufu_alloc_string(env,UFU_REM_HOSTNAME);
  rh->username=ufu_alloc_string(env,UFU_REM_USERNAME);
  rh->password=ufu_alloc_string(env,UFU_REM_PASSWORD);
  rh->dirname=ufu_alloc_string(env,UFU_LEN_NAME);
  rh->dversion=ufu_alloc_string(env,UFU_LEN_VERSION);

  rh->next=NULL;
  rh->prev=NULL;

  rh->sockfd=0;

  ufu_mod_lmem(env,sizeof(struct s_remote));

  return(rh);

}

struct s_hist *ufu_alloc_hist(struct s_env *env) {

  struct s_hist *h;

  strcpy(env->fun,"ufu_alloc_hist");
  ufu_wai(env);

  if((h=(struct s_hist *)malloc(sizeof(struct s_hist)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_hist'!");
    ufu_log(env);
    sprintf(env->msg,"Needed: %ld bytes.",sizeof(struct s_hist));
    ufu_log(env);

    exit(1);

  }

  h->what=' ';
  h->dname=ufu_alloc_string(env,UFU_LEN_DNAME);
  h->seqno=0;
  h->next=NULL;
  h->prev=NULL;

  ufu_mod_lmem(env,sizeof(struct s_hist));

  return(h);

}

struct s_xcheck *ufu_alloc_xcheck(struct s_env *env) {

  struct s_xcheck *x;

  strcpy(env->fun,"ufu_alloc_xcheck");
  ufu_wai(env);

  if((x=(struct s_xcheck *)malloc(sizeof(struct s_xcheck)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_xcheck'!");
    ufu_log(env);
    sprintf(env->msg,"Needed: %ld bytes.",sizeof(struct s_xcheck));
    ufu_log(env);

    exit(1);

  }

  x->username=ufu_alloc_string(env,UFU_REM_USERNAME);
  x->nodename=ufu_alloc_string(env,UFU_REM_HOSTNAME);
  x->next=NULL;

  ufu_mod_lmem(env,sizeof(struct s_xcheck));

  return(x);

}

struct s_tc *ufu_alloc_tc(struct s_env *env) {

  struct s_tc *tc;

  strcpy(env->fun,"ufu_alloc_tc");
  ufu_wai(env);

  if((tc=(struct s_tc *)malloc(sizeof(struct s_tc)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_tc'!");
    ufu_log(env);
    sprintf(env->msg,"Needed: %ld bytes.",sizeof(struct s_tc));
    ufu_log(env);

    exit(1);

  }

  tc->nodename=ufu_alloc_string(env,UFU_REM_HOSTNAME);
  tc->dir=ufu_alloc_string(env,UFU_LEN_DNAME);
  tc->file=ufu_alloc_string(env,UFU_LEN_FNAME);
  tc->alias=ufu_alloc_string(env,UFU_LEN_FNAME);

  tc->next=NULL;
  tc->prev=NULL;

  tc->entry=ufu_alloc_entry(env);

  ufu_mod_lmem(env,sizeof(struct s_tc));

  return(tc);

}

char *ufu_alloc_string(struct s_env *env,int len) {

  char *p;

  // strcpy(env->fun,"ufu_alloc_string");
  // ufu_wai(env);

  if((p=(char *)malloc((sizeof(char)*len)+1))==NULL) {

    printf("Unable to allocate memory (%d bytes) for datatype 'char'!",len);

    // sprintf(env->msg,"Unable to allocate memory for datatype 'char'!");
    // ufu_log(env);
    // sprintf(env->msg,"Needed: %ld bytes.",(sizeof(char)*len)+1);
    // ufu_log(env);

    exit(1);

  }

  ufu_mod_lmem(env,sizeof(p));

  ufu_clear_string(env,p,len);

  return(p);

}

void ufu_free_env(struct s_env *env) {

  int i;
  struct s_mark *m,*m_next;
  struct s_ucmd *uc,*uc_next;
  struct s_msg *msg,*msg_next;

  strcpy(env->fun,"ufu_free_env");
  ufu_wai(env);

  sprintf(env->msg,"Releasing global environment.");
  ufu_log(env);

  for(i=UFU_MIN_PANEL;i<=env->mpanel;i++) {
    ufu_free_panel(env,i);
  }

  uc=env->ucfirst;
  while(uc!=NULL) {
    uc_next=uc->next;
    ufu_free_ucmd(env,uc);
    uc=uc_next;
  }

  m=env->mfirst;
  while(m!=NULL) {
    m_next=m->next;
    ufu_free_mark(env,m);
    m=m_next;
  }
  env->mfirst=NULL;
  env->mlast=NULL;

  msg=env->msgfirst;
  while(msg!=NULL) {
    msg_next=msg->next;
    ufu_free_msg(env,msg);
    msg=msg_next;
  }
  env->msgfirst=NULL;

  ufu_free_users(env);
  ufu_free_groups(env);
  ufu_free_remote_hosts(env);

  ufu_free_string(env,env->cfglocal);
  ufu_free_string(env,env->cfgglobal);
  ufu_free_string(env,env->msgglobal);
  ufu_free_string(env,env->logname);

  ufu_free_string(env,env->sh_hfile);

  ufu_free_string(env,env->editor);
  ufu_free_string(env,env->viewer);

  ufu_free_string(env,env->gname);

  ufu_free_string(env,env->tcdir);
  ufu_free_string(env,env->tcfile);

  ufu_free_string(env,env->nodename);
  ufu_free_string(env,env->sysname);
  ufu_free_string(env,env->release);

  ufu_free_string(env,env->master);

  sprintf(env->msg,"End of release.");
  ufu_log(env);

  ufu_close_log(env);

  ufu_free_string(env,env->msg);
  ufu_free_string(env,env->fun);

  ufu_mod_lmem(env,(long)(0-sizeof(struct s_env)));

  free(env);

}

void ufu_free_panel(struct s_env *env,int p) {

  strcpy(env->fun,"ufu_free_panel");
  ufu_wai(env);

  if(env->panel[p]!=NULL) {

    if(env->panel[p]->first!=NULL) {
      ufu_free_entries(env,p);
    }

    ufu_free_string(env,env->panel[p]->dirname);
    ufu_free_string(env,env->panel[p]->dirnameprev);

    ufu_free_string(env,env->panel[p]->nodename);

    ufu_mod_lmem(env,(long)(0-sizeof(struct s_panel)));

    free(env->panel[p]);

  }

}

void ufu_free_entries(struct s_env *env,int p) {

  struct s_entry *e,*tmp_e;

  strcpy(env->fun,"ufu_free_entries");
  ufu_wai(env);

  sprintf(env->msg," Releasing panel #%d (%s).",p,env->panel[p]->dirname);
  ufu_log(env);

  e=env->panel[p]->first;
  while(e!=NULL) {
    tmp_e=e->next;
    ufu_mod_lmem(env,(long)(0-sizeof(struct s_entry)));
    ufu_free_entry(env,e);
    e=tmp_e;
  }

}

void ufu_free_entry(struct s_env *env,struct s_entry *e) {

  strcpy(env->fun,"ufu_free_entry");
  ufu_wai(env);

  if(env->debug) {
    sprintf(env->msg," Releasing entry \"%s\".",e->fname);
    ufu_log(env);
  }

}

void ufu_free_split(struct s_env *env,struct s_split *s) {

  strcpy(env->fun,"ufu_free_split");
  ufu_wai(env);

  if(env->debug) {
    sprintf(env->msg," Releasing split \"%s\".",s->name);
    ufu_log(env);
  }

  if(s!=NULL) {
    if(s->name!=NULL) {
      ufu_free_string(env,s->name);
    }

    ufu_mod_lmem(env,(long)(0-sizeof(struct s_split)));

    free(s);
  }

}

void ufu_free_string(struct s_env *env,char *s) {

  // strcpy(env->fun,"ufu_free_string");
  // ufu_wai(env);

  if(s!=NULL) {
    ufu_mod_lmem(env,(long)(0-sizeof(s)));
    free(s);
  }

}

void ufu_free_usr(struct s_env *env,struct s_usr *u) {

  strcpy(env->fun,"ufu_free_usr");
  ufu_wai(env);

  if(u!=NULL) {
    ufu_free_string(env,u->uname);
    ufu_free_string(env,u->pword);
    ufu_free_string(env,u->dir);
    ufu_free_string(env,u->sh);
    ufu_mod_lmem(env,(long)(0-sizeof(struct s_usr)));
    free(u);
  }

}

void ufu_free_grp(struct s_env *env,struct s_grp *g) {

  struct s_grpusr *gu;

  strcpy(env->fun,"ufu_free_grp");
  ufu_wai(env);

  if(g!=NULL) {
    ufu_free_string(env,g->gname);
    gu=g->fgrpusr;
    while(gu!=NULL) {
      ufu_free_grpusr(env,gu);
      gu=gu->next;
    }
    g->fgrpusr=NULL;
    ufu_mod_lmem(env,(long)(0-sizeof(struct s_grp)));
    free(g);
  }

}

void ufu_free_grpusr(struct s_env *env,struct s_grpusr *gu) {

  strcpy(env->fun,"ufu_free_grpusr");
  ufu_wai(env);

  if(gu!=NULL) {
    ufu_free_string(env,gu->uname);
    ufu_mod_lmem(env,(long)(0-sizeof(struct s_grpusr)));
    free(gu);
  }

}

void ufu_free_msg(struct s_env *env,struct s_msg *m) {

  strcpy(env->fun,"ufu_free_msg");
  ufu_wai(env);

  if(m!=NULL) {
    ufu_free_string(env,m->id);
    ufu_free_string(env,m->msg);
    ufu_mod_lmem(env,(long)(0-sizeof(struct s_msg)));
    free(m);
  }

}

void ufu_free_log(struct s_env *env,struct s_log *t) {

  if(t!=NULL) {
    ufu_free_string(env,t->ts);
    ufu_free_string(env,t->text);
    ufu_mod_lmem(env,(long)(0-sizeof(struct s_log)));
    free(t);
  }

}

void ufu_free_txt(struct s_env *env,struct s_txt *t) {

  strcpy(env->fun,"ufu_free_txt");
  ufu_wai(env);

  if(t!=NULL) {
    ufu_free_string(env,t->line);
    ufu_mod_lmem(env,(long)(0-sizeof(struct s_txt)));
    free(t);
  }

}

void ufu_free_dirent(struct s_env *env,struct dirent *de) {

  strcpy(env->fun,"ufu_free_dirent");
  ufu_wai(env);

  if(de!=NULL) {
    ufu_mod_lmem(env,(long)(0-sizeof(struct dirent)));
    free(de);
  }

}

void ufu_free_stat(struct s_env *env,struct stat *st) {

  strcpy(env->fun,"ufu_free_stat");
  ufu_wai(env);

  if(st!=NULL) {
    ufu_mod_lmem(env,(long)(0-sizeof(struct stat)));
    free(st);
  }

}

void ufu_free_mark(struct s_env *env,struct s_mark *mark) {

  strcpy(env->fun,"ufu_free_mark");
  ufu_wai(env);

  if(mark!=NULL) {
    if(mark->entry!=NULL) {
      ufu_free_entry(env,mark->entry);
    }
    ufu_free_string(env,mark->dname);
    ufu_free_string(env,mark->msg);
    ufu_mod_lmem(env,(long)(0-sizeof(struct s_mark)));
    free(mark);
  }

}

void ufu_free_ucmd(struct s_env *env,struct s_ucmd *uc) {

  strcpy(env->fun,"ufu_free_ucmd");
  ufu_wai(env);

  if(uc!=NULL) {
    ufu_free_string(env,uc->exec);
    ufu_mod_lmem(env,(long)(0-sizeof(struct s_ucmd)));
    free(uc);
  }

}

void ufu_free_remote(struct s_env *env,struct s_remote *rh) {

  strcpy(env->fun,"ufu_free_remote");
  ufu_wai(env);

  if(rh!=NULL) {
    ufu_free_string(env,rh->hostname);
    ufu_free_string(env,rh->username);
    ufu_free_string(env,rh->password);
    ufu_free_string(env,rh->dirname);
    ufu_free_string(env,rh->dversion);
    ufu_mod_lmem(env,(long)(0-sizeof(struct s_remote)));
    free(rh);
  }

}

void ufu_free_hist(struct s_env *env,struct s_hist *h) {

  strcpy(env->fun,"ufu_free_hist");
  ufu_wai(env);

  if(h!=NULL) {
    ufu_free_string(env,h->dname);
    ufu_mod_lmem(env,(long)(0-sizeof(struct s_hist)));
    free(h);
  }

}

void ufu_free_xcheck(struct s_env *env,struct s_xcheck *x) {

  strcpy(env->fun,"ufu_free_xcheck");
  ufu_wai(env);

  if(x!=NULL) {
    ufu_free_string(env,x->username);
    ufu_free_string(env,x->nodename);
    ufu_mod_lmem(env,(long)(0-sizeof(struct s_xcheck)));
    free(x);
  }

}

void ufu_free_tc(struct s_env *env,struct s_tc *tc) {

  strcpy(env->fun,"ufu_free_tc");
  ufu_wai(env);

  if(tc!=NULL) {
    ufu_free_string(env,tc->nodename);
    ufu_free_string(env,tc->dir);
    ufu_free_string(env,tc->file);
    ufu_free_string(env,tc->alias);
    ufu_free_entry(env,tc->entry);
    ufu_mod_lmem(env,(long)(0-sizeof(struct s_tc)));
    free(tc);
  }

}

struct s_buffer *ufu_alloc_buffer(struct s_env *env,int action) {

  struct s_buffer *b;

  strcpy(env->fun,"ufu_alloc_buffer");
  ufu_wai(env);

  if((b=(struct s_buffer *)malloc(sizeof(struct s_buffer)))==NULL) {

    exit(1);

  }

  ufu_mod_lmem(env,sizeof(struct s_buffer));

  ufu_clear_string(env,b->username,UFU_LEN_UNAME);
  ufu_clear_string(env,b->password,UFU_LEN_PWORD);
  ufu_clear_string(env,b->dirname,UFU_LEN_NAME);
  ufu_clear_string(env,b->filename,UFU_LEN_NAME);
  ufu_clear_string(env,b->errmsg,UFU_LEN_ERROR);

  ufu_clear_string(env,b->line,UFU_LEN_MSG);

  return(b);

}

struct s_buffer_action *ufu_alloc_buffer_action(struct s_env *env,int action) {

  struct s_buffer_action *b;

  strcpy(env->fun,"ufu_buffer_action");
  ufu_wai(env);

  if((b=(struct s_buffer_action *)malloc(sizeof(struct s_buffer_action)))==NULL) {

    exit(1);

  }

  ufu_mod_lmem(env,sizeof(struct s_buffer_action));

  ufu_clear_string(env,b->username,UFU_LEN_UNAME);
  ufu_clear_string(env,b->password,UFU_LEN_PWORD);
  ufu_clear_string(env,b->dirname,UFU_LEN_NAME);
  ufu_clear_string(env,b->filename,UFU_LEN_NAME);
  ufu_clear_string(env,b->errmsg,UFU_LEN_ERROR);

  ufu_clear_string(env,b->line,UFU_ACTION_BUFFER);

  return(b);

}

void ufu_free_buffer(struct s_env *env,struct s_buffer *b,int action) {

  strcpy(env->fun,"ufu_free_buffer");
  ufu_wai(env);

  ufu_mod_lmem(env,(long)(0-sizeof(struct s_buffer)));

  free(b);

}

void ufu_free_buffer_action(struct s_env *env,struct s_buffer_action *b,int action) {

  strcpy(env->fun,"ufu_free_buffer_action");
  ufu_wai(env);

  ufu_mod_lmem(env,(long)(0-sizeof(struct s_buffer_action)));

  free(b);

}

