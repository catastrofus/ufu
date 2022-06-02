
#include "ufu.h"
#include "ufu.p"

int ufu_read_entries(struct s_env *env,int p) {

  int c,dirread;
  struct s_entry *e,*tmp_e;
  struct dirent *de;
  DIR *dirp;

  strcpy(env->fun,"ufu_read_entries");
  ufu_wai(env);

  dirread=FALSE;

  if(strlen(env->panel[p]->dirname)>0) {

    sprintf(env->msg," Reading panel #%d (%s).",p,env->panel[p]->dirname);
    ufu_log(env);

    sprintf(env->msg,"Reading panel #%d (%s).",p,env->panel[p]->dirname);
    ufu_msg(env);

    strcpy(env->panel[p]->info->fname,env->panel[p]->dirname);

    env->panel[p]->first=NULL;
    env->panel[p]->last=NULL;

    if((dirp=opendir(env->panel[p]->dirname))!=NULL) {

      de=ufu_alloc_dirent(env);

      // Seqno.
      c=0;

      e=NULL;
      tmp_e=NULL;

      while((de=readdir(dirp))!=NULL) {

        tmp_e=e;
        e=ufu_alloc_entry(env);

        e->seqno=c++;
        strcpy(e->fname,de->d_name);

        if(env->debug) {
          sprintf(env->msg,"  Reading entry \"%s\".",e->fname);
          ufu_log(env);
        }

        if(env->panel[p]->first!=NULL) {
          e->next=NULL;
          e->prev=tmp_e;
          if(tmp_e!=NULL) {
            tmp_e->next=e;
          }
        }
        else {
          env->panel[p]->first=e;
          e->next=NULL;
          e->prev=NULL;
        }

        ufu_stat_entry(env,p,e);

        if(e->fstat!=NULL) {
          if(S_ISDIR(e->fstat->st_mode))  env->panel[p]->ndirs++;
          if(S_ISREG(e->fstat->st_mode))  env->panel[p]->nfiles++;
          env->panel[p]->ntotal++;
          env->panel[p]->size+=(long) e->fstat->st_size;
        }
        if(e->lstat!=NULL) {
          if(S_ISLNK(e->lstat->st_mode))  env->panel[p]->nlinks++;
        }

	e->marked=ufu_is_marked(env,p,e);

      }

      env->panel[p]->last=e;

      ufu_free_dirent(env,de);

      closedir(dirp);

      dirread=TRUE;

    }

  }

  ufu_sort(env,p);

  return(dirread);

}

void ufu_stat_entry2(struct s_env *env,int panel,struct s_entry *e) {

  int r;
  char *f,*s;

  strcpy(env->fun,"ufu_stat_entry");
  ufu_wai(env);

  f=ufu_alloc_string(env,UFU_LEN_NAME);
  s=ufu_alloc_string(env,UFU_LEN_NAME);

  if(panel<(env->mpanel+1)) {
    ufu_concat_dir(env,f,env->panel[panel]->dirname,e->fname);
  }
  else {
    strcpy(f,e->fname);
  }

// sprintf(env->msg,"File: %s",f);
// ufu_log(env);

  e->fstat=ufu_alloc_stat(env);
  e->lstat=ufu_alloc_stat(env);

  // if(e->fstat!=NULL) {
  if((e->fstat!=NULL)&&(e->lstat!=NULL)) {

// sprintf(env->msg,"e->fstat en e->lstat blijkbaar !NULL");
// ufu_log(env);

    r=stat(f,e->fstat);
    lstat(f,e->lstat);

    if(panel<(env->mpanel+1)) {
      if(S_ISLNK(e->lstat->st_mode)) {

        ufu_concat_dir(env,s,env->panel[panel]->dirname,e->fname);
        ufu_clear_string(env,e->lname,UFU_LEN_NAME);
        readlink(s,e->lname,UFU_LEN_NAME);

      }
 
      e->fatime=e->lstat->st_atime;
      e->fmtime=e->lstat->st_mtime;
      e->fctime=e->lstat->st_ctime;

      e->lmode=e->lstat->st_mode;

      // Build permissionstring.
      strcpy(e->lperm,"----------");

      // e->is_reg=FALSE;
      // e->is_dir=FALSE;
      // e->is_chr=FALSE;
      // e->is_blk=FALSE;
      // e->is_fifo=FALSE;
      // e->is_sock=FALSE;
      // e->is_lnk=FALSE;

      if(S_ISREG(e->lstat->st_mode))   e->lperm[0]='-';

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

// sprintf(env->msg,"e->lperm: %s",e->lperm);
// ufu_log(env);

    }
    else {

      e->fatime=e->fstat->st_atime;
      e->fmtime=e->fstat->st_mtime;
      e->fctime=e->fstat->st_ctime;

    }

    e->fmode=e->fstat->st_mode;

    e->size=(long)e->fstat->st_size;

    // Build permissionstring.
    strcpy(e->fperm,"----------");

    e->is_reg=FALSE;
    e->is_dir=FALSE;
    e->is_chr=FALSE;
    e->is_blk=FALSE;
    e->is_fifo=FALSE;
    e->is_sock=FALSE;
    e->is_lnk=FALSE;

    if(S_ISREG(e->fstat->st_mode)) {
      e->fperm[0]='-';
      e->is_reg=TRUE;
    }
    if(S_ISDIR(e->fstat->st_mode)) {
      e->fperm[0]='d';
      e->is_dir=TRUE;
    }
    if(S_ISCHR(e->fstat->st_mode)) {
      e->fperm[0]='c';
      e->is_chr=TRUE;
    }
    if(S_ISBLK(e->fstat->st_mode)) {
      e->fperm[0]='b';
      e->is_blk=TRUE;
    }
    if(S_ISFIFO(e->fstat->st_mode)) {
      e->fperm[0]='p';
      e->is_fifo=TRUE;
    }
    if(S_ISSOCK(e->fstat->st_mode)) {
      e->fperm[0]='p';
      e->is_sock=TRUE;
    }
    if(S_ISLNK(e->fstat->st_mode)) {
      e->fperm[0]='l';
      e->is_lnk=TRUE;
    }

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

// sprintf(env->msg,"e->fperm: %s",e->fperm);
// ufu_log(env);

  }

  ufu_free_string(env,f);
  ufu_free_string(env,s);

}

