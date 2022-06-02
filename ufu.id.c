
#include "ufu.h"
#include "ufu.p"

void ufu_read_users(struct s_env *env) {

  int c,seqno;
  char *line1,*line2;
  struct s_usr *u,*tmp_u;
  struct passwd *pwd;
  FILE *fp;

  strcpy(env->fun,"ufu_read_users");
  ufu_wai(env);

  if((fp=fopen(UFU_USR_FILE,"r"))!=NULL) {

    sprintf(env->msg,"Scanning users (%s)",UFU_USR_FILE);
    ufu_log(env);

    line1=ufu_alloc_string(env,256);

    u=NULL;
    tmp_u=NULL;

    seqno=0;
    c=0;

    while(fgets(line1,256,fp)!=NULL) {

      c++;

      tmp_u=u;
      u=ufu_alloc_usr(env);

      line2=strtok(line1,":");

      strcpy(u->uname,line2);
      pwd=getpwnam(u->uname);
      if(pwd==NULL) {
        sprintf(env->msg,"Unable to read entry for user %s!",u->uname);
        ufu_log(env);
        exit(1);
      }

      u->seqno=seqno++;
      u->uid=pwd->pw_uid;
      u->gid=pwd->pw_gid;
      strcpy(u->pword,pwd->pw_passwd);
      strcpy(u->dir,pwd->pw_dir);
      strcpy(u->sh,pwd->pw_shell);

      sprintf(env->msg," Found user \"%s\" (uid %d,gid %d)",u->uname,u->uid,u->gid);
      ufu_log(env);

      if(tmp_u!=NULL) {
        tmp_u->next=u;
        u->prev=tmp_u;
        tmp_u=u;
      }
      else {
        env->ufirst=u;
        u->prev=NULL;
        u->next=NULL;
      }

    }

    fclose(fp);

    sprintf(env->msg,"Read %d users.",c);
    ufu_log(env);

    ufu_free_string(env,line1);

  }
  else {

    sprintf(env->msg,"Unable to open file %s!",UFU_USR_FILE);
    ufu_log(env);

    exit(1);

  }

}

void ufu_read_groups(struct s_env *env) {

  int c,i,seqno,seqno_gu;
  char *line1,*line2,*line3;
  struct s_grp *g,*tmp_g;
  struct s_grpusr *gu,*tmp_gu;
  struct group *grp;
  FILE *fp;

  strcpy(env->fun,"ufu_read_groups");
  ufu_wai(env);

  if((fp=fopen(UFU_GRP_FILE,"r"))!=NULL) {

    sprintf(env->msg,"Scanning groups (%s)",UFU_GRP_FILE);
    ufu_log(env);

    line1=ufu_alloc_string(env,256);

    g=NULL;
    tmp_g=NULL;

    seqno=0;
    c=0;

    while(fgets(line1,256,fp)!=NULL) {

      c++;

      tmp_g=g;
      g=ufu_alloc_grp(env);

      line2=strtok(line1,":");

      strcpy(g->gname,line2);
      grp=getgrnam(g->gname);
      if(grp==NULL) {
        sprintf(env->msg,"Unable to read entry for group %s!",g->gname);
        ufu_log(env);
        exit(1);
      }

      g->seqno=seqno++;
      g->gid=grp->gr_gid;

      sprintf(env->msg," Found group \"%s\" (gid %d)",g->gname,g->gid);
      ufu_log(env);

      line2=strtok(NULL,":");
      line2=strtok(NULL,":");
      line2=strtok(NULL,":");

      if(line2!=NULL) {

        line3=strtok(line2,",");
	seqno_gu=0;
	while((line3!=NULL)&&(strlen(line3)>1)) {
          // Strip NL.
	  for(i=0;i<strlen(line3);i++) {
            if(line3[i]==0x0a) {
              line3[i]='\0';
	    }
	  }
	  gu=ufu_alloc_grpusr(env);
	  gu->seqno=seqno_gu;
          strcpy(gu->uname,line3);
          if(seqno_gu>0) {
            gu->next=NULL;
	    gu->prev=tmp_gu;
	    tmp_gu->next=gu;
          }
	  else {
	    gu->next=NULL;
            gu->prev=NULL;
            g->fgrpusr=gu;
	  }
          sprintf(env->msg,"  Group \"%s\" is a secondary group for user \"%s\"",g->gname,gu->uname);
          ufu_log(env);
          seqno_gu++;
	  tmp_gu=gu;
          line3=strtok(NULL,",");
	}

      }

      if(tmp_g!=NULL) {
        tmp_g->next=g;
        g->prev=tmp_g;
        tmp_g=g;
      }
      else {
        env->gfirst=g;
        g->prev=NULL;
        g->next=NULL;
      }

    }

    fclose(fp);

    sprintf(env->msg,"Read %d groups.",c);
    ufu_log(env);

    ufu_free_string(env,line1);

  }
  else {

    sprintf(env->msg,"Unable to open file %s!",UFU_GRP_FILE);
    ufu_log(env);

    exit(1);

  }

}

