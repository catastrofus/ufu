
#include "ufud.h"
#include "ufud.p"

void ufud_open_log(struct s_env *env) {

  env->lognamefp=fopen(env->logname,"w");
  if(env->lognamefp!=NULL) {
    // No buffering.
    setbuf(env->lognamefp,NULL);
    sprintf(env->msg,"Log opened for writing.");
    ufud_log(env);
  }

}

void ufud_close_log(struct s_env *env) {

  if(env->lognamefp!=NULL) {
    sprintf(env->msg,"Log closed for writing.");
    ufud_log(env);
    fclose(env->lognamefp);
  }
}

void ufud_log(struct s_env *env) {

  int year,month,day,hour,min,sec;
  char *tstamp;
  time_t t_result;
  struct tm *tm_result;

  tstamp=ufud_alloc_string(env,32);
  t_result=time(NULL);
  tm_result=localtime(&t_result);

  year=tm_result->tm_year+1900;
  month=tm_result->tm_mon+1;
  day=tm_result->tm_mday;

  hour=tm_result->tm_hour;
  min=tm_result->tm_min;
  sec=tm_result->tm_sec;

  sprintf(tstamp,"%4d%02d%02d%02d%02d%02d",year,month,day,hour,min,sec);

  if(env->lognamefp!=NULL) {
    fprintf(env->lognamefp,"%s: %s\n",tstamp,env->msg);
    fflush(env->lognamefp);
  }

  ufud_free_string(env,tstamp);

}