void ufu_stat_entry(struct s_env *env,int panel,struct s_entry *e) {

  int r1,r2;
  char *f,*l;

  strcpy(env->fun,"ufu_stat_entry");
  ufu_wai(env);

  f=ufu_alloc_string(env,UFU_LEN_NAME);
  l=ufu_alloc_string(env,UFU_LEN_NAME);

  ufu_concat_dir(env,f,env->panel[panel]->dirname,e->fname);

  e->fstat=ufu_alloc_stat(env);
  e->lstat=ufu_alloc_stat(env);

  r1=stat(f,e->fstat);
  r2=lstat(f,e->lstat);

  e->is_reg=FALSE;
  e->is_dir=FALSE;
  e->is_chr=FALSE;
  e->is_blk=FALSE;
  e->is_fifo=FALSE;
  e->is_sock=FALSE;
  e->is_lnk=FALSE;

  strcpy(e->fperm,"----------");

  if(S_ISREG(e->fstat->st_mode)) {
    e->fperm[0]='-';
    e->is_reg=TRUE;
  }
  if(S_ISDIR(e->fstat->st_mode)) {
    e->fperm[0]='d';
    e->is_dir=TRUE;
  }
  if(S_ISCHR(e->fstat->st_mode)) {
    e->fperm[0]='c';
    e->is_chr=TRUE;
  }
  if(S_ISBLK(e->fstat->st_mode)) {
    e->fperm[0]='b';
    e->is_blk=TRUE;
  }
  if(S_ISFIFO(e->fstat->st_mode)) {
    e->fperm[0]='p';
    e->is_fifo=TRUE;
  }
  if(S_ISSOCK(e->fstat->st_mode)) {
    e->fperm[0]='p';
    e->is_sock=TRUE;
  }
  if(S_ISLNK(e->fstat->st_mode)) {
    e->fperm[0]='l';
    e->is_lnk=TRUE;
  }

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

  e->u_read=e->fstat->st_mode&S_IRUSR;
  e->u_write=e->fstat->st_mode&S_IWUSR;
  e->u_exec=e->fstat->st_mode&S_IXUSR;
  e->g_read=e->fstat->st_mode&S_IRGRP;
  e->g_write=e->fstat->st_mode&S_IWGRP;
  e->g_exec=e->fstat->st_mode&S_IXGRP;
  e->o_read=e->fstat->st_mode&S_IROTH;
  e->o_write=e->fstat->st_mode&S_IWOTH;
  e->o_exec=e->fstat->st_mode&S_IXOTH;
  e->u_stick=e->fstat->st_mode&S_ISUID;
  e->g_stick=e->fstat->st_mode&S_ISGID;
  e->o_stick=e->fstat->st_mode&S_ISVTX;

  if(S_ISLNK(e->lstat->st_mode)) {

    ufu_concat_dir(env,l,env->panel[panel]->dirname,e->fname);
    ufu_clear_string(env,e->lname,UFU_LEN_NAME);
    readlink(l,e->lname,UFU_LEN_NAME);

    // Build permissionstring.
    strcpy(e->lperm,"----------");

    if(S_ISREG(e->lstat->st_mode)) {
      e->lperm[0]='-';
      // e->is_reg=TRUE;
    }
    if(S_ISDIR(e->lstat->st_mode)) {
      e->lperm[0]='d';
      // e->is_dir=TRUE;
    }
    if(S_ISCHR(e->lstat->st_mode)) {
      e->lperm[0]='c';
      // e->is_chr=TRUE;
    }
    if(S_ISBLK(e->lstat->st_mode)) {
      e->lperm[0]='b';
      // e->is_blk=TRUE;
    }
    if(S_ISFIFO(e->lstat->st_mode)) {
      e->lperm[0]='p';
      // e->is_fifo=TRUE;
    }
    if(S_ISSOCK(e->lstat->st_mode)) {
      e->lperm[0]='p';
      // e->is_sock=TRUE;
    }
    if(S_ISLNK(e->lstat->st_mode)) {
      e->lperm[0]='l';
      // e->is_lnk=TRUE;
    }

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

    e->latime=e->lstat->st_atime;
    e->lmtime=e->lstat->st_mtime;
    e->lctime=e->lstat->st_ctime;

  }
 
  e->fatime=e->fstat->st_atime;
  e->fmtime=e->fstat->st_mtime;
  e->fctime=e->fstat->st_ctime;

  e->lmode=e->fstat->st_mode;

  e->size=(long)e->fstat->st_size;

  ufu_free_string(env,f);
  ufu_free_string(env,l);

}

