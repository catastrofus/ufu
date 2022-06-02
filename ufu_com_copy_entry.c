int ufu_com_copy_entry(struct s_env *env,struct s_mark *m) {

  int ok;
  char *f;
  struct s_buffer *b;

  strcpy(env->fun,"ufu_com_copy_entry");
  ufu_wai(env);

  strcpy(env->fun,"ufu_com_copy_entry");
  ufu_wai(env);

  f=ufu_alloc_string(env,UFU_LEN_NAME);

  ok=FALSE;

  if(ufu_com_init(env,env->panel[m->panel]->rem)) {

    b=ufu_alloc_buffer(env,UFU_B_UNLINK_ENTRY);

    ufu_crypt_passwd(env,b,env->panel[m->panel]->rem);

    b->action=UFU_B_UNLINK_ENTRY;
    b->last=TRUE;
    b->error=FALSE;

    ufu_clear_string(env,b->dirname,UFU_LEN_NAME);
    ufu_clear_string(env,b->filename,UFU_LEN_NAME);

    if(S_ISDIR(m->entry->fmode)) {
      // Directory.

      strcpy(b->dirname,env->panel[m->panel]->dirname);

      strcpy(f,b->dirname);

    }
    else {
      // File.

      strcpy(b->dirname,env->panel[m->panel]->dirname);
      strcpy(b->filename,m->entry->fname);

      ufu_concat_dir(env,f,b->dirname,b->filename);

    }

    sprintf(env->msg,"Remote file to unlink: %s",f);
    ufu_log(env);

    // Send to UFUd daemon on remote host.
    ok=ufu_send(env,env->panel[m->panel]->rem,b);
    if(ok) {

      // Receive from UFUd daemon.

      ufu_clear_string(env,b->username,UFU_LEN_UNAME);
      ufu_clear_string(env,b->password,UFU_LEN_PWORD);
      ufu_clear_string(env,b->dirname,UFU_LEN_NAME);
      ufu_clear_string(env,b->filename,UFU_LEN_NAME);
      ufu_clear_string(env,b->errmsg,UFU_LEN_ERROR);

      ufu_clear_string(env,b->e_fusername,UFU_LEN_UNAME);
      ufu_clear_string(env,b->e_fgroupname,UFU_LEN_GROUP);
      ufu_clear_string(env,b->e_lusername,UFU_LEN_UNAME);
      ufu_clear_string(env,b->e_lgroupname,UFU_LEN_GROUP);

      ufu_clear_string(env,b->e_fname,UFU_LEN_NAME);
      ufu_clear_string(env,b->e_lname,UFU_LEN_NAME);
      ufu_clear_string(env,b->e_fperm,UFU_LEN_PERM);
      ufu_clear_string(env,b->e_lperm,UFU_LEN_PERM);

      if(ufu_recv(env,env->panel[m->panel]->rem,b)) {

        if(!b->error) {

          // Do something with the data....

          sprintf(env->msg,"Unlink succesfull!");
          ufu_log(env);

        }
        else {

          // Send failed, clearing up panel p.

          strcpy(env->msg,b->errmsg);
          ufu_log(env);

        }

      }
      else {

        // Something wrong with transfer,
        // so let's close the connection.

        sprintf(env->msg,"Something wrong with transfer!");
        ufu_alert(env);

      }

      ufu_com_exit(env,env->panel[m->panel]->rem);

    }
    else {
      // Send failed, clearing up panel p.
      ufu_com_exit(env,env->panel[m->panel]->rem);

      sprintf(env->msg,"Send failed!");
      ufu_alert(env);

    }

    ufu_free_buffer(env,b,UFU_B_UNLINK_ENTRY);

  }
  else {
    sprintf(env->msg,"Unable to setup connection!");
    ufu_log(env);
    sprintf(env->msg,"Host %s is not reponding!",env->panel[m->panel]->rem->hostname);
    ufu_alert(env);
  }

  ufu_free_string(env,f);
 
  return(ok);

}

