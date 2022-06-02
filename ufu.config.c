
#include "ufu.h"
#include "ufu.p"

int ufu_read_config(struct s_env *env,int panel,char *cfgfile,int argc,char **argv) {

  int f_debug,f_version;
  int offset,found,i,j;
  int dir_count,cmd_count,rem_count;
  char hdr,sortc,sorto;
  char *line1,*line2,*hfile,*subs,*s;
  struct s_ucmd *uc,*uc1,*uc2;
  struct s_mark *m1,*m2;
  struct s_remote *rh;
  FILE *fp,*fpm;

  strcpy(env->fun,"ufu_read_config");
  ufu_wai(env);

  uc=ufu_alloc_ucmd(env);
  rh=ufu_alloc_remote(env);

  cmd_count=0;

  if(strcmp(cfgfile,UFU_GLOBAL_CONFIG)==0) {
    // Panel #0 is the panel with the files of you own homedirectory.
    strcpy(env->panel[panel]->dirname,env->homedir);
    sprintf(env->msg,"Putting HOMEDIR (%s) in panel #%d.",env->panel[panel]->dirname,panel);
    ufu_log(env);

    env->panel[panel]->local=FALSE;
    env->panel[panel]->remote=FALSE;

    // Put WORKDIR in panel #1, if different from HOMEDIR in panel #0.
    if(strcmp(env->homedir,env->workdir)!=0) {

      // Panel #1 is the panel with the files of your working directory.
      panel++;
      strcpy(env->panel[panel]->dirname,env->workdir);
      sprintf(env->msg,"Putting WORKDIR (%s) in panel #%d.",env->panel[panel]->dirname,panel);
      ufu_log(env);

      env->panel[panel]->local=FALSE;
      env->panel[panel]->remote=FALSE;

    }

    f_debug=FALSE;
    f_version=FALSE;

    // Process comandline arguments, if they exists.
    if(argc>0) {

      i=1;
      while(i<argc) {

        f_debug=(strcmp(argv[i],"-d")==0);
        f_version=(strcmp(argv[i],"-v")==0);

        if((!f_debug)&&(!f_version)) {

          found=FALSE;

          j=0;
          while(j<=panel) {

            if(!found) {
              found=(strcmp(env->panel[j]->dirname,argv[i])==0);
            }

            j++;

          }

          if(!found) {

            if(ufu_direxist(env,argv[i])) {

              panel++;

              strcpy(env->panel[panel]->dirname,argv[i]);
              sprintf(env->msg,"Putting arg #%d (%s) in panel #%d.",i,env->panel[panel]->dirname,panel);
              ufu_log(env);

              env->panel[panel]->local=FALSE;
              env->panel[panel]->remote=FALSE;

            }
            else {

              sprintf(env->msg,"Ignoring arg #%d (%s), unreadable directory!",i,argv[i]);
              ufu_log(env);

            }

          }
          else {

            sprintf(env->msg,"Ignoring arg #%d (%s), already exist in arguments!",i,argv[i]);
            ufu_log(env);

          }

        }

        i++;

      }

    }

    if(env->mpanel<panel) {

      sprintf(env->msg,"Adjusting max panels from %d to %d.",env->mpanel,panel);
      ufu_log(env);

      env->mpanel=panel;

    }

  }

  if((fp=fopen(cfgfile,"r"))!=NULL) {

    sprintf(env->msg,"Scanning config (%s)",cfgfile);
    ufu_log(env);

    // env->mfirst=NULL;
    // env->mlast=NULL;

    line1=ufu_alloc_string(env,UFU_MAX_TEXT);
    line2=ufu_alloc_string(env,UFU_MAX_TEXT);

    dir_count=0;
    rem_count=0;

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
          ufu_log(env);
        }

        // Extract EDITOR from configfile.
        if(strstr(line1,"EDITOR=")) {
          strcpy(line2,line1+7);
          line2[strlen(line2)-1]='\0';
          strcpy(env->editor,line2);
          sprintf(env->msg," Found EDITOR (%s).",env->editor);
          ufu_log(env);
        }

        // Extract VIEWER from configfile.
        if(strstr(line1,"VIEWER=")) {
          strcpy(line2,line1+7);
          line2[strlen(line2)-1]='\0';
          strcpy(env->viewer,line2);
          sprintf(env->msg," Found VIEWER (%s).",env->viewer);
          ufu_log(env);
        }

        // Extract SHELL_HISTFILE from configfile.
        if(strstr(line1,"SHELL_HISTFILE=")) {
          strcpy(line2,line1+15);
          line2[strlen(line2)-1]='\0';
          strcpy(env->sh_hfile,line2);

          strcpy(line2,env->homedir);
          strcat(line2,"/");
          strcat(line2,env->sh_hfile);

          sprintf(env->msg," Found SHELL_HISTFILE (%s).",line2);
          ufu_log(env);
        }

        // Extract PANELS (max number of panels allowed) from configfile.
        if(strstr(line1,"PANELS=")) {
          strcpy(line2,line1+7);
          env->mpanel=atoi(line2);
          sprintf(env->msg," Found PANELS (%d).",env->mpanel);
          ufu_log(env);
          if(env->mpanel<UFU_MAX_PANEL_LOW) {
            sprintf(env->msg," Setting PANELS to %d.",UFU_MAX_PANEL_LOW);
            ufu_log(env);
            env->mpanel=UFU_MAX_PANEL_LOW;
          }
          if(env->mpanel>UFU_MAX_PANEL) {
            sprintf(env->msg," Setting PANELS to %d.",UFU_MAX_PANEL);
            ufu_log(env);
            env->mpanel=UFU_MAX_PANEL;
          }
        }

        // Extract WIDTH (number of columns) from configfile.
        if(strstr(line1,"ROWS=")) {
          strcpy(line2,line1+5);
          env->cfg_rows=atoi(line2);
          sprintf(env->msg," Found ROWS (%d).",env->cfg_rows);
          ufu_log(env);
        }

        // Extract HEIGHT (number of rowe) from configfile.
        if(strstr(line1,"COLUMNS=")) {
          strcpy(line2,line1+8);
          env->cfg_cols=atoi(line2);
          sprintf(env->msg," Found COLUMNS (%d).",env->cfg_cols);
          ufu_log(env);
        }

        // Max entries to mark.
        if(strstr(line1,"MAX_TO_MARK=")) {
          strcpy(line2,line1+12);
          env->max_to_mark=atoi(line2);
          sprintf(env->msg," Found MAX_TO_MARK (%d).",env->max_to_mark);
          ufu_log(env);
        }

        // Confirmation of quit actions.
        if(strstr(line1,"CONFIRMQUIT=")) {
          strcpy(line2,line1+12);
          env->confirmquit=(tolower(*line2)=='y');
          if(env->confirmquit)
            sprintf(env->msg," Setting CONFIRMQUIT to TRUE");
          else
            sprintf(env->msg," Setting CONFIRMQUIT to FALSE");
          ufu_log(env);
        }

        // Confirmation of copy actions.
        if(strstr(line1,"CONFIRMCOPY=")) {
          strcpy(line2,line1+12);
          env->confirmcopy=(tolower(*line2)=='y');
          if(env->confirmcopy)
            sprintf(env->msg," Setting CONFIRMCOPY to TRUE");
          else
            sprintf(env->msg," Setting CONFIRMCOPY to FALSE");
          ufu_log(env);
        }

        // Confirmation of move actions.
        if(strstr(line1,"CONFIRMMOVE=")) {
          strcpy(line2,line1+12);
          env->confirmmove=(tolower(*line2)=='y');
          if(env->confirmmove)
            sprintf(env->msg," Setting CONFIRMMOVE to TRUE");
          else
            sprintf(env->msg," Setting CONFIRMMOVE to FALSE");
          ufu_log(env);
        }

        // Confirmation of remove actions.
        if(strstr(line1,"CONFIRMREMOVE=")) {
          strcpy(line2,line1+14);
          env->confirmremove=(tolower(*line2)=='y');
          if(env->confirmremove)
            sprintf(env->msg," Setting CONFIRMREMOVE to TRUE");
          else
            sprintf(env->msg," Setting CONFIRMREMOVE to FALSE");
          ufu_log(env);
        }

        // Save marked entries.
        if(strstr(line1,"SAVEMARKED=")) {
          strcpy(line2,line1+11);
          env->savemarked=(tolower(*line2)=='y');
          if(env->savemarked)
            sprintf(env->msg," Setting SAVEMARKED to TRUE");
          else
            sprintf(env->msg," Setting SAVEMARKED to FALSE");
          ufu_log(env);
        }

        // Write config to file?
        if(strstr(line1,"WRITECONFIG=")) {
          strcpy(line2,line1+12);
          env->writeconfig=(tolower(*line2)=='y');
          if(env->writeconfig)
            sprintf(env->msg," Setting WRITECONFIG to TRUE");
          else
            sprintf(env->msg," Setting WRITECONFIG to FALSE");
          ufu_log(env);
        }

        // Remove log after exit?
        if(strstr(line1,"REMOVELOG=")) {
          strcpy(line2,line1+10);
          env->removelog=(tolower(*line2)=='y');
          if(env->removelog)
            sprintf(env->msg," Setting REMOVELOG to TRUE");
          else
            sprintf(env->msg," Setting REMOVELOG to FALSE");
          ufu_log(env);
        }

        // Query for action confirmation.
        if(strstr(line1,"CONFIRMACTION=")) {
          strcpy(line2,line1+12);
          env->confirmaction=(tolower(*line2)=='y');
          if(env->confirmaction)
            sprintf(env->msg," Setting CONFIRMACTION to TRUE");
          else
            sprintf(env->msg," Setting CONFIRMACTION to FALSE");
          ufu_log(env);
        }

        // Include subdirectories.
        if(strstr(line1,"INCSUBDIRS=")) {
          strcpy(line2,line1+11);
          env->incsubdir=(tolower(*line2)=='y');
          if(env->incsubdir)
            sprintf(env->msg," Setting INCSUBDIRS to TRUE");
          else
            sprintf(env->msg," Setting INCSUBDIRS to FALSE");
          ufu_log(env);
        }

        // Ignore errors?
        if(strstr(line1,"IGNOREERROR=")) {
          strcpy(line2,line1+12);
          env->ignoreerror=(tolower(*line2)=='y');
          if(env->ignoreerror)
            sprintf(env->msg," Setting IGNOREERROR to TRUE");
          else
            sprintf(env->msg," Setting IGNOREERROR to FALSE");
          ufu_log(env);
        }

        // Show directories first.
        if(strstr(line1,"DIRFIRST=")) {
          strcpy(line2,line1+9);
          env->dirfirst=(tolower(*line2)=='y');
          if(env->dirfirst)
            sprintf(env->msg," Setting DIRFIRST to TRUE");
          else
            sprintf(env->msg," Setting DIRFIRST to FALSE");
          ufu_log(env);
        }

        // HIST_xyz values.
        if(strstr(line1,"HIST_PANEL=")) {
          strcpy(line2,line1+11);
          env->hist_panel=atoi(line2);
          sprintf(env->msg," Found HIST_PANEL (%d).",env->hist_panel);
        }

        // HIST_xyz values.
        if(strstr(line1,"HIST_MARK=")) {
          strcpy(line2,line1+10);
          env->hist_mark=atoi(line2);
          sprintf(env->msg," Found HIST_MARK (%d).",env->hist_mark);
        }

        // HIST_xyz values.
        if(strstr(line1,"HIST_MARK_ACTION=")) {
          strcpy(line2,line1+17);
          env->hist_mark_action=atoi(line2);
          sprintf(env->msg," Found HIST_MARK_ACTION (%d).",env->hist_mark_action);
        }

        // HIST_xyz values.
        if(strstr(line1,"HIST_UCMD=")) {
          strcpy(line2,line1+10);
          env->hist_ucmd=atoi(line2);
          sprintf(env->msg," Found HIST_UCMD (%d).",env->hist_ucmd);
        }

        // HIST_xyz values.
        if(strstr(line1,"HIST_XINFO=")) {
          strcpy(line2,line1+11);
          env->hist_xinfo=atoi(line2);
          sprintf(env->msg," Found HIST_XINFO (%d).",env->hist_xinfo);
        }

        // HIST_xyz values.
        if(strstr(line1,"HIST_REMOTE=")) {
          strcpy(line2,line1+12);
          env->hist_remote=atoi(line2);
          sprintf(env->msg," Found HIST_REMOTE (%d).",env->hist_remote);
        }

        // HIST_xyz values.
        if(strstr(line1,"HIST_TC=")) {
          strcpy(line2,line1+8);
          env->hist_tc=atoi(line2);
          sprintf(env->msg," Found HIST_TC (%d).",env->hist_tc);
        }

        // HIST_xyz values.
        if(strstr(line1,"HIST_SETUP=")) {
          strcpy(line2,line1+11);
          env->hist_setup=atoi(line2);
          sprintf(env->msg," Found HIST_SETUP (%d).",env->hist_setup);
        }

        // HIST_xyz values.
        if(strstr(line1,"HIST_U=")) {
          strcpy(line2,line1+7);
          env->hist_u=atoi(line2);
          sprintf(env->msg," Found HIST_U (%d).",env->hist_u);
        }

        // HIST_xyz values.
        if(strstr(line1,"HIST_G=")) {
          strcpy(line2,line1+7);
          env->hist_g=atoi(line2);
          sprintf(env->msg," Found HIST_G (%d).",env->hist_g);
        }

        // HIST_xyz values.
        if(strstr(line1,"HIST_MSG=")) {
          strcpy(line2,line1+9);
          env->hist_msg=atoi(line2);
          sprintf(env->msg," Found HIST_MSG (%d).",env->hist_msg);
        }

        // Extract usercommand from configfile.
        if(strstr(line1,"CMD=")) {
          strcpy(line2,line1+4);
          line2[strlen(line2)-1]='\0';
          strcpy(uc->exec,line2);
          sprintf(env->msg," Found CMD #%03d: %s.",cmd_count,line2);
          ufu_log(env);
          uc->seqno=cmd_count;
          uc->key=' ';
          uc->history=FALSE;
          ufu_add_ucmd(env,uc);
          sprintf(env->msg,"CMD:%4d-%s",uc->seqno,uc->exec);
          ufu_log(env);
          cmd_count++;
        }

        // Extract usercommand from configfile.
        if((strstr(line1,"CMD0="))||
           (strstr(line1,"CMD1="))||
           (strstr(line1,"CMD2="))||
           (strstr(line1,"CMD3="))||
           (strstr(line1,"CMD4="))||
           (strstr(line1,"CMD5="))||
           (strstr(line1,"CMD6="))||
           (strstr(line1,"CMD7="))||
           (strstr(line1,"CMD8="))||
           (strstr(line1,"CMD9="))) {
          strcpy(line2,line1+5);
          line2[strlen(line2)-1]='\0';
          strcpy(uc->exec,line2);
          sprintf(env->msg," Found CMD%c: %s.",line1[3],line2);
          ufu_log(env);
          uc->seqno=cmd_count;
          uc->key=line1[3];
          uc->history=FALSE;
          ufu_add_ucmd(env,uc);
          sprintf(env->msg,"CMD:%4d-%c-%s",uc->seqno,uc->key,uc->exec);
          ufu_log(env);
          cmd_count++;
        }

        // Extract remote hosts from configfile.
        if(strstr(line1,"REMOTE=")) {
          strcpy(line2,line1+7);
          line2[strlen(line2)-1]='\0';
          strcpy(rh->hostname,"\0");
          strcpy(rh->username,"\0");
          strcpy(rh->password,"\0");
          rh->port=0;
          if(ufu_split_remote(env,rh,line2,strcmp(cfgfile,UFU_GLOBAL_CONFIG)!=0)) {
            ufu_add_remote(env,rh,strcmp(cfgfile,UFU_GLOBAL_CONFIG)!=0);
            sprintf(env->msg," Found REMOTE #%02d: host=%s,port=%d,user=%s.",rem_count,rh->hostname,rh->port,rh->username);
            ufu_log(env);
            rem_count++;
          }
        }

        // Extract minimum number of history lines from configfile.
        if(strstr(line1,"HISTORY_MIN=")) {
          strcpy(line2,line1+12);
          line2[strlen(line2)-1]='\0';
          env->history_min=atoi(line2);
          sprintf(env->msg," Found MIN HISTORY (%d).",env->history_min);
          ufu_log(env);
        }

        // Extract maximum number of history lines from configfile.
        if(strstr(line1,"HISTORY_MAX=")) {
          strcpy(line2,line1+12);
          line2[strlen(line2)-1]='\0';
          env->history_max=atoi(line2);
          sprintf(env->msg," Found MAX HISTORY (%d).",env->history_max);
          ufu_log(env);
        }

        // Extract TIMEOUT from configfile.
        if(strstr(line1,"TIMEOUT=")) {
          strcpy(line2,line1+8);
          line2[strlen(line2)-1]='\0';
          env->timeout=atoi(line2);
          sprintf(env->msg," Found TIMEOUT (%d).",env->timeout);
          ufu_log(env);
        }

        // Extract TIMEOUT_RETRY from configfile.
        if(strstr(line1,"TIMEOUT_RETRY=")) {
          strcpy(line2,line1+14);
          line2[strlen(line2)-1]='\0';
          env->timeout_retry=atoi(line2);
          sprintf(env->msg," Found TIMEOUT_RETRY (%d).",env->timeout_retry);
          ufu_log(env);
        }

        // Extract TIMEIN from configfile.
        if(strstr(line1,"TIMEIN=")) {
          strcpy(line2,line1+7);
          line2[strlen(line2)-1]='\0';
          env->timein=atoi(line2);
          sprintf(env->msg," Found TIMEIN (%d).",env->timein);
          ufu_log(env);
        }

        // Extract TIMEIN_RETRY from configfile.
        if(strstr(line1,"TIMEIN_RETRY=")) {
          strcpy(line2,line1+13);
          line2[strlen(line2)-1]='\0';
          env->timein_retry=atoi(line2);
          sprintf(env->msg," Found TIMEIN_RETRY (%d).",env->timein_retry);
          ufu_log(env);
        }




        // Extract MARKED_ENTRY (marked entries) from configfile.
        if(strstr(line1,"MARKED_ENTRY=")) {
          strcpy(line2,line1+13);
          line2[strlen(line2)-1]='\0';
          sprintf(env->msg," Found MARKED_ENTRY: %s.",line2);
          ufu_log(env);

          if(env->mfirst!=NULL) {
            m1->next=ufu_alloc_mark(env);
	    m1->next->prev=m1;
	    m1=m1->next;
          }
	  else {
            env->mfirst=ufu_alloc_mark(env);
	    m1=env->mfirst;
            sprintf(env->msg,"  This is the first marked entry");
            ufu_log(env);
	  }

          subs=strtok(line2,":");
	  if(subs!=NULL) {
            // Seqno.
            m1->seqno=atoi(subs);
            sprintf(env->msg," - Found seqno %d.",m1->seqno);
            ufu_log(env);
	  }

          subs=strtok(NULL,":");
	  if(subs!=NULL) {
            // Panel..
            m1->panel=atoi(subs);
            sprintf(env->msg," - Found panel %d.",m1->panel);
            ufu_log(env);
	  }

          subs=strtok(NULL,":");
	  if(subs!=NULL) {
            // Level..
            m1->level=atoi(subs);
            sprintf(env->msg," - Found level %d.",m1->level);
            ufu_log(env);
	  }

          env->panel[UFU_MAX_PANEL+1]=ufu_alloc_panel(env,UFU_MAX_PANEL+1);

          subs=strtok(NULL,":");
	  if(subs!=NULL) {
            // Directoryname.
            strcpy(m1->dname,subs);
            strcpy(env->panel[UFU_MAX_PANEL+1]->dirname,subs);
            sprintf(env->msg," - Found directory \"%s\".",m1->dname);
            ufu_log(env);
	  }

          subs=strtok(NULL,":");
	  if(subs!=NULL) {
            // Filename.
            m1->entry=ufu_alloc_entry(env);
            strcpy(m1->entry->fname,subs);
            sprintf(env->msg," - Found file \"%s\".",m1->entry->fname);
            ufu_log(env);
            ufu_stat_entry(env,UFU_MAX_PANEL+1,m1->entry);
	  }

	  // Check if marked entry exists.
	  s=ufu_alloc_string(env,strlen(m1->dname)+strlen(m1->entry->fname)+4);
	  ufu_concat_dir(env,s,m1->dname,m1->entry->fname);
          fpm=fopen(s,"r");
	  if(fpm==NULL) {
            // File can't be opened.
            sprintf(env->msg,"Marked entry \"%s\" removed. File doesn't exists!",s);
            ufu_log(env);
	    if(m1->prev!=NULL) {
              // Not the first marked entry.
              m1=m1->prev;
	      ufu_free_mark(env,m1->next);
	      m1->next=NULL;
	    }
	    else {
              // First entry.
	      ufu_free_mark(env,m1);
	      env->mfirst=NULL;
	    }
	  }
	  else {
            // File exeists so close this file.
            fclose(fpm);
	  }

          ufu_free_panel(env,UFU_MAX_PANEL+1);

        }

        // Extract paneldirectory from configfile.
        if(strstr(line1,"DIR=")) {
          if(panel<env->mpanel) {
            strcpy(line2,line1+4);
            line2[strlen(line2)-1]='\0';
            if(ufu_direxist(env,line2+4)) {
              panel++;
              offset=0;
              hdr='S';
              sortc='f';
              sorto='a';
              env->panel[panel]->header=UFU_HDR_STD;
              if((strlen(line2)>4)&&(line2[3]==':')) {
                /*
                 * Format: [HCO,]directoryname
                 * Whereis:
                 *    H - Header:
                 *      S - Standard,
                 *      A - Date/Time of last access,
                 *      M - Date/Time of last modification,
                 *      C - Date/Time of last change.
                 *    C - Columns to sort on:
                 *      F - Filename,
                 *      S - Size,
                 *      A - Date/Time of last access,
                 *      M - Date/Time of last modification,
                 *      C - Date/Time of last change.
                 *    O - Sort order:
                 *      A - Ascending,
                 *      D - Descending.
                 */
                offset=4;
                hdr=line2[0];
                sortc=line2[1];
                sorto=line2[2];
                switch(hdr) {
                  case 'S':
                  case 's':
                    env->panel[panel]->header=UFU_HDR_STD;
                    break;
                  case 'A':
                  case 'a':
                    env->panel[panel]->header=UFU_HDR_LA;
                    break;
                  case 'M':
                  case 'm':
                    env->panel[panel]->header=UFU_HDR_LM;
                    break;
                  case 'C':
                  case 'c':
                    env->panel[panel]->header=UFU_HDR_LC;
                    break;
                }
                switch(sortc) {
                  case 'F':
                  case 'f':
                    env->panel[panel]->sortc=UFU_SORT_FNAME;
                    break;
                  case 'S':
                  case 's':
                    env->panel[panel]->sortc=UFU_SORT_SIZE;
                    break;
                  case 'A':
                  case 'a':
                    env->panel[panel]->sortc=UFU_SORT_LA;
                    break;
                  case 'M':
                  case 'm':
                    env->panel[panel]->sortc=UFU_SORT_LM;
                    break;
                  case 'C':
                  case 'c':
                    env->panel[panel]->sortc=UFU_SORT_LC;
                    break;
                }
                switch(sorto) {
                  case 'A':
                  case 'a':
                    env->panel[panel]->sorto=UFU_SORT_ASC;
                    break;
                  case 'D':
                  case 'd':
                    env->panel[panel]->sorto=UFU_SORT_DSC;
                    break;
                }
              }

              strcpy(env->panel[panel]->dirname,line2+offset);
              dir_count++;
              env->panel[panel]->local=(strcmp(cfgfile,UFU_GLOBAL_CONFIG)!=0);
              env->panel[panel]->remote=FALSE;
              if(env->panel[panel]->local)
                sprintf(env->msg," Found (local) DIR #%02d: %s,%c%c%c for panel #%d.",dir_count,env->panel[panel]->dirname,tolower(hdr),tolower(sortc),tolower(sorto),panel);
              else
                sprintf(env->msg," Found (global) DIR #%02d: %s,%c%c%c for panel #%d.",dir_count,env->panel[panel]->dirname,tolower(hdr),tolower(sortc),tolower(sorto),panel);
              ufu_log(env);
            }
            else {
              sprintf(env->msg," Ignoring DIR (%s), unreadable directory!",line2+4);
              ufu_log(env);
            }
          }
          else {
            sprintf(env->msg," Ignoring DIR (%s). No panels left!",env->panel[panel]->dirname);
            ufu_log(env);
          }
        }

      }

    }

    fclose(fp);

    // Mark every entry which is in the list of marked entries.
    m1=env->mfirst;
    while(m1!=NULL) {
      ufu_find_entry_from_mark(env,m1,TRUE);
      m1=m1->next;
    }

    // Reading SHELL_HISTFILE in ucmd.
    cmd_count=0;
    hfile=ufu_alloc_string(env,strlen(env->homedir)+40);
    strcpy(hfile,env->homedir);
    strcat(hfile,"/");
    if(strlen(env->sh_hfile)>0) {
      strcat(hfile,env->sh_hfile);
      sprintf(env->msg," Using (shell) historyfile %s.",hfile);
      ufu_log(env);
    }
    else {
      strcat(hfile,UFU_HISTORY_FILE);
      sprintf(env->msg," Config for (shell) historyfile not found.");
      ufu_log(env);
      sprintf(env->msg," Using default (shell) historyfile %s",hfile);
      ufu_log(env);
      strcpy(env->sh_hfile,UFU_HISTORY_FILE);
    }
    if((fp=fopen(hfile,"r"))!=NULL) {
      sprintf(env->msg," Reading file %s",hfile);
      ufu_log(env);
      while(fgets(line1,128,fp)!=NULL) {
        line1[strlen(line1)-1]='\0';
        found=0;
        uc1=env->ucfirst;
        while((uc1!=NULL)&&(found==0)) {
          if(strcmp(uc1->exec,line1)==0) {
            found=1;
          }
          uc1=uc1->next;
        }
        if(cmd_count>0) {
          if(found==0) {
            uc1=ufu_alloc_ucmd(env);
            uc1->local=TRUE;
            uc1->history=TRUE;
            strcpy(uc1->exec,line1);
            uc2=env->ucfirst;
            while(uc2->next!=NULL) {
              uc2=uc2->next;
            }
            uc1->prev=uc2;
            uc2->next=uc1;
            cmd_count++;
            sprintf(env->msg,"  +Reading uc \"%s\".",line1);
            ufu_log(env);
          }
          else {
            sprintf(env->msg,"  -Reading uc \"%s\".",line1);
            ufu_log(env);
          }
        }
        else {
          if(found==0) {
            uc1=ufu_alloc_ucmd(env);
            uc1->local=TRUE;
            uc1->history=TRUE;
            strcpy(uc1->exec,line1);
            env->ucfirst=uc1;
            cmd_count++;
            sprintf(env->msg,"  +Reading uc \"%s\".",line1);
            ufu_log(env);
          }
        }

      }
      sprintf(env->msg," Done reading file %s",hfile);
      ufu_log(env);
      fclose(fp);
      ufu_renum_ucmd(env);
    }
    free(hfile);

    sprintf(env->msg,"End of scan.");
    ufu_log(env);

    if(env->mpanel<panel) {

      sprintf(env->msg,"Adjusting max panels from %d to %d.",env->mpanel,panel);
      ufu_log(env);

      env->mpanel=panel;

    }

    ufu_read_msg(env);

    ufu_free_ucmd(env,uc);
    ufu_free_remote(env,rh);

    free(line1);
    free(line2);

  }
  else {

    sprintf(env->msg,"Configfile \"%s\" not found.",cfgfile);
    ufu_log(env);

  }

  return(panel);

}

