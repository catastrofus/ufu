
#include "ufud.h"
#include "ufud.p"

struct s_env *ufud_alloc_env(void) {

  struct s_env *env;

  if((env=(struct s_env *)malloc(sizeof(struct s_env)))==NULL) {

    exit(2);

  }

  env->cfgfile=ufud_alloc_string(env,UFUD_LEN_NAME);
  env->logname=ufud_alloc_string(env,UFUD_LEN_NAME);
  env->dirname=ufud_alloc_string(env,UFUD_LEN_NAME);
  env->msg=ufud_alloc_string(env,UFUD_LEN_MSG);
  env->debug=TRUE;

  env->ppid=0;
  env->pid=0;

  strcpy(env->logname,UFUD_LOGFILE);
  strcpy(env->cfgfile,UFUD_CFGFILE);
  env->port=UFUD_PORT;
  env->backlog=UFUD_BACKLOG;

  ufud_open_log(env);

  // Read user & group file.
  env->ufirst=NULL;
  env->gfirst=NULL;

  env->ipfirst=NULL;

  ufud_read_users(env);
  ufud_read_groups(env);

  return(env);

}

struct s_usr *ufud_alloc_usr(struct s_env *env) {

  struct s_usr *u;

  if((u=(struct s_usr *)malloc(sizeof(struct s_usr)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_usr'!");
    ufud_log(env);
    sprintf(env->msg,"Needed: %d bytes.",sizeof(struct s_usr));
    ufud_log(env);

    exit(1);

  }

  u->uid=-1;
  u->gid=-1;
  u->uname=ufud_alloc_string(env,UFUD_LEN_UNAME);
  u->pword=ufud_alloc_string(env,UFUD_LEN_PWORD);
  u->dir=ufud_alloc_string(env,UFUD_LEN_UDIR);
  u->sh=ufud_alloc_string(env,UFUD_LEN_USH);

  return(u);

}

struct s_grp *ufud_alloc_grp(struct s_env *env) {

  struct s_grp *g;

  if((g=(struct s_grp *)malloc(sizeof(struct s_grp)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_grp'!");
    ufud_log(env);
    sprintf(env->msg,"Needed: %d bytes.",sizeof(struct s_grp));
    ufud_log(env);

    exit(1);

  }

  g->gid=-1;
  g->gname=ufud_alloc_string(env,UFUD_LEN_GROUP);

  return(g);

}

struct s_ip *ufud_alloc_ip(struct s_env *env) {

  struct s_ip *ip;

  if((ip=(struct s_ip *)malloc(sizeof(struct s_ip)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_ip'!");
    ufud_log(env);
    sprintf(env->msg,"Needed: %d bytes.",sizeof(struct s_ip));
    ufud_log(env);

    exit(1);

  }

  ip->ipno=ufud_alloc_string(env,UFUD_LEN_IPNO);
  ip->next=NULL;
  ip->prev=NULL;

  return(ip);

}

struct s_buffer *ufud_alloc_buffer(struct s_env *env,char *ipno) {

  struct s_buffer *b;

  if((b=(struct s_buffer *)malloc(sizeof(struct s_buffer)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_buffer'!");
    ufud_log(env);
    sprintf(env->msg,"Needed: %d bytes.",sizeof(struct s_buffer));
    ufud_log(env);

    exit(1);

  }

  sprintf(env->msg,"[%s] Allocating buffer (%d bytes) for data transfer",ipno,sizeof(struct s_buffer));
  ufud_log(env);

  return(b);

}

struct s_buffer_action *ufud_alloc_buffer_action(struct s_env *env,char *ipno) {

  struct s_buffer_action *ba;

  if((ba=(struct s_buffer_action *)malloc(sizeof(struct s_buffer_action)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_buffer_action'!");
    ufud_log(env);
    sprintf(env->msg,"Needed: %d bytes.",sizeof(struct s_buffer_action));
    ufud_log(env);

    exit(1);

  }

  sprintf(env->msg,"[%s] Allocating buffer (%d bytes) for data transfer",ipno,sizeof(struct s_buffer_action));
  ufud_log(env);

  return(ba);

}

struct s_entry *ufud_alloc_entry(struct s_env *env) {

  struct s_entry *e;

  if((e=(struct s_entry *)malloc(sizeof(struct s_entry)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 's_entry'!");
    ufud_log(env);
    sprintf(env->msg,"Needed: %d bytes.",sizeof(struct s_entry));
    ufud_log(env);

    return(NULL);

  }

  e->fstat=NULL;
  e->lstat=NULL;

  e->size=(long)0;

  e->fname=ufud_alloc_string(env,UFUD_LEN_NAME);
  e->fperm=ufud_alloc_string(env,UFUD_LEN_PERM);
  e->lname=ufud_alloc_string(env,UFUD_LEN_NAME);
  e->lperm=ufud_alloc_string(env,UFUD_LEN_PERM);

  return(e);

}

struct dirent *ufud_alloc_dirent(struct s_env *env) {

  struct dirent *d;

  if((d=(struct dirent *)malloc(sizeof(struct dirent)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 'dirent'!");
    ufud_log(env);
    sprintf(env->msg,"Needed: %d bytes.",sizeof(struct dirent));
    ufud_log(env);

    return(NULL);

  }

  return(d);

}

struct stat *ufud_alloc_stat(struct s_env *env) {

  struct stat *st;

  if((st=(struct stat *)malloc(sizeof(struct stat)))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for structure 'stat'!");
    ufud_log(env);
    sprintf(env->msg,"Needed: %d bytes.",sizeof(struct stat));
    ufud_log(env);

    return(NULL);

  }

  return(st);

}

char *ufud_alloc_string(struct s_env *env,int len) {

  char *p;

  if((p=(char *)malloc((sizeof(char)*len)+1))==NULL) {

    sprintf(env->msg,"Unable to allocate memory for datatype 'char'!");
    ufud_log(env);
    sprintf(env->msg,"Needed: %d bytes.",(sizeof(char)*len)+1);
    ufud_log(env);
    exit(1);

  }

  return(p);

}

void ufud_free_env(struct s_env *env) {

  sprintf(env->msg,"Releasing global environment.");
  ufud_log(env);

  ufud_free_string(env,env->cfgfile);
  ufud_free_string(env,env->logname);
  ufud_free_string(env,env->dirname);

  sprintf(env->msg,"End of release.");
  ufud_log(env);

  ufud_free_string(env,env->msg);

  ufud_close_log(env);

  free(env);

}

void ufud_free_usr(struct s_env *env,struct s_usr *u) {

  if(u!=NULL) {
    if(u->uname!=NULL)  free(u->uname);
    if(u->pword!=NULL)  free(u->pword);
    if(u->dir!=NULL)    free(u->dir);
    if(u->sh!=NULL)     free(u->sh);
    free(u);
  }

}

void ufud_free_grp(struct s_env *env,struct s_grp *g) {

  if(g!=NULL) {
    if(g->gname!=NULL)  free(g->gname);
    free(g);
  }

}

void ufud_free_ip(struct s_env *env,struct s_ip *ip) {

  if(ip!=NULL) {
    if(ip->ipno)  free(ip->ipno);
    free(ip);
  }

}

void ufud_free_buffer(struct s_env *env,struct s_buffer *b,char *ipno) {

  sprintf(env->msg,"[%s] Freeing transfer buffer.",ipno);
  ufud_log(env);

}

void ufud_free_buffer_action(struct s_env *env,struct s_buffer_action *b,char *ipno) {

  sprintf(env->msg,"[%s] Freeing transfer buffer.",ipno);
  ufud_log(env);

}

void ufud_free_string(struct s_env *env,char *s) {

  if(s!=NULL) {
    free(s);
  }

}

void ufud_free_entries(struct s_env *env,char *ipno) {

  struct s_entry *e,*tmp_e;

  sprintf(env->msg,"[%s] Releasing panel %s.",ipno,env->dirname);
  ufud_log(env);

  e=env->efirst;
  while(e!=NULL) {
    tmp_e=e->next;
    ufud_free_entry(env,e,ipno);
    e=tmp_e;
  }

}

void ufud_free_entry(struct s_env *env,struct s_entry *e,char *ipno) {

  if(env->debug) {
    sprintf(env->msg,"[%s] Releasing entry \"%s\".",ipno,e->fname);
    ufud_log(env);
  }

  if(e!=NULL) {
    if(e->fname!=NULL)  free(e->fname);
    if(e->fperm!=NULL)  free(e->fperm);
    if(e->lname!=NULL)  free(e->lname);
    if(e->lperm!=NULL)  free(e->lperm);
    if(e->fstat!=NULL)  ufud_free_stat(env,e->fstat);
    free(e);
  }

}

void ufud_free_dirent(struct s_env *env,struct dirent *de) {

  if(de!=NULL) {
    free(de);
  }

}

void ufud_free_stat(struct s_env *env,struct stat *st) {

  if(st!=NULL) {
    free(st);
  }

}

