
#include "ufud.h"
#include "ufud.p"

int ufud_get_txtfile(struct s_env *env,struct s_buffer *b,int fd,char *ipno) {

  int i;
  char *s,*f;
  FILE *fp;

  f=ufud_alloc_string(env,UFUD_LEN_NAME);
  ufud_concat_dir(env,f,b->dirname,b->filename);

  fp=fopen(f,"r");
  if(fp!=NULL) {

    sprintf(env->msg,"[%s] File \"%s\" opened.",ipno,f);
    ufud_log(env);

    b->last=FALSE;
    b->error=FALSE;

    i=0;

    ufud_clear_string(env,b->line,UFUD_LEN_MSG);
    while(fgets(b->line,UFUD_LEN_MSG,fp)!=NULL) {

      ufud_send(env,b,fd,ipno);

      ufud_clear_string(env,b->line,UFUD_LEN_MSG);

      i++;

    }

    sprintf(env->msg,"[%s] %d lines send.",ipno,i);
    ufud_log(env);

    fclose(fp);

    sprintf(env->msg,"[%s] File \"%s\" closed.",ipno,f);
    ufud_log(env);

    b->last=TRUE;
    ufud_clear_string(env,b->line,UFUD_LEN_MSG);

    ufud_send(env,b,fd,ipno);

  }
  else {

    b->last=TRUE;
    b->error=TRUE;

    s=ufud_alloc_string(env,UFUD_LEN_NAME);
    ufud_resize_string(env,b->filename,36,s);

    sprintf(b->errmsg,"No permission to read file %s!",s);

    ufud_send(env,b,fd,ipno);

    ufud_free_string(env,s);

  }

  ufud_free_string(env,f);

  return(b->error);

}

int ufud_put_txtfile(struct s_env *env,struct s_buffer *b,int fd,char *ipno) {

  int i;
  char *s,*f;
  FILE *fp;

  f=ufud_alloc_string(env,UFUD_LEN_NAME);
  ufud_concat_dir(env,f,b->dirname,b->filename);

  fp=fopen(f,"w");
  if(fp!=NULL) {

    b->last=FALSE;
    b->error=FALSE;

    sprintf(b->errmsg,"Ok, file \"%s\" is writable.",s);

    ufud_send(env,b,fd,ipno);

    sprintf(env->msg,"[%s] File \"%s\" opened for writing.",ipno,f);
    ufud_log(env);

    b->last=FALSE;
    b->error=FALSE;

    i=0;

    ufud_clear_string(env,b->line,UFUD_LEN_MSG);
    ufud_recv(env,b,fd,ipno);
    while(!b->last) {
      if(!b->error) {
        fputs(b->line,fp);
        i++;
        ufud_clear_string(env,b->line,UFUD_LEN_MSG);
        ufud_recv(env,b,fd,ipno);
      }
      else {
        b->last=TRUE;
      }
    }

    sprintf(env->msg,"[%s] %d lines received.",ipno,i);
    ufud_log(env);

    fclose(fp);

    sprintf(env->msg,"[%s] File \"%s\" closed.",ipno,f);
    ufud_log(env);

    b->last=TRUE;
    ufud_clear_string(env,b->line,UFUD_LEN_MSG);

    ufud_send(env,b,fd,ipno);

  }
  else {

    b->last=TRUE;
    b->error=TRUE;

    s=ufud_alloc_string(env,UFUD_LEN_NAME);
    ufud_resize_string(env,b->filename,36,s);

    sprintf(b->errmsg,"No permission to write file %s!",s);

    ufud_send(env,b,fd,ipno);

    ufud_free_string(env,s);

  }

  ufud_free_string(env,f);

  return(b->error);

}

int ufud_unlink_entry(struct s_env *env,struct s_buffer *b,int fd,char *ipno) {

  int r;
  char *f;

  f=ufud_alloc_string(env,UFUD_LEN_NAME);

  if(strlen(b->filename)>0) {
    ufud_concat_dir(env,f,b->dirname,b->filename);
    sprintf(env->msg,"[%s] Entry \"%s\" is a file.",ipno,f);
  }
  else {
    strcpy(f,b->dirname);
    sprintf(env->msg,"[%s] Entry \"%s\" is a directory.",ipno,f);
  }
  ufud_log(env);

  b->last=TRUE;

  // r=unlink(f);
  r=0;  // Temporary, for testing.

  if(r<0) {

    sprintf(env->msg,"[%s] Unable to unlink entry  \"%s\".",ipno,f);
    ufud_log(env);

    b->error=TRUE;
    sprintf(b->errmsg,"Unable to unlink entry \"%s\".",f);

    ufud_send(env,b,fd,ipno);

  }
  else {

    b->error=FALSE;

    sprintf(env->msg,"[%s] Entry \"%s\" unlinked.",ipno,f);
    ufud_log(env);

    ufud_send(env,b,fd,ipno);

  }

  ufud_free_string(env,f);

  return(b->error);

}