void ufu_write_config(struct s_env *env) {

  int c,p,found;
  char hdr,sortc,sorto;
  char *line;

  strcpy(env->fun,"ufu_write_config");
  ufu_wai(env);

  struct s_ucmd *uc;
  struct s_remote *rh;
  struct s_mark *m;

  FILE *fp;

  strcpy(env->fun,"ufu_write_config");
  ufu_wai(env);

  if((fp=fopen(env->cfglocal,"w"))!=NULL) {

    sprintf(env->msg,"Start writing config (%s).",env->cfglocal);
    ufu_log(env);

    sprintf(env->msg,"Writing config (%s)",env->cfglocal);
    ufu_msg(env);

    line=ufu_alloc_string(env,256);

    sprintf(line,"# DEBUG: Set this program in DEBUG mode.\n");
    fputs(line,fp);
    sprintf(line,"#        Gives us more logging.\n");
    fputs(line,fp);
    if(env->debug)
      sprintf(line,"DEBUG=Y\n");
    else
      sprintf(line,"DEBUG=N\n");
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# EDITOR: Use this as the default (read/write) editor.\n");
    fputs(line,fp);
    sprintf(line,"EDITOR=%s\n",env->editor);
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# VIEWER: Use this as the default (read only) viewer.\n");
    fputs(line,fp);
    sprintf(line,"VIEWER=%s\n",env->viewer);
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);


    sprintf(line,"# SHELL_HISTFILE: Use this as the default history file provided by the shell used.\n");
    fputs(line,fp);
    sprintf(line,"SHELL_HISTFILE=%s\n",env->sh_hfile);
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# HISTORY: Minimum number of entries retained in the history structure.\n");
    fputs(line,fp);
    sprintf(line,"#          Default 99\n");
    fputs(line,fp);
    sprintf(line,"HISTORY_MIN=%d\n",env->history_min);
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# HISTORY: Maximum number of entries retained in the history structure.\n");
    fputs(line,fp);
    sprintf(line,"#          Default 99\n");
    fputs(line,fp);
    sprintf(line,"HISTORY_MAX=%d\n",env->history_max);
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# ROWS: Screensize - Number of rows.\n");
    fputs(line,fp);
    sprintf(line,"#       Is only used if the program can't find this value in the environment.\n");
    fputs(line,fp);
    sprintf(line,"#       Minimum value is 24.\n");
    fputs(line,fp);
    sprintf(line,"ROWS=24\n");
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# COLUMNS: Screensize - Number of columns.\n");
    fputs(line,fp);
    sprintf(line,"#          Is only used if the program can't find this value in the environment.\n");
    fputs(line,fp);
    sprintf(line,"#          Minimum value is 80.\n");
    fputs(line,fp);
    sprintf(line,"COLUMNS=80\n");
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# PANELS: Maximum number of panels allowed.\n");
    fputs(line,fp);
    sprintf(line,"#         Do NOT change this value unless told otherwise!\n");
    fputs(line,fp);
    sprintf(line,"#         Minimum value is %d.\n",UFU_MAX_PANEL_LOW);
    fputs(line,fp);
    sprintf(line,"PANELS=%d\n",env->mpanel);
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# MAX_TO_MARK: Max entries to mark.\n");
    fputs(line,fp);
    sprintf(line,"#       This is the maximum number of entries UFU can mark.\n");
    fputs(line,fp);
    sprintf(line,"#       Default value is 1024, 1K entries.\n");
    fputs(line,fp);
    sprintf(line,"MAX_TO_MARK=%d\n",env->max_to_mark);
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# CONFIRMCOPY: Confirmation of copy actions.\n");
    fputs(line,fp);
    sprintf(line,"#              Default Y(es)\n");
    fputs(line,fp);
    if(env->confirmcopy)
      sprintf(line,"CONFIRMCOPY=Y\n");
    else
      sprintf(line,"CONFIRMCOPY=N\n");
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# CONFIRMQUIT: Confirmation of quit actions.\n");
    fputs(line,fp);
    sprintf(line,"#              Default Y(es)\n");
    fputs(line,fp);
    if(env->confirmquit)
      sprintf(line,"CONFIRMQUIT=Y\n");
    else
      sprintf(line,"CONFIRMQUIT=N\n");
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# CONFIRMMOVE: Confirmation of move actions.\n");
    fputs(line,fp);
    sprintf(line,"#              Default Y(es)\n");
    fputs(line,fp);
    if(env->confirmmove)
      sprintf(line,"CONFIRMMOVE=Y\n");
    else
      sprintf(line,"CONFIRMMOVE=N\n");
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# CONFIRMREMOVE: Confirmation of remove actions.\n");
    fputs(line,fp);
    sprintf(line,"#                Default Y(es)\n");
    fputs(line,fp);
    if(env->confirmremove)
      sprintf(line,"CONFIRMREMOVE=Y\n");
    else
      sprintf(line,"CONFIRMREMOVE=N\n");
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# CONFIRMACTION: Query for action confirmation.\n");
    fputs(line,fp);
    sprintf(line,"#                Default Y(es)\n");
    fputs(line,fp);
    if(env->confirmaction)
      sprintf(line,"CONFIRMACTION=Y\n");
    else
      sprintf(line,"CONFIRMACTION=N\n");
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# SAVEMARKED: Save the marked entries in the configfile.\n");
    fputs(line,fp);
    sprintf(line,"#                Default N(o)\n");
    fputs(line,fp);
    if(env->savemarked)
      sprintf(line,"SAVEMARKED=Y\n");
    else
      sprintf(line,"SAVEMARKED=N\n");
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# WRITECONFIG: Write program config to disk before quitting.\n");
    fputs(line,fp);
    sprintf(line,"#                Default Y(es)\n");
    fputs(line,fp);
    if(env->writeconfig)
      sprintf(line,"WRITECONFIG=Y\n");
    else
      sprintf(line,"WRITECONFIG=N\n");
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# REMOVELOG: Remove log after a successfull exit of the program.\n");
    fputs(line,fp);
    sprintf(line,"#            Default Y(es)\n");
    fputs(line,fp);
    if(env->removelog)
      sprintf(line,"REMOVELOG=Y\n");
    else
      sprintf(line,"REMOVELOG=N\n");
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# INCSUBDIRS: Include subdirectories.\n");
    fputs(line,fp);
    sprintf(line,"#             Default N(o)\n");
    fputs(line,fp);
    if(env->incsubdir)
      sprintf(line,"INCSUBDIRS=Y\n");
    else
      sprintf(line,"INCSUBDIRS=N\n");
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# IGNOREERROR: Ignore errors on copy/[re]move actions.\n");
    fputs(line,fp);
    sprintf(line,"#              Default N(o)\n");
    fputs(line,fp);
    if(env->ignoreerror)
      sprintf(line,"IGNOREERROR=Y\n");
    else
      sprintf(line,"IGNOREERROR=N\n");
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# DIRFIRST: Show directories first before the regular files.\n");
    fputs(line,fp);
    sprintf(line,"#           Default Y(es)\n");
    fputs(line,fp);
    if(env->dirfirst)
      sprintf(line,"DIRFIRST=Y\n");
    else
      sprintf(line,"DIRFIRST=N\n");
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# TIMEOUT: Transfer timeout, output.\n");
    fputs(line,fp);
    sprintf(line,"#          Default 100\n");
    fputs(line,fp);
    sprintf(line,"TIMEOUT=%d\n",env->timeout);
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# TIMEOUT_RETRY: Retry transfer timeout.\n");
    fputs(line,fp);
    sprintf(line,"#                Default 4\n");
    fputs(line,fp);
    sprintf(line,"TIMEOUT_RETRY=%d\n",env->timeout_retry);
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# TIMEIN: Transfer timeout, input.\n");
    fputs(line,fp);
    sprintf(line,"#         Default 100\n");
    fputs(line,fp);
    sprintf(line,"TIMEIN=%d\n",env->timein);
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# TIMEIN_RETRY: Retry transfer timeout.\n");
    fputs(line,fp);
    sprintf(line,"#               Default 4\n");
    fputs(line,fp);
    sprintf(line,"TIMEIN_RETRY=%d\n",env->timein_retry);
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# HIST_xyz: Where was I in which screen?.\n");
    fputs(line,fp);
    sprintf(line,"HIST_PANEL=%d\n",env->hist_panel);
    fputs(line,fp);
    sprintf(line,"HIST_MARK=%d\n",env->hist_mark);
    fputs(line,fp);
    sprintf(line,"HIST_MARK_ACTION=%d\n",env->hist_mark_action);
    fputs(line,fp);
    sprintf(line,"HIST_UCMD=%d\n",env->hist_ucmd);
    fputs(line,fp);
    sprintf(line,"HIST_XINFO=%d\n",env->hist_xinfo);
    fputs(line,fp);
    sprintf(line,"HIST_REMOTE=%d\n",env->hist_remote);
    fputs(line,fp);
    sprintf(line,"HIST_TC=%d\n",env->hist_tc);
    fputs(line,fp);
    sprintf(line,"HIST_SETUP=%d\n",env->hist_setup);
    fputs(line,fp);
    sprintf(line,"HIST_U=%d\n",env->hist_u);
    fputs(line,fp);
    sprintf(line,"HIST_G=%d\n",env->hist_g);
    fputs(line,fp);
    sprintf(line,"HIST_MSG=%d\n",env->hist_msg);
    fputs(line,fp);

    sprintf(line,"\n");
    fputs(line,fp);

    ufu_renum_ucmd(env);

    c=0;
    uc=env->ucfirst;
    while(uc!=NULL) {
      sprintf(env->msg,"CMDx: (%c) %s",uc->key,uc->exec);
      ufu_log(env);
      if(uc->key!=' ') {
        if(c==0) {
          sprintf(line,"# CMD: Keyed usercommands.\n");
          fputs(line,fp);
        }
        sprintf(line,"CMD%c=%s\n",uc->key,uc->exec);
        fputs(line,fp);
        c++;
      }
      uc=uc->next;
    }

    sprintf(line,"\n");
    fputs(line,fp);

    c=0;
    uc=env->ucfirst;
    while(uc!=NULL) {
      sprintf(env->msg,"CMD: (%c) %s",uc->key,uc->exec);
      ufu_log(env);
      if((uc->local)&&(!uc->history)&&(uc->key==' ')) {
        if(c==0) {
          sprintf(line,"# CMD: Usercommands.\n");
          fputs(line,fp);
        }
        sprintf(line,"CMD=%s\n",uc->exec);
        fputs(line,fp);
        c++;
      }
      uc=uc->next;
    }

    sprintf(line,"\n");
    fputs(line,fp);

    rh=env->rhfirst;
    if(rh!=NULL) {
      found=FALSE;
      while((rh!=NULL)&&(!found)) {
        if(!found)  found=rh->local;
        rh=rh->next;
      }
      if(found) {
        rh=env->rhfirst;
        sprintf(line,"# REMOTE: Remote hosts.\n");
        fputs(line,fp);
        sprintf(line,"#         host:port:username[:dirname]\n");
        fputs(line,fp);
        while(rh!=NULL) {
          sprintf(line,"REMOTE=%s:%d:%s:%s\n",rh->hostname,rh->port,rh->username,rh->dirname);
          fputs(line,fp);
          rh=rh->next;
        }
      }
    }

    sprintf(line,"\n");
    fputs(line,fp);

    sprintf(line,"# DIR: List of current available panels.\n");
    fputs(line,fp);

    p=UFU_MIN_PANEL+1;
    while((p<=UFU_MAX_PANEL)&&(strlen(env->panel[p]->dirname)>0)) {

      if(env->panel[p]->local) {

        sprintf(env->msg," Writing panel #%d (%s).",p,env->panel[p]->dirname);
        ufu_log(env);

        switch(env->panel[p]->header) {
          case UFU_HDR_STD:
            hdr='s';
            break;
          case UFU_HDR_LA:
            hdr='a';
            break;
          case UFU_HDR_LM:
            hdr='m';
            break;
          case UFU_HDR_LC:
            hdr='c';
            break;
        }

        switch(env->panel[p]->sortc) {
          case UFU_SORT_FNAME:
            sortc='f';
            break;
          case UFU_SORT_SIZE:
            sortc='s';
            break;
          case UFU_SORT_LA:
            sortc='a';
            break;
          case UFU_SORT_LM:
            sortc='m';
            break;
          case UFU_SORT_LC:
            sortc='c';
            break;
        }

        switch(env->panel[p]->sorto) {
          case UFU_SORT_ASC:
            sorto='a';
            break;
          case UFU_SORT_DSC:
            sorto='d';
            break;
        }

        sprintf(line,"DIR=%c%c%c:%s\n",hdr,sortc,sorto,env->panel[p]->dirname);
        fputs(line,fp);

      }
      else {

        sprintf(env->msg," Skipping global panel #%d (%s).",p,env->panel[p]->dirname);
        ufu_log(env);

      }

      p++;

    }

    if(env->savemarked) {
      if(env->mfirst!=NULL) {
        sprintf(line,"\n");
        fputs(line,fp);
        sprintf(line,"# MARKED_ENTRY: List of the marked entries.\n");
        fputs(line,fp);
	m=env->mfirst;
	while(m!=NULL) {
	  if(m->entry!=NULL) {
            sprintf(line,"MARKED_ENTRY=%d:%d:%d:%s:%s\n",m->seqno,m->panel,m->level,m->dname,m->entry->fname);
            fputs(line,fp);
          }
	  m=m->next;
        }
      }
    }

    fclose(fp);

    chmod(env->cfglocal,S_IRUSR|S_IWUSR);

    ufu_free_string(env,line);

    sprintf(env->msg,"Finished writing config (%s).",env->cfglocal);
    ufu_log(env);

  }
  else {

    sprintf(env->msg,"Unable to open configfile \"%s\" for writing!",env->cfglocal);
    ufu_log(env);

  }

}

