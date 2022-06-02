
#include "ufud.h"
#include "ufud.p"

void ufud_cdate(struct s_env *env) {

  time_t result;

  result=time(NULL);
  strcpy(env->msg,asctime(localtime(&result)));
  env->msg[strlen(env->msg)-1]='\0';

}

void ufud_sdate(struct s_env *env,time_t *timestamp) {

  strcpy(env->msg,ctime(timestamp));
  env->msg[strlen(env->msg)-1]='\0';

}

char *ufud_concat_dir(struct s_env *env,char *name,char *dname,char *fname) {

  if(name!=NULL) {
    strcpy(name,dname);
    if(strlen(name)>1) {
      strcat(name,"/");
    }
    strcat(name,fname);
  }

  return(name);

}

char *ufud_clear_string(struct s_env *env,char *str,int len) {

  int i;

  // Init parameter 'str' with \0.
  for(i=0;i<len;i++) {
    str[i]='\0';
  }

  return(str);

}

int ufud_setuid(struct s_env *env,int uid,char *ipno) {

  int ok,e;

  sprintf(env->msg,"[%s] Setting uid to %d.",ipno,uid);
  ufud_log(env);

  ok=(seteuid(env->uid)==0);
  e=errno;

  if(!ok) {

    switch(e) {

      case EINVAL:
        sprintf(env->msg,"[%s] Not supported by the implementation",ipno);
        break;

      case EPERM:
        sprintf(env->msg,"No appropriate privileges");
        break;

    }
    ufud_log(env);

  }

  return(ok);

}

int ufud_setgid(struct s_env *env,int gid,char *ipno) {

  int ok,e;

  sprintf(env->msg,"Setting gid to %d",gid);
  ufud_log(env);

  ok=(setegid(env->gid)==0);
  e=errno;

  if(!ok) {

    switch(e) {

      case EINVAL:
        sprintf(env->msg,"[%s] Not supported by the implementation",ipno);
        break;

      case EPERM:
        sprintf(env->msg,"No appropriate privileges");
        break;

    }
    ufud_log(env);

  }

  return(ok);

}

void ufud_resize_string(struct s_env *env,char *str,int len,char *s) {

  int i,sp;

  sp=20;

  if(strlen(str)>len) {

    ufud_clear_string(env,s,len);

    if(len>sp) {

      for(i=0;i<8;i++) {
        s[i]=str[i];
      }
      strcat(s,"...");
      strcat(s,str+(strlen(str)-len+11));

    }
    else {
      strcpy(s,str+(len-sp));
    }

  }
  else {
    strcpy(s,str);
  }

}

char *ufud_lower(struct s_env *env,char *s) {

  int i;

  i=0;
  while(s[i]!='\0') {
    s[i]=tolower(s[i]);
    i++;
  }

  return(s);

}