void ufu_info_entry(struct s_env *env,struct s_entry *e,int p,struct s_mark *mark,struct s_panel *panel,int what) {

  int row,first,len,key;
  long int size,offset,k,m,g;
  char *s,*s1,*s2,*s3,*s4;

  strcpy(env->fun,"ufu_info_entry");
  ufu_wai(env);

  s=ufu_alloc_string(env,80);
  s4=ufu_alloc_string(env,32);
  s1=ufu_alloc_string(env,UFU_LEN_NAME);
  s2=ufu_alloc_string(env,UFU_LEN_NAME);

  if(what==UFU_INFO_XINFO) {
    s3=ufu_alloc_string(env,strlen(e->fname)+1);
    strcpy(s3,e->fname);
  }

  ufu_wclear(env->top);
  switch(what) {
    case UFU_INFO_MARK:
      if(env->panel[p]->remote)
        sprintf(s1,"[%s!%s] FileInfo for marked entry",env->panel[mark->panel]->rem->hostname,env->panel[mark->panel]->rem->username);
      else
        sprintf(s1,"[%s] FileInfo for marked entry",env->nodename);
      ufu_resize_string(env,e->fname,76-strlen(s1),s2);
      break;
    case UFU_INFO_PANEL:
      if(env->panel[p]->remote)
        sprintf(s1,"[%s!%s] FileInfo for panel entry",env->panel[p]->rem->hostname,env->panel[p]->rem->username);
      else
        sprintf(s1,"[%s] FileInfo for panel entry",env->nodename);
      ufu_resize_string(env,e->fname,76-strlen(s1),s2);
      break;
    case UFU_INFO_ENTRY:
      if(env->panel[p]->remote)
        sprintf(s1,"[%s!%s] FileInfo for entry ",env->panel[p]->rem->hostname,env->panel[p]->rem->username);
      else
        sprintf(s1,"[%s] FileInfo for entry ",env->nodename);
      ufu_resize_string(env,e->fname,76-strlen(s1),s2);
      break;
    case UFU_INFO_XINFO:
      if(env->panel[p]->remote)
        sprintf(s1,"[%s!%s] FileInfo for (x)entry ",env->panel[p]->rem->hostname,env->panel[p]->rem->username);
      else
        sprintf(s1,"[%s] FileInfo for (x)entry ",env->nodename);
      ufu_resize_string(env,e->fname,76-strlen(s1),s2);
      break;
  }
  mvwprintw(env->top,0,0,"%s \"%s\".",s1,s2);
  ufu_wrefresh(env->top);

  ufu_wclear(env->body);

  row=1;

  if(env->panel[p]->remote)
    size=e->size;
  else
    size=e->fstat->st_size;

  if((size%1024)==0)
    offset=0;
  else
    offset=1;
  k=((size*10)/1024/10)+offset;
  m=((k*10)/1024/10)+offset;
  g=((m*10)/1024/10)+offset;

  sprintf(s,"%ld KB",k);
  if(m>1)  sprintf(s,"%s, %ld MB",s,m);
  if(g>1)  sprintf(s,"%s, %ld GB",s,g);

  switch(what) {
    case UFU_INFO_MARK:
      ufu_resize_string(env,mark->dname,54,s1);
      break;
    case UFU_INFO_PANEL:
      ufu_resize_string(env,dirname(env->panel[p]->dirname),54,s1);
      break;
    case UFU_INFO_ENTRY:
      ufu_resize_string(env,env->panel[p]->dirname,54,s1);
      break;
    case UFU_INFO_XINFO:
      strcpy(e->fname,s3);
      ufu_resize_string(env,dirname(e->fname),54,s1);
      break;
  }
  mvwprintw(env->body,row++,14,"Directory: %s",s1);

  if(strcmp(e->fname,".")==0) {
    mvwprintw(env->body,row++,15,"Filename: . (current directory)");
  }
  else {
    if(strcmp(e->fname,"..")==0) {
      mvwprintw(env->body,row++,15,"Filename: .. (up directory)");
    }
    else {
      switch(what) {
        case UFU_INFO_PANEL:
          ufu_resize_string(env,basename(e->fname),54,s1);
          break;
        case UFU_INFO_MARK:
          ufu_resize_string(env,basename(e->fname),54,s1);
          break;
        case UFU_INFO_XINFO:
          strcpy(e->fname,s3);
          ufu_resize_string(env,basename(e->fname),54,s1);
          break;
        default:
          ufu_resize_string(env,e->fname,54,s1);
          break;
      }
      mvwprintw(env->body,row++,15,"Filename: %s",s1);
    }
  }

  mvwprintw(env->body,row,15,"Filetype:");
  ufu_clear_string(env,s4,31);

  if(e->is_reg)   strcpy(s4,"Regular file");
  if(e->is_dir)   strcpy(s4,"Directory");
  if(e->is_chr)   strcpy(s4,"Char device");
  if(e->is_blk)   strcpy(s4,"Block device");
  if(e->is_lnk)   strcpy(s4,"Symbolic link");
  if(e->is_fifo)  strcpy(s4,"Named pipe");
  if(e->is_sock)  strcpy(s4,"Socket");

  if(e->status_action!=UFU_MARK_ACTION_NONE) {
    strcat(s4,", ");
    switch(e->status_action) {
      case UFU_MARK_ACTION_REMOVE:
        strcat(s4,"removed");
        break;
      case UFU_MARK_ACTION_COPY:
        strcat(s4,"copied");
        break;
      case UFU_MARK_ACTION_MOVE:
        strcat(s4,"moved");
        break;
    }
  }

  mvwprintw(env->body,row++,25,s4);
  if(env->panel[p]->remote) {
    if(e->is_lnk) {
      ufu_resize_string(env,e->lname,54,s1);
      mvwprintw(env->body,row++,15,"Links to: %s",s1);
    }
  }
  else {
    if(S_ISLNK(e->lstat->st_mode)) {
      ufu_resize_string(env,e->lname,54,s1);
      mvwprintw(env->body,row++,15,"Links to: %s",s1);
    }
  }

  mvwprintw(env->body,row++,19,"Size: %ld bytes (%d blocks, %s)",e->size,e->blocks,s);

  if(panel!=NULL) {
    size=(long) panel->size;
    if((size%(long) 1024)==0)
      offset=(long) 0;
    else
      offset=(long) 1;
    k=((size*(long) 10)/(long) 1024/(long) 10)+offset;
    m=((k*(long) 10)/(long) 1024/(long) 10)+offset;
    g=((m*(long) 10)/(long) 1024/(long) 10)+offset;
    sprintf(s,"%ld KB",k);
    if(m>(long) 1)  sprintf(s,"%s, %ld MB",s,m);
    if(g>(long) 1)  sprintf(s,"%s, %ld GB",s,g);
    mvwprintw(env->body,row++,13,"Panel size: %ld bytes (%s)",size,s);
  }

  if(S_ISLNK(e->lmode)) {
    mvwprintw(env->body,row++,13,"Permission: %s [%s]",e->fperm,e->lperm);
  }
  else {
    mvwprintw(env->body,row++,13,"Permission: %s",e->fperm);
  }

  first=TRUE;
  ufu_clear_string(env,s,80);
  if(e->u_read) {
    strcat(s,"read");
    first=FALSE;
  }
  if(e->u_write) {
    if(!first)
      strcat(s,", write");
    else
      strcat(s,"write");
    first=FALSE;
  }
  if(e->u_exec) {
    if(!first)
      strcat(s,", execute");
    else
      strcat(s,"execute");
    first=FALSE;
  }
  if(e->u_stick) {
    if(!first)
      strcat(s,", UID bit set");
    else
      strcat(s,"UID bit set");
    first=FALSE;
  }
  if(first) strcpy(s,"none");
  mvwprintw(env->body,row++,8,"User permission: %s",s);

  first=TRUE;
  ufu_clear_string(env,s,80);
  if(e->g_read) {
    if(!first)
      strcat(s,", read");
    else
      strcat(s,"read");
    first=FALSE;
  }
  if(e->g_write) {
    if(!first)
      strcat(s,", write");
    else
      strcat(s,"write");
    first=FALSE;
  }
  if(e->g_exec) {
    if(!first)
      strcat(s,", execute");
    else
      strcat(s,"execute");
    first=FALSE;
  }
  if(e->g_stick) {
    if(!first)
      strcat(s,", GID bit set");
    else
      strcat(s,"GID bit set");
    first=FALSE;
  }
  if(first) strcpy(s,"none");
  mvwprintw(env->body,row++,7,"Group permission: %s",s);

  first=TRUE;
  ufu_clear_string(env,s,80);
  if(e->o_read) {
    if(!first)
      strcat(s,", read");
    else
      strcat(s,"read");
    first=FALSE;
  }
  if(e->o_write) {
    if(!first)
      strcat(s,", write");
    else
      strcat(s,"write");
    first=FALSE;
  }
  if(e->o_exec) {
    if(!first)
      strcat(s,", execute");
    else
      strcat(s,"execute");
    first=FALSE;
  }
  if(e->o_stick) {
    if(!first)
      strcat(s,", sticky");
    else
      strcat(s,"sticky");
    first=FALSE;
  }
  if(first) strcpy(s,"none");
  mvwprintw(env->body,row++,7,"Other permission: %s",s);

  if(env->panel[p]->remote) {
    if(S_ISLNK(e->lmode)) {
      mvwprintw(env->body,row++,13,"User/Group: %s [%s] / %s [%s]",e->fusrname,e->lusrname,e->fgrpname,e->lgrpname);
    }
    else {
      mvwprintw(env->body,row++,13,"User/Group: %s / %s",e->fusrname,e->fgrpname);
    }
    mvwprintw(env->body,row++,13,"Hard links: %d (%d)",e->links,e->blocks);
    if(e->is_lnk)
      mvwprintw(env->body,row++,18,"Inode: %d [%d]",e->fino,e->lino);
    else
      mvwprintw(env->body,row++,18,"Inode: %d",e->fino);
  }
  else {
    if(S_ISLNK(e->lmode)) {
      mvwprintw(env->body,row++,13,"User/Group: %s [%s] / %s [%s]",ufu_search_user(env,e->fstat->st_uid),ufu_search_user(env,e->lstat->st_uid),ufu_search_group(env,ufu_search_user_gid(env,e->fstat->st_uid)),ufu_search_group(env,ufu_search_user_gid(env,e->lstat->st_uid)));
    }
    else {
      mvwprintw(env->body,row++,13,"User/Group: %s / %s",ufu_search_user(env,e->fstat->st_uid),ufu_search_group(env,ufu_search_user_gid(env,e->fstat->st_uid)));
    }

    mvwprintw(env->body,row++,13,"Hard links: %d (%d)",e->fstat->st_nlink,e->fstat->st_blksize);

    if(S_ISLNK(e->lstat->st_mode)) {
      mvwprintw(env->body,row++,18,"Inode: %d [%d]",e->fstat->st_ino,e->lstat->st_ino);
    }
    else {
      mvwprintw(env->body,row++,18,"Inode: %d",e->fstat->st_ino);
    }
  }

  if(S_ISLNK(e->lmode)) {
    ufu_sdate(env,&e->fatime);
    mvwprintw(env->body,row,12,"Last access: %s",env->msg);
    ufu_sdate(env,&e->latime);
    mvwprintw(env->body,row++,50,"[%s]",env->msg);
  }
  else {
    ufu_sdate(env,&e->fatime);
    mvwprintw(env->body,row++,12,"Last access: %s",env->msg);
  }

  if(S_ISLNK(e->lmode)) {
    ufu_sdate(env,&e->fmtime);
    mvwprintw(env->body,row,6,"Last modification: %s",env->msg);
    ufu_sdate(env,&e->lmtime);
    mvwprintw(env->body,row++,50,"[%s]",env->msg);
  }
  else {
    ufu_sdate(env,&e->fmtime);
    mvwprintw(env->body,row++,6,"Last modification: %s",env->msg);
  }

  if(S_ISLNK(e->lmode)) {
    ufu_sdate(env,&e->fctime);
    mvwprintw(env->body,row,5,"Last status change: %s",env->msg);
    ufu_sdate(env,&e->lctime);
    mvwprintw(env->body,row++,50,"[%s]",env->msg);
  }
  else {
    ufu_sdate(env,&e->fctime);
    mvwprintw(env->body,row++,5,"Last status change: %s",env->msg);
  }

  switch(what) {
    case UFU_INFO_MARK:
      mvwprintw(env->body,row++,12,"Panel/Seqno: %d/%d",mark->panel,mark->seqno);
      mvwprintw(env->body,row++,18,"Level: %d",mark->level);
      if(env->panel[p]->remote) {
        mvwprintw(env->body,row++,2,"Remote daemon version: %s",env->panel[mark->panel]->rem->dversion);
        mvwprintw(env->body,row++,12,"Credentials: %s!%s",env->panel[mark->panel]->rem->hostname,env->panel[mark->panel]->rem->username);
      }
      break;
    case UFU_INFO_ENTRY:
      mvwprintw(env->body,row++,12,"Panel/Seqno: %d/%d",p,e->seqno);
      if(env->panel[p]->remote) {
        mvwprintw(env->body,row++,2,"Remote daemon version: %s",env->panel[p]->rem->dversion);
        mvwprintw(env->body,row++,12,"Credentials: %s!%s",env->panel[p]->rem->hostname,env->panel[p]->rem->username);
      }
      break;
    case UFU_INFO_PANEL:
      mvwprintw(env->body,row++,18,"Panel: %d",p);
      strcpy(env->panel[p]->dirname,e->fname);
      if(env->panel[p]->remote) {
        mvwprintw(env->body,row++,2,"Remote daemon version: %s",env->panel[p]->rem->dversion);
        mvwprintw(env->body,row++,12,"Credentials: %s!%s",env->panel[p]->rem->hostname,env->panel[p]->rem->username);
      }
      break;
  }

  ufu_wrefresh(env->body);

  ufu_free_string(env,s);
  ufu_free_string(env,s1);
  ufu_free_string(env,s2);
  ufu_free_string(env,s4);

  if(what==UFU_INFO_XINFO) {
    ufu_free_string(env,s3);
  }

  key=ufu_get_key(env,UFU_NO_TEXT,NULL);

  switch(key) {
    case UFU_KEY_QMARK:
      if(what!=UFU_INFO_XINFO) {
        env->key_xinfo++;
        if(strcmp(env->panel[p]->dirname,"/")!=0) {
          switch(what) {
            case UFU_INFO_ENTRY:
              len=strlen(env->panel[p]->dirname)+strlen(e->fname)+1;
              s=ufu_alloc_string(env,len);
              ufu_concat_dir(env,s,env->panel[p]->dirname,e->fname);
              break;
            case UFU_INFO_MARK:
              len=strlen(mark->dname)+strlen(e->fname)+1;
              s=ufu_alloc_string(env,len);
              ufu_concat_dir(env,s,mark->dname,e->fname);
              break;
            case UFU_INFO_PANEL:
              len=strlen(env->panel[p]->dirname);
              s=ufu_alloc_string(env,len);
              strcpy(s,env->panel[p]->dirname);
              break;
          }
          ufu_xinfo_entry(env,p,s,what);
          ufu_free_string(env,s);
        }
      }
      break;

    case UFU_KEY_EMARK:
      if(what!=UFU_INFO_XINFO) {
        env->key_xinfo++;
        switch(what) {
          case UFU_INFO_ENTRY:
            ufu_size_entry(env,p,e);
            break;
        }
      }
      break;

  }

}