void ufu_show_config(struct s_env *env) {

  int key,col,row,again,p,r,i;
  long int min,max,total;
  char *s;
  struct s_mark *m;
  struct s_hist *h;

  WINDOW *w,*w1,*w2;

  strcpy(env->fun,"ufu_show_config");
  ufu_wai(env);

  strcpy(env->fun,"ufu_show_config");
  ufu_wai(env);

  again=TRUE;
  while(again) {

    r=(env->rows-18)/2-4;
    if(r<2) {
      r=2;
    }
    w=ufu_popup(env,env->cols-20,20,10,r,"Program configuration");

    ufu_wclear(env->top);
    mvwprintw(env->top,0,0,"[%s] Show program configuration",env->nodename);
    ufu_wrefresh(env->top);

    row=1;
    col=2;

    ufu_wclear(env->bottom);
    ufu_wrefresh(env->bottom);

    mvwprintw(w,row++,col,"Pid          : %d (%d)",env->pid,env->ppid);
    mvwprintw(w,row++,col,"User/Group   : %s (%d) / %s (%d)",env->uname,env->uid,env->gname,env->gid);
    mvwprintw(w,row++,col,"Rows/Columns : %d/%d",env->rows,env->cols);
    mvwprintw(w,row++,col,"Ext editor   : %-40s",env->editor);
    mvwprintw(w,row++,col,"Ext viewer   : %-40s",env->viewer);
    mvwprintw(w,row++,col,"Global config: %-40s",env->cfgglobal);
    mvwprintw(w,row++,col,"Local config : %-40s",env->cfglocal);
    mvwprintw(w,row++,col,"Home dir     : %-40s",env->homedir);
    mvwprintw(w,row++,col,"Work dir     : %-40s",env->workdir);
    ufu_wrefresh(w);

    i=0;
    h=env->hfirst;
    while(h!=NULL) {
      i++;
      h=h->next;
    }

    if(i>0) {
      mvwprintw(w,row++,col,"History min  : %d entries min, %d entries max, %d in use",env->history_min,env->history_max,i);
    }
    else {
      mvwprintw(w,row++,col,"History min  : %d entries min, %d entries max",env->history_min,env->history_max);
    }

    i=0;
    m=env->mfirst;
    while(m!=NULL) {
      i++;
      m=m->next;
    }

    if(i>0) {
      mvwprintw(w,row++,col,"Max to mark  : %d entries, %d in use, %d free",env->max_to_mark,i,env->max_to_mark-i);
    }
    else {
      mvwprintw(w,row++,col,"Max to mark  : %d entries",env->max_to_mark);
    }
    mvwprintw(w,row++,col,"Logfile      : %s",env->logname);

    total=(long int)0;
    min=(long int)env->panel[UFU_MIN_PANEL]->ntotal;
    max=(long int)env->panel[UFU_MIN_PANEL]->ntotal;
    p=UFU_MIN_PANEL;
    while((strlen(env->panel[p]->dirname)>0)&&(p<env->mpanel)) {
      total=total+env->panel[p]->ntotal;
      if(env->panel[p]->ntotal<min)
        min=env->panel[p]->ntotal;
      if(env->panel[p]->ntotal>max)
        max=env->panel[p]->ntotal;
      p++;
    }
    p--;

    mvwprintw(w,row++,col,"Panels       : %d of %d total",p+1,env->mpanel+1);

    if(p>0)
      mvwprintw(w,row++,col,"Entries      : %ld (min %ld,max %ld,avg %ld)",total,min,max,total/p);
    else
      mvwprintw(w,row++,col,"Entries      : %ld (min %ld,max %ld)",total,min,max);

    if(env->rb) {
      mvwprintw(w,row++,col,"Recycle path : %s",env->rb_path);
      mvwprintw(w,row++,col,"Recycle size : %d MBytes max",env->rb_size);
    }

    mvwprintw(w,row++,col,"Nodename     : %-40s",env->nodename);
    mvwprintw(w,row++,col,"Sysname      : %-40s",env->sysname);
    mvwprintw(w,row++,col,"Release      : %-40s",env->release);
 
    if(env->lmem>(1024*1024)) {
      mvwprintw(w,row++,col,"Allocated mem: %ld B (%.2f KB, %.2f MB)",env->lmem,env->lmem/1024.0,env->lmem/(1024.0*1024.0));
    }
    else {
      if(env->lmem>1024) {
        mvwprintw(w,row++,col,"Allocated mem: %ld B (%.2f KB)",env->lmem,env->lmem/1024.0);
      }
      else {
        mvwprintw(w,row++,col,"Allocated mem: %ld B",env->lmem);
      }
    }

    ufu_wrefresh(w);

    key=ufu_get_key(env,UFU_NO_TEXT,NULL);

    switch(key) {

      case UFU_KEY_QUIT:

        ufu_popdown(env,w);
        again=FALSE;

        break;

      case UFU_KEY_HELP:
        env->key_help++;
        ufu_help(env,0,UFU_HELP_CONFIG);
        break;

      case UFU_KEY_1:

        ufu_wclear(env->top);
        mvwprintw(env->top,0,0,"[%s] Show program configuration (used keys (for your eyes only))",env->nodename);
        ufu_wrefresh(env->top);

        ufu_wclear(env->bottom);
        ufu_wrefresh(env->bottom);

        row=1;
        col=2;

        w1=ufu_popup(env,env->cols,21,0,(env->rows-21)/2+1,"Program configuration (screen #2)");
        ufu_wrefresh(w1);

        mvwprintw(w1,row++,col,"Key          Times used");
        mvwprintw(w1,row++,col,"------------ ----------");

        mvwprintw(w1,row++,col,"Help         %6d",env->key_help);
        mvwprintw(w1,row++,col,"Up           %6d",env->key_up);
        mvwprintw(w1,row++,col,"Down         %6d",env->key_down);
        mvwprintw(w1,row++,col,"Left         %6d",env->key_left);
        mvwprintw(w1,row++,col,"Right        %6d",env->key_right);
        mvwprintw(w1,row++,col,"Prev page    %6d",env->key_prev_page);
        mvwprintw(w1,row++,col,"Next page    %6d",env->key_next_page);
        mvwprintw(w1,row++,col,"Panel        %6d",env->key_panel);
        mvwprintw(w1,row++,col,"Prev panel   %6d",env->key_prev_panel);
        mvwprintw(w1,row++,col,"Next panel   %6d",env->key_next_panel);
        mvwprintw(w1,row++,col,"Updir        %6d",env->key_dir_up);
        mvwprintw(w1,row++,col,"Homedir      %6d",env->key_dir_home);
        mvwprintw(w1,row++,col,"Workdir      %6d",env->key_dir_work);
        mvwprintw(w1,row++,col,"Prevdir      %6d",env->key_dir_prev);
        mvwprintw(w1,row++,col,"Rootdir      %6d",env->key_dir_root);
        mvwprintw(w1,row++,col,"Go           %6d",env->key_go);
        mvwprintw(w1,row++,col,"Setting      %6d",env->key_setting);

        row=1;
        col=29;

        mvwprintw(w1,row++,col,"Key          Times used");
        mvwprintw(w1,row++,col,"------------ ----------");

        mvwprintw(w1,row++,col,"Info         %6d",env->key_info);
        mvwprintw(w1,row++,col,"XInfo        %6d",env->key_xinfo);
        mvwprintw(w1,row++,col,"Config       %6d",env->key_config);
        mvwprintw(w1,row++,col,"Select       %6d",env->key_select);
        mvwprintw(w1,row++,col,"Edit         %6d",env->key_edit);
        mvwprintw(w1,row++,col,"View         %6d",env->key_view);
        mvwprintw(w1,row++,col,"Expunge      %6d",env->key_expunge);
        mvwprintw(w1,row++,col,"Show mark    %6d",env->key_show_mark);
        mvwprintw(w1,row++,col,"Add mark     %6d",env->key_add_mark);
        mvwprintw(w1,row++,col,"Remove mark  %6d",env->key_rem_mark);
        mvwprintw(w1,row++,col,"Add panel    %6d",env->key_add_panel);
        mvwprintw(w1,row++,col,"Remove panel %6d",env->key_rem_panel);
        mvwprintw(w1,row++,col,"Add ucmd     %6d",env->key_add_ucmd);
        mvwprintw(w1,row++,col,"Remove ucmd  %6d",env->key_rem_ucmd);
        mvwprintw(w1,row++,col,"Header       %6d",env->key_header);
        mvwprintw(w1,row++,col,"Sort order   %6d",env->key_sort_order);
        mvwprintw(w1,row++,col,"Sort column  %6d",env->key_sort_col);

        row=1;
        col=54;

        mvwprintw(w1,row++,col,"Key          Times used");
        mvwprintw(w1,row++,col,"------------ ----------");

        mvwprintw(w1,row++,col,"Search       %6d",env->key_search);
        mvwprintw(w1,row++,col,"Trash        %6d",env->key_tc);
        mvwprintw(w1,row++,col,"Show users   %6d",env->key_u);
        mvwprintw(w1,row++,col,"Show groups  %6d",env->key_g);

        ufu_wrefresh(w1);
        ufu_wrefresh(env->body);

        ufu_any_key(env);

        ufu_popdown(env,w1);

        break;

      case UFU_KEY_2:

        row=1;
        col=2;

        w2=ufu_popup(env,54,14,(env->cols-54)/2,(env->rows-14)/2+2,"Program configuration (screen #3)");
        ufu_wrefresh(w2);

        mvwprintw(w2,row++,col,"Key          Times used");
        mvwprintw(w2,row++,col,"------------ ----------");

        mvwprintw(w2,row++,col,"Config #1    %6d",env->key_config_1);
        mvwprintw(w2,row++,col,"Config #2    %6d",env->key_config_2);
        mvwprintw(w2,row++,col,"Config #3    %6d",env->key_config_3);
        mvwprintw(w2,row++,col,"Config #4    %6d",env->key_config_4);
        mvwprintw(w2,row++,col,"Config #5    %6d",env->key_config_5);
        mvwprintw(w2,row++,col,"Config #6    %6d",env->key_config_6);
        mvwprintw(w2,row++,col,"Config #7    %6d",env->key_config_7);

        row=1;
        col=29;

        mvwprintw(w2,row++,col,"Key          Times used");
        mvwprintw(w2,row++,col,"------------ ----------");

        mvwprintw(w2,row++,col,"Exec ucmd #0 %6d",env->key_ucmd_0);
        mvwprintw(w2,row++,col,"Exec ucmd #1 %6d",env->key_ucmd_1);
        mvwprintw(w2,row++,col,"Exec ucmd #2 %6d",env->key_ucmd_2);
        mvwprintw(w2,row++,col,"Exec ucmd #3 %6d",env->key_ucmd_3);
        mvwprintw(w2,row++,col,"Exec ucmd #4 %6d",env->key_ucmd_4);
        mvwprintw(w2,row++,col,"Exec ucmd #5 %6d",env->key_ucmd_5);
        mvwprintw(w2,row++,col,"Exec ucmd #6 %6d",env->key_ucmd_6);
        mvwprintw(w2,row++,col,"Exec ucmd #7 %6d",env->key_ucmd_7);
        mvwprintw(w2,row++,col,"Exec ucmd #8 %6d",env->key_ucmd_8);
        mvwprintw(w2,row++,col,"Exec ucmd #9 %6d",env->key_ucmd_9);

        ufu_wrefresh(w2);

        ufu_any_key(env);

        ufu_popdown(env,w2);

        break;

      case UFU_KEY_3:

        h=env->hfirst;
        if(h!=NULL) {

          s=ufu_alloc_string(env,60);

          ufu_wclear(env->top);
          mvwprintw(env->top,0,0,"[%s] Show program history (for your eyes only)",env->nodename);
          ufu_wrefresh(env->top);

          while(h->next!=NULL) {
            h=h->next;
          }

          ufu_wclear(env->bottom);
          ufu_wrefresh(env->bottom);

          row=1;
          col=2;

          while(h!=NULL) {

            ufu_wclear(w);

            mvwprintw(w,row++,col,"Part        SeqNo Directory");
            mvwprintw(w,row++,col,"----------- ----- ----------------------------------------------------------");

            i=0;

            while((h!=NULL)&&(i<(env->rows-8))) {

              switch(h->what) {
                case UFU_HIST_ENTRY:
                  mvwprintw(w,row,col,"Entry");
                  break;
                case UFU_HIST_PANEL:
                  mvwprintw(w,row,col,"Panel");
                  break;
                case UFU_HIST_MARK:
                  mvwprintw(w,row,col,"Mark");
                  break;
                case UFU_HIST_MARK_ACTION:
                  mvwprintw(w,row,col,"Mark action");
                  break;
                case UFU_HIST_UCMD:
                  mvwprintw(w,row,col,"Usercmd");
                  break;
                case UFU_HIST_XINFO:
                  mvwprintw(w,row,col,"XInfo");
                  break;
                case UFU_HIST_REMOTE:
                  mvwprintw(w,row,col,"Remote");
                  break;
                case UFU_HIST_SETUP:
                  mvwprintw(w,row,col,"Setup");
                  break;
                default:
                  mvwprintw(w,row,col,"?");
                  break;
              }

              ufu_resize_string(env,h->dname,58,s);

              mvwprintw(w,row++,col+12,"%5d %-56s",h->seqno,s);
              h=h->prev;

              i++;

            }

          }

          ufu_wrefresh(w);

          ufu_any_key(env);

          ufu_popdown(env,w);

          ufu_free_string(env,s);

        }

        break;

    }

  }

}