void ufu_free_users(struct s_env *env) {

  int c;
  struct s_usr *u,*tmp_u;

  strcpy(env->fun,"ufu_free_users");
  ufu_wai(env);

  sprintf(env->msg," Releasing users");
  ufu_log(env);

  c=0;

  u=env->ufirst;

  while(u!=NULL) {

    tmp_u=u->next;

    c++;

    if(env->debug) {
      sprintf(env->msg,"  Releasing user \"%s\" (uid %d,gid %d)",u->uname,u->uid,u->gid);
    }

    ufu_free_usr(env,u);
    u=tmp_u;

  }

  env->ufirst=NULL;

  sprintf(env->msg," Released %d users.",c);
  ufu_log(env);

}

void ufu_free_groups(struct s_env *env) {

  int c;
  struct s_grp *g,*tmp_g;
  struct s_grp *gu,*tmp_gu;

  strcpy(env->fun,"ufu_free_groups");
  ufu_wai(env);

  sprintf(env->msg," Releasing groups");
  ufu_log(env);

  c=0;

  g=env->gfirst;

  while(g!=NULL) {

    tmp_g=g->next;

    c++;

    if(env->debug) {
      sprintf(env->msg,"  Releasing group \"%s\" (gid %d)",g->gname,g->gid);
    }

    ufu_free_grp(env,g);
    g=tmp_g;

  }

  env->gfirst=NULL;

  sprintf(env->msg," Released %d groups.",c);
  ufu_log(env);

}

char *ufu_search_user(struct s_env *env,int uid) {

  struct s_usr *u;

  strcpy(env->fun,"ufu_search_user");
  ufu_wai(env);

  u=env->ufirst;
  while(u!=NULL) {
    if(uid==u->uid) {
      if(env->debug) {
        sprintf(env->msg,"Found username \"%s\" for uid %d.",u->uname,uid);
      }
      return(u->uname);
    }
    u=u->next;
  }

  sprintf(env->msg," No match for uid %d!",uid);
  ufu_log(env);

  return(NULL);

}

int ufu_search_user_gid(struct s_env *env,int uid) {

  struct s_usr *u;

  strcpy(env->fun,"ufu_search_user_gid");
  ufu_wai(env);

  u=env->ufirst;
  while(u!=NULL) {
    if(uid==u->uid) {
      if(env->debug) {
        sprintf(env->msg,"Found gid %d for uid %d.",u->gid,uid);
      }
      return(u->gid);
    }
    u=u->next;
  }

  sprintf(env->msg," No match for gid %d!",u->gid);
  ufu_log(env);

  return(-1);

}

char *ufu_search_group(struct s_env *env,int gid) {

  struct s_grp *g;

  strcpy(env->fun,"ufu_search_group");
  ufu_wai(env);

  g=env->gfirst;
  while(g!=NULL) {
    if(gid==g->gid) {
      if(env->debug) {
        sprintf(env->msg,"Found groupname \"%s\" for gid %d.",g->gname,gid);
      }
      return(g->gname);
    }
    g=g->next;
  }

  sprintf(env->msg," No match for gid %d!",gid);
  ufu_log(env);

  return(NULL);

}

int ufu_search_uid(struct s_env *env,char *uname) {

  struct s_usr *u;

  strcpy(env->fun,"ufu_search_uid");
  ufu_wai(env);

  u=env->ufirst;
  while(u!=NULL) {
    if(strcmp(uname,u->uname)==0) {
      if(env->debug) {
        sprintf(env->msg,"Found uid %d for username \"%s\".",u->uid,uname);
      }
      return(u->uid);
    }
    u=u->next;
  }

  sprintf(env->msg," No match for username \"%s\"!",uname);
  ufu_log(env);

  return(-1);

}

int ufu_search_gid(struct s_env *env,char *gname) {

  struct s_grp *g;

  strcpy(env->fun,"ufu_search_gid");
  ufu_wai(env);

  g=env->gfirst;
  while(g!=NULL) {
    if(strcmp(gname,g->gname)==0) {
      if(env->debug) {
        sprintf(env->msg,"Found gid %d for groupname \"%s\".",g->gid,gname);
      }
      return(g->gid);
    }
    g=g->next;
  }

  sprintf(env->msg," No match for groupname \"%s\"!",gname);
  ufu_log(env);

  return(-1);

}