int ufud_get_binfile(struct s_env *env,struct s_buffer *b,int fd,char *ipno) {

  int i;
  char *s,*f;
  struct s_buffer_action *ba;
  FILE *fp;

  ba=ufud_alloc_buffer_action(env,ipno);

  f=ufud_alloc_string(env,UFUD_LEN_NAME);
  ufud_concat_dir(env,f,b->dirname,b->filename);

  fp=fopen(f,"r");
  if(fp!=NULL) {

    sprintf(env->msg,"[%s] File \"%s\" opened.",ipno,f);
    ufud_log(env);

    b->last=FALSE;
    b->error=FALSE;

    i=0;

    ufud_clear_string(env,ba->line,UFUD_ACTION_BUFFER);
    while(fgets(ba->line,UFUD_ACTION_BUFFER,fp)!=NULL) {

      ufud_send_action(env,ba,fd,ipno);

      ufud_clear_string(env,ba->line,UFUD_ACTION_BUFFER);

      i++;

    }

    sprintf(env->msg,"[%s] %ld bytes send.",ipno,(long)i*UFUD_ACTION_BUFFER);
    ufud_log(env);

    fclose(fp);

    sprintf(env->msg,"[%s] File \"%s\" closed.",ipno,f);
    ufud_log(env);

    ba->last=TRUE;
    ufud_clear_string(env,ba->line,UFUD_ACTION_BUFFER);

    ufud_send_action(env,ba,fd,ipno);

  }
  else {

    ba->last=TRUE;
    ba->error=TRUE;

    s=ufud_alloc_string(env,UFUD_LEN_NAME);
    ufud_resize_string(env,ba->filename,36,s);

    sprintf(ba->errmsg,"No permission to read file %s!",s);

    ufud_send_action(env,ba,fd,ipno);

    ufud_free_string(env,s);

  }

  ufud_free_string(env,f);
  ufud_free_buffer_action(env,ba,ipno);

  return(ba->error);

}

int ufud_put_binfile(struct s_env *env,struct s_buffer *b,int fd,char *ipno) {

  int i;
  char *s,*f;
  struct s_buffer_action *ba;
  FILE *fp;

  ba=ufud_alloc_buffer_action(env,ipno);

  f=ufud_alloc_string(env,UFUD_LEN_NAME);
  ufud_concat_dir(env,f,b->dirname,b->filename);

  fp=fopen(f,"w");
  if(fp!=NULL) {

    ba->last=FALSE;
    ba->error=FALSE;

    sprintf(ba->errmsg,"Ok, file \"%s\" is writable.",s);

    ufud_send_action(env,ba,fd,ipno);

    sprintf(env->msg,"[%s] File \"%s\" opened for writing.",ipno,f);
    ufud_log(env);

    ba->last=FALSE;
    ba->error=FALSE;

    i=0;

    ufud_clear_string(env,ba->line,UFUD_ACTION_BUFFER);
    ufud_recv_action(env,ba,fd,ipno);
    while(!ba->last) {
      if(!ba->error) {
        fputs(ba->line,fp);
        i++;
        ufud_clear_string(env,ba->line,UFUD_ACTION_BUFFER);
        ufud_recv_action(env,ba,fd,ipno);
      }
      else {
        ba->last=TRUE;
      }
    }

    sprintf(env->msg,"[%s] %ld bytes received.",ipno,(long)i*UFUD_ACTION_BUFFER);
    ufud_log(env);

    fclose(fp);

    sprintf(env->msg,"[%s] File \"%s\" closed.",ipno,f);
    ufud_log(env);

    ba->last=TRUE;
    ufud_clear_string(env,ba->line,UFUD_ACTION_BUFFER);

    ufud_send_action(env,ba,fd,ipno);

  }
  else {

    ba->last=TRUE;
    ba->error=TRUE;

    s=ufud_alloc_string(env,UFUD_LEN_NAME);
    ufud_resize_string(env,ba->filename,36,s);

    sprintf(ba->errmsg,"No permission to write file %s!",s);

    ufud_send_action(env,ba,fd,ipno);

    ufud_free_string(env,s);

  }

  ufud_free_string(env,f);
  ufud_free_buffer_action(env,ba,ipno);

  return(ba->error);

}

