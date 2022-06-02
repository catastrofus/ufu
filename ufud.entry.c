
#include "ufud.h"
#include "ufud.p"

int ufud_read_entries(struct s_env *env,struct s_buffer *b,int fd,char *ipno) {

  int c,dirread;
  char *s;
  struct s_entry *e,*tmp_e;
  struct dirent *de;
  DIR *dirp;

  env->efirst=NULL;
  dirread=FALSE;

  strcpy(env->dirname,b->dirname);

  env->ndirs=0;
  env->nfiles=0;
  env->nlinks=0;
  env->ntotal=0;

  if(strlen(env->dirname)>0) {

    sprintf(env->msg,"[%s] Reading dir %s.",ipno,env->dirname);
    ufud_log(env);

    if((dirp=opendir(env->dirname))!=NULL) {

      de=ufud_alloc_dirent(env);

      // Seqno.
      c=0;

      e=NULL;
      tmp_e=NULL;

      while((de=readdir(dirp))!=NULL) {

        tmp_e=e;
        e=ufud_alloc_entry(env);

        e->seqno=c++;
        strcpy(e->fname,de->d_name);

        sprintf(env->msg,"[%s] Reading entry \"%s\".",ipno,e->fname);
        ufud_log(env);

        if(env->efirst!=NULL) {
          e->next=NULL;
          e->prev=tmp_e;
          if(tmp_e!=NULL) {
            tmp_e->next=e;
          }
        }
        else {
          env->efirst=e;
          e->next=NULL;
          e->prev=NULL;
        }

        ufud_stat_entry(env,e,UFUD_ENTRIES);

        if(e->fstat!=NULL) {
          if(S_ISDIR(e->fstat->st_mode))  env->ndirs++;
          if(S_ISREG(e->fstat->st_mode))  env->nfiles++;
          env->ntotal++;
        }
        if(e->lstat!=NULL) {
          if(S_ISLNK(e->lstat->st_mode))  env->nlinks++;
        }

      }

      ufud_free_dirent(env,de);

      closedir(dirp);

      dirread=TRUE;

    }

    sprintf(env->msg,"[%s] Sending entries.",ipno);
    ufud_log(env);

    b->e_seqno=0;
    b->e_ndirs=0;
    b->e_nfiles=0;
    b->e_nlinks=0;
    b->e_ntotal=0;

    c=0;

    e=env->efirst;
    while(e!=NULL) {

      b->last=(e->next==NULL);

      if(b->last) {
        b->e_ndirs=env->ndirs;
        b->e_nfiles=env->nfiles;
        b->e_nlinks=env->nlinks;
        b->e_ntotal=env->ntotal;
      }

      b->e_seqno=e->seqno;

      ufud_clear_string(env,b->e_fname,UFUD_LEN_NAME);
      ufud_clear_string(env,b->e_fperm,UFUD_LEN_PERM);
      ufud_clear_string(env,b->e_lname,UFUD_LEN_NAME);
      ufud_clear_string(env,b->e_lperm,UFUD_LEN_PERM);

      strcpy(b->e_fname,e->fname);
      strcpy(b->e_fperm,e->fperm);
      strcpy(b->e_lname,e->lname);
      strcpy(b->e_lperm,e->lperm);

      b->e_is_dir=(S_ISDIR(e->fstat->st_mode)||S_ISDIR(e->lstat->st_mode));
      b->e_is_lnk=(S_ISLNK(e->fstat->st_mode)||S_ISLNK(e->lstat->st_mode));

      ufud_clear_string(env,b->e_fusername,UFUD_LEN_UNAME);
      ufud_clear_string(env,b->e_fgroupname,UFUD_LEN_GROUP);
      ufud_clear_string(env,b->e_lusername,UFUD_LEN_UNAME);
      ufud_clear_string(env,b->e_lgroupname,UFUD_LEN_GROUP);

      strcpy(b->e_fusername,ufud_search_user(env,e->fuid,ipno));
      strcpy(b->e_fgroupname,ufud_search_group(env,e->fgid,ipno));

      if(b->e_is_lnk) {
        strcpy(b->e_lusername,ufud_search_user(env,e->luid,ipno));
        strcpy(b->e_lgroupname,ufud_search_group(env,e->lgid,ipno));
      }

      b->e_fatime=e->fatime;
      b->e_fmtime=e->fmtime;
      b->e_fctime=e->fctime;

      b->e_latime=e->latime;
      b->e_lmtime=e->lmtime;
      b->e_lctime=e->lctime;

      b->e_u_read=e->u_read;
      b->e_u_write=e->u_write;
      b->e_u_exec=e->u_exec;

      b->e_g_read=e->g_read;
      b->e_g_write=e->g_write;
      b->e_g_exec=e->g_exec;

      b->e_o_read=e->o_read;
      b->e_o_write=e->o_write;
      b->e_o_exec=e->o_exec;

      b->e_fmode=e->fmode;
      b->e_lmode=e->lmode;

      b->e_size=(long) e->size;

      b->e_blocks=e->blocks;
      b->e_links=e->links;

      b->e_fino=e->fino;
      b->e_lino=e->lino;

      sprintf(env->msg,"[%s] Sending entry #%d \"%s\".",ipno,e->seqno,e->fname);
      ufud_log(env);

      b->error=FALSE;

      c++;

      ufud_send(env,b,fd,ipno);

      e=e->next;

    }

    if(!dirread) {

      b->last=TRUE;
      b->error=TRUE;

      s=ufud_alloc_string(env,UFUD_LEN_NAME);
      ufud_resize_string(env,env->dirname,36,s);

      sprintf(b->errmsg,"No permission to read directory %s!",s);

      ufud_send(env,b,fd,ipno);

      ufud_free_string(env,s);

    }

    sprintf(env->msg,"[%s] Finished sending entries.",ipno);
    ufud_log(env);

    ufud_free_entries(env,ipno);

    sprintf(env->msg,"[%s] Entries given to client: %d",ipno,c);
    ufud_log(env);

  }

  return(dirread);

}