void ufu_show_setting(struct s_env *env) {

  int again,changed,key,col,menu_min,menu_max,menu_c,i;
  char *menu[]={
    "                          Debug",
    "  Confirmation for quit actions",
    "  Confirmation for copy actions",
    "  Confirmation for move actions",
    "Confirmation for remove actions",
    "           Save marked entries?",
    "  Query for action confirmation",
    "                  Ignore errors",
    "         Include subdirectories",
    "         Show directories first",
    "                Use recycle bin",
    "                     Remove log",
    "                   Write config"};
  char *yes="Yes",*no="No!",*r;
  WINDOW *w;

  strcpy(env->fun,"ufu_show_setting");
  ufu_wai(env);

  menu_min=0;
  menu_max=12;

  w=ufu_popup(env,40,menu_max+3,(env->cols-40)/2,(env->rows-(menu_max+5))/2,"Program settings");
  ufu_wrefresh(w);

  r=ufu_alloc_string(env,4);

  menu_c=ufu_find_hist(env,UFU_HIST_SETUP,NULL);

  col=1;

  again=TRUE;
  changed=FALSE;

  while(again) {

    for(i=menu_min;i<=menu_max;i++) {
      if(i==menu_c)  wattron(w,A_REVERSE);
      strcpy(r,no);
      switch(i) {
        case 0:
          if(env->debug)          strcpy(r,yes);
          break;
        case 1:
          if(env->confirmquit)    strcpy(r,yes);
          break;
        case 2:
          if(env->confirmcopy)    strcpy(r,yes);
          break;
        case 3:
          if(env->confirmmove)    strcpy(r,yes);
          break;
        case 4:
          if(env->confirmremove)  strcpy(r,yes);
          break;
        case 5:
          if(env->savemarked)     strcpy(r,yes);
          break;
        case 6:
          if(env->confirmaction)  strcpy(r,yes);
          break;
        case 7:
          if(env->ignoreerror)    strcpy(r,yes);
          break;
        case 8:
          if(env->incsubdir)      strcpy(r,yes);
          break;
        case 9:
          if(env->dirfirst)       strcpy(r,yes);
          break;
        case 10:
          if(env->rb)             strcpy(r,yes);
          break;
        case 11:
          if(env->removelog)      strcpy(r,yes);
          break;
        case 12:
          if(env->writeconfig)    strcpy(r,yes);
          break;
      }
      mvwprintw(w,i+1,col," %s? %s ",menu[i],r);
      if(i==menu_c)  wattroff(w,A_REVERSE);

    }

    ufu_wrefresh(w);

    key=ufu_get_key(env,UFU_NO_TEXT,NULL);

    switch(key) {

      case UFU_KEY_QUIT:
        again=FALSE;
        ufu_add_hist(env,UFU_HIST_SETUP,NULL,menu_c);
        if(changed)  ufu_write_config(env);
        break;

      case UFU_KEY_SELECT:
      case UFU_KEY_ENTER:
      case UFU_KEY_LEFT:
      case UFU_KEY_RIGHT:
      case UFU_KEY_MARK:
        switch(menu_c) {
          case 0:
            env->debug=!env->debug;
            break;
          case 1:
            env->confirmquit=!env->confirmquit;
            break;
          case 2:
            env->confirmcopy=!env->confirmcopy;
            break;
          case 3:
            env->confirmmove=!env->confirmmove;
            break;
          case 4:
            env->confirmremove=!env->confirmremove;
            break;
          case 5:
            env->savemarked=!env->savemarked;
            break;
          case 6:
            env->confirmaction=!env->confirmaction;
            break;
          case 7:
            env->ignoreerror=!env->ignoreerror;
            break;
          case 8:
            env->incsubdir=!env->incsubdir;
            break;
          case 9:
            env->dirfirst=!env->dirfirst;
            break;
          case 10:
            env->rb=!env->rb;
            break;
          case 11:
            env->removelog=!env->removelog;
            break;
          case 12:
            env->writeconfig=!env->writeconfig;
            break;
        }
        changed=TRUE;
        break;

      case UFU_KEY_UP:
        if(menu_c>menu_min)
          menu_c--;
        else
          menu_c=menu_max;
        break;

      case UFU_KEY_DOWN:
        if(menu_c<menu_max)
          menu_c++;
        else
          menu_c=menu_min;
        break;

      default:
        ufu_wrong_key(env);
        break;

    }

  }

  ufu_free_string(env,r);

  ufu_popdown(env,w);
  ufu_wrefresh(env->body);

}

