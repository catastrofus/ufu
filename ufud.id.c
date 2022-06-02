
#include "ufud.h"
#include "ufud.p"

void ufud_read_users(struct s_env *env) {

  int c,s,loop,shadow;
  char *line1,*line2,*line3,*line6,*line7,*line8;
  struct s_usr *u,*tmp_u;
  struct passwd *pwd;
  FILE *fp,*fp_shadow;

  if((fp=fopen(UFUD_USR_FILE,"r"))!=NULL) {

    sprintf(env->msg,"Scanning users (%s)",UFUD_USR_FILE);
    ufud_log(env);

    // Password file.
    line1=ufud_alloc_string(env,256);

    // Shadow file, optional.
    line6=ufud_alloc_string(env,256);

    u=NULL;
    tmp_u=NULL;

    c=0;
    shadow=0;

    while(fgets(line1,256,fp)!=NULL) {

      c++;

      tmp_u=u;
      u=ufud_alloc_usr(env);

      line2=strtok(line1,":");

      strcpy(u->uname,line2);
      pwd=getpwnam(u->uname);
      if(pwd==NULL) {
        sprintf(env->msg,"Unable to read entry for user %s!",u->uname);
        ufud_log(env);
        exit(1);
      }

      u->uid=pwd->pw_uid;
      u->gid=pwd->pw_gid;
      strcpy(u->pword,pwd->pw_passwd);
      strcpy(u->dir,pwd->pw_dir);
      strcpy(u->sh,pwd->pw_shell);

      sprintf(env->msg," Found user \"%s\" (uid %d,gid %d)",u->uname,u->uid,u->gid);
      ufud_log(env);

      if(strcmp(u->pword,"x")!=0) {
        // No shadow file.
        // Password is the second field of the password file.
        strcpy(u->pword,line3);
      }
      else {
        // Shadow file present.
        if((fp_shadow=fopen(UFUD_USR_FILE_SHADOW,"r"))!=NULL) {
          strcpy(u->pword,"");
          loop=TRUE;
          s=0;
          while((fgets(line6,256,fp_shadow)!=NULL)&&(loop)) {
            s++;
            line7=strtok(line6,":");
            line8=strtok(NULL,":");
            if(strcmp(u->uname,line7)==0) {
              strcpy(u->pword,line8);
              loop=FALSE;
            }
          }
          fclose(fp_shadow);
          if(loop) {
            sprintf(env->msg,"  Unable to find password for user \"%s\"!",u->uname);
            ufud_log(env);
            sprintf(env->msg,"  User \"%s\" is not useable!",u->uname);
            ufud_log(env);
          }
        }
        else {
          if(shadow==0) {
            sprintf(env->msg,"Unable to open file \"%s\"!",UFUD_USR_FILE_SHADOW);
            ufud_log(env);
            sprintf(env->msg,"This message will appear just this time!");
            ufud_log(env);
          }
          shadow++;
        }
      }

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
    ufud_log(env);

    ufud_free_string(env,line1);
    ufud_free_string(env,line6);

  }
  else {

    sprintf(env->msg,"Unable to open file \"%s\"!",UFUD_USR_FILE);
    ufud_log(env);

    exit(1);

  }

}

void ufud_read_groups(struct s_env *env) {

  int c;
  char *line1,*line2;
  struct s_grp *g,*tmp_g;
  struct group *grp;
  FILE *fp;

  if((fp=fopen(UFUD_GRP_FILE,"r"))!=NULL) {

    sprintf(env->msg,"Scanning groups (%s)",UFUD_GRP_FILE);
    ufud_log(env);

    line1=ufud_alloc_string(env,256);

    g=NULL;
    tmp_g=NULL;

    c=0;

    while(fgets(line1,256,fp)!=NULL) {

      c++;

      tmp_g=g;
      g=ufud_alloc_grp(env);

      line2=strtok(line1,":");

      strcpy(g->gname,line2);
      grp=getgrnam(g->gname);
      if(grp==NULL) {
        sprintf(env->msg,"Unable to read entry for group %s!",g->gname);
        ufud_log(env);
        exit(1);
      }

      g->gid=grp->gr_gid;

      sprintf(env->msg," Found group \"%s\" (gid %d)",g->gname,g->gid);
      ufud_log(env);

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
    ufud_log(env);

    ufud_free_string(env,line1);

  }
  else {

    sprintf(env->msg,"Unable to open file %s!",UFUD_GRP_FILE);
    ufud_log(env);

    exit(1);

  }

}

void ufud_free_users(struct s_env *env) {

  int c;
  struct s_usr *u,*tmp_u;

  sprintf(env->msg," Releasing users");
  ufud_log(env);

  c=0;

  u=env->ufirst;

  while(u!=NULL) {

    tmp_u=u->next;

    c++;

    if(env->debug) {
      sprintf(env->msg,"  Releasing user \"%s\" (uid %d,gid %d)",u->uname,u->uid,u->gid);
      ufud_log(env);
    }

    ufud_free_usr(env,u);
    u=tmp_u;

  }

  env->ufirst=NULL;

  sprintf(env->msg," Released %d users.",c);
  ufud_log(env);

}

void ufud_free_groups(struct s_env *env) {

  int c;
  struct s_grp *g,*tmp_g;

  sprintf(env->msg," Releasing groups");
  ufud_log(env);

  c=0;

  g=env->gfirst;

  while(g!=NULL) {

    tmp_g=g->next;

    c++;

    if(env->debug) {
      sprintf(env->msg,"  Releasing group \"%s\" (gid %d)",g->gname,g->gid);
      ufud_log(env);
    }

    ufud_free_grp(env,g);
    g=tmp_g;

  }

  env->gfirst=NULL;

  sprintf(env->msg," Released %d groups.",c);
  ufud_log(env);

}

int ufud_check_user(struct s_env *env,struct s_buffer *b,char *ipno) {

  int i,found,auth;
  char c,*password,*encoded,*salt,*salt1,*salt2;
  struct s_usr *u;

  auth=FALSE;
  found=FALSE;

  password=ufud_alloc_string(env,UFUD_LEN_PWORD);
  salt=ufud_alloc_string(env,35);

  u=env->ufirst;
  while(u!=NULL) {
    if(strcmp(b->username,u->uname)==0) {
      found=TRUE;
      sprintf(env->msg,"[%s] Found user \"%s\" (%d/%d) in list.",ipno,b->username,u->uid,u->gid);
      ufud_log(env);
      env->uid=u->uid;
      env->gid=u->gid;
      break;
    }
    u=u->next;
  }

  sprintf(env->msg,"[%s] Unscrambling password.",ipno);
  ufud_log(env);

  if(found) {

    for(i=0;i<strlen(b->password);i++) {

      c=b->password[i];

      if((i%2)>0) {
        c=c-128+strlen(b->username);
      }
      else {
        c=256-c+strlen(b->username);
      }

      b->password[i]=c;

    }

    // Save original encrypted password for this user.
    strcpy(password,u->pword);

    salt1=strtok(u->pword,"$");
    salt2=strtok(NULL,"$");

    sprintf(salt,"$%s$%s",salt1,salt2);
    encoded=crypt(b->password,salt);

    auth=(strcmp(encoded,password)==0);

    if(auth) {

      sprintf(env->msg,"[%s] User \"%s\" is authorized by MD5 encryption.",ipno,b->username);
      ufud_log(env);

    }
    else {

      sprintf(env->msg,"[%s] User \"%s\" is NOT authorized by MD5 encryption!",ipno,b->username);
      ufud_log(env);

      strcpy(salt,password);
      // The first 2 characters of the encrypted password is the salt.
      salt[2]='\0';

      encoded=crypt(b->password,salt);

      auth=(strcmp(encoded,password)==0);

      if(auth) {

        sprintf(env->msg,"[%s] User \"%s\" is authorized by DES encryption.",ipno,b->username);
        ufud_log(env);

      }
      else {

        sprintf(env->msg,"User \"%s\" is NOT authorized by DES encryption!",b->username);
        ufud_log(env);

        sprintf(env->msg,"User \"%s\" is NOT authorized at all!",b->username);
        ufud_log(env);

        sprintf(b->errmsg,"User \"%s\" is NOT authorized!",b->username);

      }

    }

    // Restore original encrypted password for this user.
    strcpy(u->pword,password);

  }
  else {
    sprintf(env->msg,"[%s] User \"%s\" not found in list of users!",ipno,b->username);
    ufud_log(env);
  }

  ufud_free_string(env,salt);
  ufud_free_string(env,password);

  return(auth);

}

char *ufud_search_user(struct s_env *env,int uid,char *ipno) {

  struct s_usr *u;

  u=env->ufirst;
  while(u!=NULL) {
    if(uid==u->uid) {
//      if(env->debug) {
//        sprintf(env->msg,"[%s] Found user %s for uid %d.",ipno,u->uname,uid);
//        ufud_log(env);
//      }
      return(u->uname);
    }
    u=u->next;
  }

  sprintf(env->msg,"[%s] No match for uid %d!",ipno,uid);
  ufud_log(env);

  return("????????");

}

char *ufud_search_group(struct s_env *env,int gid,char *ipno) {

  struct s_grp *g;

  g=env->gfirst;
  while(g!=NULL) {
    if(gid==g->gid) {
//      if(env->debug) {
//        sprintf(env->msg,"[%s] Found group %s for gid %d.",ipno,g->gname,gid);
//        ufud_log(env);
//      }
      return(g->gname);
    }
    g=g->next;
  }

  sprintf(env->msg,"[%s] No match for gid %d!",ipno,gid);
  ufud_log(env);

  return("????????");

}