void ufu_xinfo_entry(struct s_env *env,int panel,char *s,int what) {

  int key,i,j,again,redraw,row,rows,seq_tos,seq_bos;
  char *s1,*s2,*s3,*s4,*token;
  struct s_split *split,*split1,*split2,*cos,*tos;
  struct s_entry *e;

  strcpy(env->fun,"ufu_xinfo_entry");
  ufu_wai(env);

  s1=ufu_alloc_string(env,strlen(s)+1);
  s4=ufu_alloc_string(env,strlen(s)+1);
  strcpy(s1,s);

  ufu_wclear(env->top);
  s2=ufu_alloc_string(env,env->cols+1);
  s3=ufu_alloc_string(env,env->cols+1);
  if(env->panel[panel]->remote)
    sprintf(s2,"[%s!%s] PathInfo for entry ",env->panel[panel]->rem->hostname,env->panel[panel]->rem->username);
  else
    sprintf(s2,"[%s] PathInfo for entry ",env->nodename);
  ufu_resize_string(env,basename(s),env->cols-strlen(s2)-1,s3);
  mvwprintw(env->top,0,0,"%s \"%s\".",s2,s3);
  ufu_wrefresh(env->top);
  ufu_free_string(env,s2);
  ufu_free_string(env,s3);

  i=0;

  sprintf(env->msg,"Start splitting string \"%s\".",s);
  ufu_log(env);

  // Split the string in tokens.
  split1=NULL;

  token=strtok(s,"/");

  while(token!=NULL) {

    if(env->sfirst!=NULL) {

      split1->next=ufu_alloc_split(env,strlen(token)+1);
      split2=split1->next;
      split2->prev=split1;
      strcpy(split2->name,token);
      split1=split2;

    }
    else {

      split1=ufu_alloc_split(env,strlen(token)+1);
      env->sfirst=split1;
      split2=split1;
      split2->prev=NULL;
      strcpy(split2->name,token);

    }

    split2->next=NULL;
    split2->seqno=i++;

    sprintf(env->msg,"Found split #%d: \"%s\".",split2->seqno,split2->name);
    ufu_log(env);

    token=strtok(NULL,"/");

  }

  sprintf(env->msg,"Finished splitting.");
  ufu_log(env);

  strcpy(s,s1);
  ufu_free_string(env,s1);

  ufu_wclear(env->body);

  rows=env->rows-6;
  split1=env->sfirst;

  cos=env->sfirst;
  tos=env->sfirst;

  again=TRUE;
  while(again) {

    if(redraw) {

      ufu_wclear(env->body);
      mvwprintw(env->body,0,1,"Lvl Len Split");
      mvwprintw(env->body,1,1,"--- --- ----------------------------------------------------------------------");

    }

    row=2;
    split=tos;

    while(row<=rows) {

      if(split!=NULL) {

        if(split==cos)  wattron(env->body,A_REVERSE);

        mvwprintw(env->body,row,1,"%3d",split->seqno);
        mvwprintw(env->body,row,5,"%3d",strlen(split->name));
        s1=ufu_alloc_string(env,72);
        ufu_resize_string(env,split->name,70,s1);
        mvwprintw(env->body,row,9,"%-70s",s1);
        ufu_free_string(env,s1);

        if(split==cos)  wattroff(env->body,A_REVERSE);

        split=split->next;

      }

      row++;

    }

    ufu_wrefresh(env->body);

    key=ufu_get_key(env,UFU_NO_TEXT,NULL);

    switch(key) {

      case UFU_KEY_QUIT:
        ufu_add_hist(env,UFU_HIST_XINFO,NULL,cos->seqno);
        again=FALSE;
        break;

      case UFU_KEY_SELECT:
      case UFU_KEY_ENTER:
      case UFU_KEY_MARK:
      case UFU_KEY_INFO:
        env->key_select++;
        ufu_add_hist(env,UFU_HIST_XINFO,NULL,cos->seqno);

        split=env->sfirst;
        strcpy(s4,"");

        j=0;
        while(j<=cos->seqno) {
          strcat(s4,"/");
          strcat(s4,split->name);
          split=split->next;
          j++;
        }

        e=ufu_alloc_entry(env);
        strcpy(e->fname,s4);
        ufu_stat_entry(env,env->mpanel+1,e);
        ufu_info_entry(env,e,env->mpanel+1,NULL,NULL,UFU_INFO_XINFO);
        ufu_free_entry(env,e);
        break;

      case UFU_KEY_HELP:
      case UFU_KEY_F1:
        env->key_help++;
        ufu_help(env,env->mpanel+1,UFU_HELP_XINFO);
        redraw=TRUE;
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
        tos=env->sfirst;
        cos=env->sfirst;
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

      case UFU_KEY_NEXTPAGE:
        env->key_next_page++;
        i=0;
        while((i<rows)&&(tos->next!=NULL)) {
          tos=tos->next;
          i++;
        }
        cos=tos;
        break;

      case UFU_KEY_PREVPAGE:
        env->key_prev_page++;
        i=0;
        while((i<rows)&&(tos->prev!=NULL)) {
          tos=tos->prev;
          i++;
        }
        cos=tos;
        break;

      default:
        ufu_wrong_key(env);
        break;

    }

  }

  // Remove list.
  sprintf(env->msg,"Removing splitted entries.");
  ufu_log(env);
  split1=env->sfirst;
  while(split1!=NULL) {
    sprintf(env->msg," Removing entry #%d: %s.",split1->seqno,split1->name);
    ufu_log(env);
    split2=split1->next;
    ufu_free_split(env,split1);
    split1=split2;
  }
  env->sfirst=NULL;
  sprintf(env->msg,"Finished removing splitted entries.");
  ufu_log(env);

  ufu_free_string(env,s4);

}