void ufu_findext(struct s_env *env,char *file,char *s) {

  FILE *fp;
  int found,c;
  char *path,*subs,*bin,*fullpath;

  strcpy(env->fun,"ufu_findext");
  ufu_wai(env);

  if(strlen(s)==0) {

    fullpath=ufu_alloc_string(env,1024);
    sprintf(fullpath,"/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:/opt/bin:/opt/sbin:%s/bin:%s/sbin:%s/bin:%s/sbin",env->homedir,env->homedir,env->workdir,env->workdir);

    path=ufu_alloc_string(env,1024);

    sprintf(env->msg,"Searching PATH -> Looking for binary '%s'",file);
    ufu_log(env);

    if(env->debug) {
      sprintf(env->msg,"Searchpath to find \"%s\":",file);
      ufu_log(env);
      c=0;
      strcpy(path,fullpath);
      subs=strtok(path,":");
      while(subs!=NULL) {
        sprintf(env->msg,"%2d %s",c,subs);
        ufu_log(env);
        c++;
        subs=strtok(NULL,":");
      }
    }

    found=FALSE;
    strcpy(path,fullpath);
    subs=strtok(path,":");

    while((!found)&&(subs!=NULL)) {

      sprintf(env->msg," PATH: %s",subs);
      ufu_log(env);

      bin=ufu_alloc_string(env,strlen(subs)+strlen(file)+3);
      strcpy(bin,subs);
      strcat(bin,"/");
      strcat(bin,file);

      found=((fp=fopen(bin,"r"))!=NULL);
      if(found) {
        fclose(fp);
        strcpy(s,bin);
      }

      ufu_free_string(env,bin);

      subs=strtok(NULL,":");

    }

    if(strlen(s)>0) {

      sprintf(env->msg,"Searching PATH -> Found: %s",s);
      ufu_log(env);

    }
    else {

      sprintf(env->msg,"Searching PATH -> Not found!");
      ufu_log(env);

    }

    ufu_free_string(env,fullpath);
    ufu_free_string(env,path);

  }

}

