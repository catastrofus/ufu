
#include "ufud.h"
#include "ufud.p"

void ufud_add_ip(struct s_env *env,char *ipno) {

  struct s_ip *ip,*tmp_ip;

  ip=ufud_alloc_ip(env);
  strcpy(ip->ipno,ipno);

  if(env->ipfirst!=NULL) {

    // First ipno exists.

    tmp_ip=env->ipfirst;

    while(tmp_ip->next!=NULL) {
      tmp_ip=tmp_ip->next;
    }

    tmp_ip->next=ip;
    ip->prev=tmp_ip;

  }
  else {

    // First ipno.

    env->ipfirst=ip;

  }

}

int ufud_auth_ip(struct s_env *env,char *ipno) {

  int ok;
  struct s_ip *ip;

  if(env->auth_ip) {
    ok=FALSE;
    ip=env->ipfirst;
    while((ip!=NULL)&&(!ok)) {
      if(strstr(ipno,ip->ipno)) {
        ok=TRUE;
        sprintf(env->msg,"[%s] Found match in mask %s.",ipno,ip->ipno);
        ufud_log(env);
      }
      else {
        if(env->debug) {
          sprintf(env->msg,"[%s] No match in mask %s!",ipno,ip->ipno);
          ufud_log(env);
        }
        ip=ip->next;
      }
    }
  }
  else {
    ok=TRUE;
  }

  return(ok);

}

void ufud_free_ips(struct s_env *env) {

  int c;
  struct s_ip *ip,*tmp_ip;

  sprintf(env->msg," Releasing ip's.");
  ufud_log(env);

  c=0;

  ip=env->ipfirst;

  while(ip!=NULL) {

    tmp_ip=ip->next;

    c++;

    if(env->debug) {
      sprintf(env->msg,"  Releasing ip \"%s\".",ip->ipno);
      ufud_log(env);
    }

    ufud_free_ip(env,ip);
    ip=tmp_ip;

  }

  env->ipfirst=NULL;

  sprintf(env->msg," Released %d ip's.",c);
  ufud_log(env);

}