int ufud_read_entry(struct s_env *env,struct s_buffer *b,int fd,char *ipno) {

  struct s_entry *e;

  b->error=TRUE;

  strcpy(env->dirname,b->dirname);

  if(strlen(env->dirname)>0) {

    e=ufud_alloc_entry(env);

    strcpy(e->fname,b->dirname);

    b->e_seqno=0;
    b->e_ndirs=0;
    b->e_nfiles=0;
    b->e_nlinks=0;
    b->e_ntotal=0;

    sprintf(env->msg,"[%s] Statting entry \"%s\".",ipno,e->fname);
    ufud_log(env);

    ufud_stat_entry(env,e,UFUD_ENTRY);

    ufud_clear_string(env,b->e_fname,UFUD_LEN_NAME);
    ufud_clear_string(env,b->e_fperm,UFUD_LEN_PERM);
    ufud_clear_string(env,b->e_lname,UFUD_LEN_NAME);
    ufud_clear_string(env,b->e_lperm,UFUD_LEN_PERM);

    strcpy(b->e_fname,e->fname);
    strcpy(b->e_fperm,e->fperm);
    strcpy(b->e_lname,e->lname);
    strcpy(b->e_lperm,e->lperm);

    b->e_is_dir=(S_ISDIR(e->fstat->st_mode)||S_ISDIR(e->lstat->st_mode));
    b->e_is_lnk=(S_ISLNK(e->fstat->st_mode)||S_ISLNK(e->lstat->st_mode));

    ufud_clear_string(env,b->e_fusername,UFUD_LEN_UNAME);
    ufud_clear_string(env,b->e_fgroupname,UFUD_LEN_GROUP);
    ufud_clear_string(env,b->e_lusername,UFUD_LEN_UNAME);
    ufud_clear_string(env,b->e_lgroupname,UFUD_LEN_GROUP);

    strcpy(b->e_fusername,ufud_search_user(env,e->fuid,ipno));
    strcpy(b->e_fgroupname,ufud_search_group(env,e->fgid,ipno));

    if(b->e_is_lnk) {
      strcpy(b->e_lusername,ufud_search_user(env,e->luid,ipno));
      strcpy(b->e_lgroupname,ufud_search_group(env,e->lgid,ipno));
    }

    b->e_fatime=e->fatime;
    b->e_fmtime=e->fmtime;
    b->e_fctime=e->fctime;

    b->e_latime=e->latime;
    b->e_lmtime=e->lmtime;
    b->e_lctime=e->lctime;

    b->e_u_read=e->u_read;
    b->e_u_write=e->u_write;
    b->e_u_exec=e->u_exec;

    b->e_g_read=e->g_read;
    b->e_g_write=e->g_write;
    b->e_g_exec=e->g_exec;

    b->e_o_read=e->o_read;
    b->e_o_write=e->o_write;
    b->e_o_exec=e->o_exec;

    b->e_fmode=e->fmode;
    b->e_lmode=e->lmode;

    b->e_size=e->size;

    b->e_blocks=e->blocks;
    b->e_links=e->links;

    b->e_fino=e->fino;
    b->e_lino=e->lino;

    sprintf(env->msg,"[%s] Sending entry \"%s\".",ipno,e->fname);
    ufud_log(env);

    b->error=FALSE;

    ufud_send(env,b,fd,ipno);

    ufud_free_entry(env,e,ipno);

  }

  return(!b->error);

}