struct s_entry *ufu_show_entries(struct s_env *env,int panel,int key,int redraw) {

  int hdrdisplay,row,rows,width,i;
  int tmp_rows,tmp_cols;
  char sortc,sorto;
  char *uname,*gname,*s1,*s2,*s3,*s4;
  struct s_entry *e;

  strcpy(env->fun,"ufu_show_entries");
  ufu_wai(env);

  s1=ufu_alloc_string(env,UFU_LEN_NAME);
  s2=ufu_alloc_string(env,UFU_LEN_NAME);
  s3=ufu_alloc_string(env,UFU_LEN_NAME);
  s4=ufu_alloc_string(env,UFU_LEN_NAME);

  if(env->panel[panel]->remote) {
    uname=ufu_alloc_string(env,UFU_LEN_UNAME);
    gname=ufu_alloc_string(env,UFU_LEN_GROUP);
  }

  width=env->cols;

  hdrdisplay=ufu_display_header(env);

  if(redraw) {

    if(env->panel[panel]->remote)
      sprintf(s2,"[%s!%s] Panel #%d -",env->panel[panel]->rem->hostname,env->panel[panel]->rem->username,panel); 
    else
      sprintf(s2,"[%s] Panel #%d -",env->panel[panel]->nodename,panel); 
    sprintf(s3,"(%d/%d/%d)",env->panel[panel]->nfiles,env->panel[panel]->ndirs,env->panel[panel]->nlinks);
    ufu_resize_string(env,env->panel[panel]->dirname,env->cols-strlen(s2)-strlen(s3)-5,s4);

    ufu_wclear(env->top);
    mvwprintw(env->top,0,0,"%s %s %s",s2,s4,s3);

    if(env->panel[panel]->sortc==UFU_SORT_FNAME)  sortc='F';
    if(env->panel[panel]->sortc==UFU_SORT_SIZE)   sortc='S';
    if(env->panel[panel]->sortc==UFU_SORT_LA)     sortc='A';
    if(env->panel[panel]->sortc==UFU_SORT_LM)     sortc='M';
    if(env->panel[panel]->sortc==UFU_SORT_LC)     sortc='C';
        
    if(env->panel[panel]->sorto==UFU_SORT_ASC)    sorto='A';
    if(env->panel[panel]->sorto==UFU_SORT_DSC)    sorto='D';

    mvwprintw(env->top,0,width-2,"%c%c",sortc,sorto);

    ufu_wrefresh(env->top);

    ufu_wclear(env->bottom);
    ufu_wrefresh(env->bottom);

    ufu_wclear(env->body);

    mvwprintw(env->body,0,1,"SeqNo");
    mvwprintw(env->body,0,7,"Name");
    mvwprintw(env->body,0,32+(width-UFU_MIN_COLS),"       Size");
    mvwprintw(env->body,0,44+(width-UFU_MIN_COLS),"Permission");

    mvwprintw(env->body,1,1,"-----");
    for(i=0;i<(24+(width-UFU_MIN_COLS));i++) {
      mvwprintw(env->body,1,7+i,"-");
    }
    mvwprintw(env->body,1,32+(width-UFU_MIN_COLS),"-----------");
    mvwprintw(env->body,1,44+(width-UFU_MIN_COLS),"----------");

    switch(env->panel[panel]->header) {
      case UFU_HDR_STD:
        mvwprintw(env->body,0,55+(width-UFU_MIN_COLS),"User     Group");
        mvwprintw(env->body,1,55+(width-UFU_MIN_COLS),"-------- --------");
        break;
      case UFU_HDR_LA:
        mvwprintw(env->body,0,55+(width-UFU_MIN_COLS),"Last access");
        mvwprintw(env->body,1,55+(width-UFU_MIN_COLS),"------------------------");
        break;
      case UFU_HDR_LM:
        mvwprintw(env->body,0,55+(width-UFU_MIN_COLS),"Last modification");
        mvwprintw(env->body,1,55+(width-UFU_MIN_COLS),"------------------------");
        break;
      case UFU_HDR_LC:
        mvwprintw(env->body,0,55+(width-UFU_MIN_COLS),"Last status change");
        mvwprintw(env->body,1,55+(width-UFU_MIN_COLS),"------------------------");
        break;
    }

    ufu_wrefresh(env->body);

  }

