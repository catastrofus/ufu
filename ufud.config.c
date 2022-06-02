
#include "ufud.h"
#include "ufud.p"

void ufud_read_config(struct s_env *env) {

  char *line1,*line2;
  FILE *fp;
  // struct s_ip *ip;

  if((fp=fopen(env->cfgfile,"r"))!=NULL) {

    sprintf(env->msg,"Scanning config (%s)",env->cfgfile);
    ufud_log(env);

    line1=ufud_alloc_string(env,128);
    line2=ufud_alloc_string(env,128);

    while(fgets(line1,128,fp)!=NULL) {

      // Ignore comments.
      if(line1[0]!='#') {

        // Extract DEBUG (debugging on/off) from configfile.
        if(strstr(line1,"DEBUG=")) {
          env->debug=(tolower(*(line1+6))=='y');
          if(env->debug)
            sprintf(env->msg," Setting DEBUG to TRUE");
          else
            sprintf(env->msg," Setting DEBUG to FALSE");
          ufud_log(env);
        }

        // Extract OS from configfile.
        if(strstr(line1,"OS=")) {
          strcpy(line2,line1+3);
          line2[strlen(line2)-1]='\0';
          if(strcmp(line2,"LINUX")==0) {
            env->os=UFUD_OS_LINUX;
          }
          else {
            if(strcmp(line2,"HPUX")==0) {
              env->os=UFUD_OS_HPUX;
            }
            else {
              env->os=UFUD_OS_UNKNOWN;
            }
          }
          switch(env->os) {
            case UFUD_OS_LINUX:
              sprintf(env->msg," Found OS (Linux).");
              break;
            case UFUD_OS_HPUX:
              sprintf(env->msg," Found OS (HP-UX).");
              break;
            case UFUD_OS_UNKNOWN:
              sprintf(env->msg," Unknow OS? (%s).",line2);
              break;
          }
          ufud_log(env);
        }

        // Extract PORT from configfile.
        if(strstr(line1,"PORT=")) {
          strcpy(line2,line1+5);
          line2[strlen(line2)-1]='\0';
          env->port=atoi(line2);
          sprintf(env->msg," Found PORT (%d).",env->port);
          ufud_log(env);
        }

        // Extract TIMEOUT from configfile.
        if(strstr(line1,"TIMEOUT=")) {
          strcpy(line2,line1+8);
          line2[strlen(line2)-1]='\0';
          env->timeout=atoi(line2);
          sprintf(env->msg," Found TIMEOUT (%d).",env->timeout);
          ufud_log(env);
        }

        // Extract TIMEOUT_RETRY from configfile.
        if(strstr(line1,"TIMEOUT_RETRY=")) {
          strcpy(line2,line1+14);
          line2[strlen(line2)-1]='\0';
          env->timeout_retry=atoi(line2);
          sprintf(env->msg," Found TIMEOUT_RETRY (%d).",env->timeout_retry);
          ufud_log(env);
        }

        // Extract TIMEIN from configfile.
        if(strstr(line1,"TIMEIN=")) {
          strcpy(line2,line1+7);
          line2[strlen(line2)-1]='\0';
          env->timein=atoi(line2);
          sprintf(env->msg," Found TIMEIN (%d).",env->timein);
          ufud_log(env);
        }

        // Extract TIMEIN_RETRY from configfile.
        if(strstr(line1,"TIMEIN_RETRY=")) {
          strcpy(line2,line1+13);
          line2[strlen(line2)-1]='\0';
          env->timein_retry=atoi(line2);
          sprintf(env->msg," Found TIMEIN_RETRY (%d).",env->timein_retry);
          ufud_log(env);
        }

        // Extract BACKLOG from configfile.
        if(strstr(line1,"BACKLOG=")) {
          strcpy(line2,line1+8);
          line2[strlen(line2)-1]='\0';
          env->backlog=atoi(line2);
          sprintf(env->msg," Found BACKLOG (%d).",env->backlog);
          ufud_log(env);
        }

        // Extract ipno's from configfile.
        if(strstr(line1,"IP=")) {
          strcpy(line2,line1+3);
          line2[strlen(line2)-1]='\0';
          ufud_add_ip(env,line2);
          if(env->debug) {
            sprintf(env->msg," Found IP mask (%s).",line2);
            ufud_log(env);
          }
        }

      }

    }

    fclose(fp);

    sprintf(env->msg,"End of scan.");
    ufud_log(env);

    env->auth_ip=(env->ipfirst!=NULL);

    ufud_free_string(env,line1);
    ufud_free_string(env,line2);

  }
  else {

    sprintf(env->msg,"Configfile \"%s\" not found.",env->cfgfile);
    ufud_log(env);

  }

}