void ufud_stat_entry(struct s_env *env,struct s_entry *e,int what) {

  int r;
  char *f,*s;

  f=ufud_alloc_string(env,UFUD_LEN_NAME);
  s=ufud_alloc_string(env,UFUD_LEN_NAME);

  switch(what) {
    case UFUD_ENTRY:
      strcpy(f,e->fname);
      break;
    case UFUD_ENTRIES:
      ufud_concat_dir(env,f,env->dirname,e->fname);
      break;
  }

  e->fstat=ufud_alloc_stat(env);
  e->lstat=ufud_alloc_stat(env);

  if(e->fstat!=NULL) {

    r=stat(f,e->fstat);
    lstat(f,e->lstat);

    ufud_clear_string(env,e->lperm,UFUD_LEN_PERM);
    ufud_clear_string(env,e->fperm,UFUD_LEN_PERM);

    strcpy(e->lperm,"----------");
    strcpy(e->fperm,"----------");

    if(S_ISLNK(e->lstat->st_mode)) {

      ufud_concat_dir(env,s,env->dirname,e->fname);
      ufud_clear_string(env,e->lname,UFUD_LEN_NAME);
      readlink(s,e->lname,UFUD_LEN_NAME);

      e->luid=e->lstat->st_uid;
      e->lgid=e->lstat->st_gid;
 
      e->latime=e->lstat->st_atime;
      e->lmtime=e->lstat->st_mtime;
      e->lctime=e->lstat->st_ctime;

      // Build permissionstring.
      if(S_ISDIR(e->lstat->st_mode))   e->lperm[0]='d';
      if(S_ISCHR(e->lstat->st_mode))   e->lperm[0]='c';
      if(S_ISBLK(e->lstat->st_mode))   e->lperm[0]='b';
      if(S_ISFIFO(e->lstat->st_mode))  e->lperm[0]='p';
      if(S_ISSOCK(e->lstat->st_mode))  e->lperm[0]='p';
      if(S_ISLNK(e->lstat->st_mode))   e->lperm[0]='l';

      if(e->lstat->st_mode&S_IRUSR)    e->lperm[1]='r';
      if(e->lstat->st_mode&S_IWUSR)    e->lperm[2]='w';
      if(e->lstat->st_mode&S_IXUSR)    e->lperm[3]='x';

      if(e->lstat->st_mode&S_IRGRP)    e->lperm[4]='r';
      if(e->lstat->st_mode&S_IWGRP)    e->lperm[5]='w';
      if(e->lstat->st_mode&S_IXGRP)    e->lperm[6]='x';

      if(e->lstat->st_mode&S_IROTH)    e->lperm[7]='r';
      if(e->lstat->st_mode&S_IWOTH)    e->lperm[8]='w';
      if(e->lstat->st_mode&S_IXOTH)    e->lperm[9]='x';

      if(e->lstat->st_mode&S_ISUID)    e->lperm[3]='s';
      if(e->lstat->st_mode&S_ISGID)    e->lperm[6]='s';
      if(e->lstat->st_mode&S_ISVTX)    e->lperm[9]='t';

      e->lmode=e->lstat->st_mode;
      e->lino=e->lstat->st_ino;

    }

    e->fatime=e->fstat->st_atime;
    e->fmtime=e->fstat->st_mtime;
    e->fctime=e->fstat->st_ctime;

    e->fmode=e->fstat->st_mode;
    e->fino=e->fstat->st_ino;

    e->fuid=e->fstat->st_uid;
    e->fgid=e->fstat->st_gid;
 
    e->size=(long)e->fstat->st_size;
    e->blocks=(long)e->fstat->st_blocks;
    e->links=(long)e->fstat->st_nlink;

    // Build permissionstring.
    if(S_ISDIR(e->fstat->st_mode))   e->fperm[0]='d';
    if(S_ISCHR(e->fstat->st_mode))   e->fperm[0]='c';
    if(S_ISBLK(e->fstat->st_mode))   e->fperm[0]='b';
    if(S_ISFIFO(e->fstat->st_mode))  e->fperm[0]='p';
    if(S_ISSOCK(e->fstat->st_mode))  e->fperm[0]='p';
    if(S_ISLNK(e->fstat->st_mode))   e->fperm[0]='l';

    if(e->fstat->st_mode&S_IRUSR)    e->fperm[1]='r';
    if(e->fstat->st_mode&S_IWUSR)    e->fperm[2]='w';
    if(e->fstat->st_mode&S_IXUSR)    e->fperm[3]='x';
    if(e->fstat->st_mode&S_IRGRP)    e->fperm[4]='r';
    if(e->fstat->st_mode&S_IWGRP)    e->fperm[5]='w';
    if(e->fstat->st_mode&S_IXGRP)    e->fperm[6]='x';

    if(e->fstat->st_mode&S_IROTH)    e->fperm[7]='r';
    if(e->fstat->st_mode&S_IWOTH)    e->fperm[8]='w';
    if(e->fstat->st_mode&S_IXOTH)    e->fperm[9]='x';

    if(e->fstat->st_mode&S_ISUID)    e->fperm[3]='s';
    if(e->fstat->st_mode&S_ISGID)    e->fperm[6]='s';
    if(e->fstat->st_mode&S_ISVTX)    e->fperm[9]='t';

  }

  ufud_free_string(env,f);
  ufud_free_string(env,s);

}