  if(env->panel[panel]->tos!=NULL) {

    e=env->panel[panel]->tos;

  }
  else {

    e=env->panel[panel]->first;

    env->panel[panel]->tos=e;
    env->panel[panel]->cos=e;

  }

  row=1;
  rows=env->rows-6;
  while(row<=rows) {

    if(e!=NULL) {

      if(e->marked) {
        mvwprintw(env->body,row+1,0,">");
      }
      else {
        mvwprintw(env->body,row+1,0," ");
      }

      if(e==env->panel[panel]->cos)  wattron(env->body,A_REVERSE);

      if(e->status_action!=UFU_MARK_ACTION_NONE) {
        switch(e->status_action) {
          case UFU_MARK_ACTION_REMOVE:
            mvwprintw(env->body,row+1,1,"-> rm");
            break;
          case UFU_MARK_ACTION_COPY:
            mvwprintw(env->body,row+1,1,"-> cp");
            break;
          case UFU_MARK_ACTION_MOVE:
            mvwprintw(env->body,row+1,1,"-> mv");
            break;
        }
      }
      else {
        mvwprintw(env->body,row+1,1,"%5d",e->seqno);
      }

      if(strcmp(e->fname,".")==0) {
        strcpy(s1,UFU_THIS_DIRECTORY);
        mvwprintw(env->body,row+1,7,"%-24s",s1);
        for(i=strlen(s1);i<(24+(width-UFU_MIN_COLS));i++) {
          mvwprintw(env->body,row+1,i+7," ");
        }
      }
      else {
        if(strcmp(e->fname,"..")==0) {
          strcpy(s1,UFU_UP_DIRECTORY);
          mvwprintw(env->body,row+1,7,"%-24s",s1);
          for(i=strlen(s1);i<(24+(width-UFU_MIN_COLS));i++) {
            mvwprintw(env->body,row+1,i+7," ");
          }
        }
        else {

          ufu_resize_string(env,e->fname,24+(width-UFU_MIN_COLS),s1);
          mvwprintw(env->body,row+1,7,"%s",s1);
          for(i=strlen(s1);i<(24+(width-UFU_MIN_COLS));i++) {
            mvwprintw(env->body,row+1,i+7," ");
          }

        }
      }

/*
      switch(e->status_action) {
        case UFU_MARK_ACTION_NONE:
          break;
        case UFU_MARK_ACTION_REMOVE:
          mvwprintw(env->body,row+1,6,"r");
          break;
        case UFU_MARK_ACTION_COPY:
          mvwprintw(env->body,row+1,6,"c");
          break;
        case UFU_MARK_ACTION_MOVE:
          mvwprintw(env->body,row+1,6,"m");
          break;
      }
*/

      if(!(env->panel[panel]->remote)) {
        if(S_ISDIR(e->lstat->st_mode)||S_ISDIR(e->fstat->st_mode))
          mvwprintw(env->body,row+1,32+(width-UFU_MIN_COLS),"[   DIR   ]");
        else 
          mvwprintw(env->body,row+1,32+(width-UFU_MIN_COLS),"%11ld",e->fstat->st_size);
        if(S_ISLNK(e->lstat->st_mode))
          mvwprintw(env->body,row+1,44+(width-UFU_MIN_COLS),"%10s",e->lperm);
        else
          mvwprintw(env->body,row+1,44+(width-UFU_MIN_COLS),"%10s",e->fperm);
      }
      else {
        // Remote entry.
        if(e->is_dir)
          mvwprintw(env->body,row+1,32+(width-UFU_MIN_COLS),"[   DIR   ]");
        else 
          mvwprintw(env->body,row+1,32+(width-UFU_MIN_COLS),"%11ld",e->size);
        if(e->is_lnk)
          mvwprintw(env->body,row+1,44+(width-UFU_MIN_COLS),"%10s",e->lperm);
        else
          mvwprintw(env->body,row+1,44+(width-UFU_MIN_COLS),"%10s",e->fperm);
      }

      switch(env->panel[panel]->header) {
        case UFU_HDR_STD:
          if(!(env->panel[panel]->remote)) {
            uname=ufu_search_user(env,e->fstat->st_uid);
            gname=ufu_search_group(env,e->fstat->st_gid);
          }
          else {
            strcpy(uname,e->fusrname);
            strcpy(gname,e->fgrpname);
          }
          mvwprintw(env->body,row+1,55+(width-UFU_MIN_COLS),"%-8s",uname);
          mvwprintw(env->body,row+1,64+(width-UFU_MIN_COLS),"%-8s",gname);
          break;
        case UFU_HDR_LA:
          if(!(env->panel[panel]->remote))
            ufu_sdate(env,&e->fstat->st_atime);
          else
            ufu_sdate(env,&e->fatime);
          mvwprintw(env->body,row+1,55+(width-UFU_MIN_COLS),"%24s",env->msg);
          break;
        case UFU_HDR_LM:
          if(!(env->panel[panel]->remote))
            ufu_sdate(env,&e->fstat->st_mtime);
          else
            ufu_sdate(env,&e->fmtime);
          mvwprintw(env->body,row+1,55+(width-UFU_MIN_COLS),"%24s",env->msg);
          break;
        case UFU_HDR_LC:
          if(!(env->panel[panel]->remote))
            ufu_sdate(env,&e->fstat->st_ctime);
          else
            ufu_sdate(env,&e->fctime);
          mvwprintw(env->body,row+1,55+(width-UFU_MIN_COLS),"%24s",env->msg);
          break;
      }

      if(e==env->panel[panel]->cos)  wattroff(env->body,A_REVERSE);
      e=e->next;
    }

    row++;

    tmp_rows=ufu_get_rows(env);
    tmp_cols=ufu_get_cols(env);
    if((env->rows!=tmp_rows)||(env->cols!=tmp_cols)) {
      sprintf(env->msg,"Resizing screen - old col/row: %d/%d, new col/row: %d/%d",env->cols,env->rows,tmp_cols,tmp_rows);
      ufu_log(env);
      env->rows=tmp_rows;
      env->cols=tmp_cols;
      ufu_deinit_curses(env);
      ufu_init_curses(env);
      ufu_clear(env);
    }

  }

  ufu_wrefresh(env->body);

  ufu_free_string(env,s1);
  ufu_free_string(env,s2);
  ufu_free_string(env,s3);
  ufu_free_string(env,s4);

  if(env->panel[panel]->remote) {
    ufu_free_string(env,uname);
    ufu_free_string(env,gname);
  }

  return(env->panel[panel]->cos);

}

void ufu_size_entry(struct s_env *env,int panel,struct s_entry *e) {


  strcpy(env->fun,"ufu_size_entry");
  ufu_wai(env);










}

