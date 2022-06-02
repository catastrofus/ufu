
#include "ufu.h"
#include "ufu.p"

void ufu_help(struct s_env *env,int panel,int function) {

  int row,col;
  WINDOW *w;

  strcpy(env->fun,"ufu_help");
  ufu_wai(env);

  row=1;
  col=9;

  ufu_wclear(env->top);
  ufu_wrefresh(env->top);

  ufu_wclear(env->bottom);
  ufu_wrefresh(env->bottom);

  w=ufu_popup(env,env->cols,env->rows-4,col-9,row+1,"HELP");

  switch(function) {

    case UFU_HELP_MAIN:
      mvwprintw(env->top,0,0,"[%s] Help for main window",env->nodename);
      mvwprintw(w,row++,col,"   %c    - Quit UFU.",UFU_KEY_QUIT);
      mvwprintw(w,row++,col," <up>   - To previous entry.");
      mvwprintw(w,row++,col," <dn>   - To next entry.");
      mvwprintw(w,row++,col,"   %c    - Scroll up.",UFU_KEY_SCROLL_UP);
      mvwprintw(w,row++,col,"   %c    - Scroll down.",UFU_KEY_SCROLL_DOWN);
      mvwprintw(w,row++,col,"   %c    - To previous page of entries.",UFU_KEY_PREVPAGE);
      mvwprintw(w,row++,col,"   %c    - To next page of entries.",UFU_KEY_NEXTPAGE);
      mvwprintw(w,row++,col,"   %c    - To first entry.",UFU_KEY_FIRST);
      mvwprintw(w,row++,col,"   %c    - To next entry.",UFU_KEY_LAST);
      mvwprintw(w,row++,col,"   %c    - Switch current panel #%d to \"%s\".",UFU_KEY_DIRHOME,panel,env->homedir);
      mvwprintw(w,row++,col,"   %c    - Switch current panel #%d to \"%s\".",UFU_KEY_DIRWORK,panel,env->workdir);
      mvwprintw(w,row++,col,"   %c    - Switch current panel #%d to \"/\".",UFU_KEY_DIRROOT,panel);
      if(strlen(env->panel[panel]->dirnameprev)>0) {
        mvwprintw(w,row++,col,"   %c    - Switch current panel #%d to \"%s\".",UFU_KEY_PREVDIR,panel,env->panel[panel]->dirnameprev);
      }
      mvwprintw(w,row++,col,"   %c    - Switch header.",UFU_KEY_HEADER);
      mvwprintw(w,row++,col,"   %c    - Switch sort column.",UFU_KEY_SORT_COL);
      mvwprintw(w,row++,col,"   %c    - Switch sort order.",UFU_KEY_SORT_ORDER);
      mvwprintw(w,row++,col," <rt>   - To next panel.");
      mvwprintw(w,row++,col," <lt>   - To previous panel.");
      mvwprintw(w,row++,col," <TB>   - Select panel.");
      if(env->rows<=24) {
        ufu_wrefresh(w);
        ufu_wrefresh(env->top);
        ufu_any_key(env);
        ufu_popdown(env,w);
        row=1;
        col=9;
        w=ufu_popup(env,env->cols,env->rows-4,col-9,row+1,"HELP");
        row=1;
        mvwprintw(env->top,0,0,"[%s] Help for main window",env->nodename);
        ufu_wrefresh(env->top);
      }
      mvwprintw(w,row++,col,"   %c    - To remote entries.",UFU_KEY_REMOTE);
      mvwprintw(w,row++,col,"   %c    - To marked entries.",UFU_KEY_MARKED);
      mvwprintw(w,row++,col,"<space> - Mark/Unmark entry.");
      mvwprintw(w,row++,col,"   %c    - Mark all entries.",UFU_KEY_APPEND);
      mvwprintw(w,row++,col,"   %c    - Show marked entries.",UFU_KEY_MARKED);
      mvwprintw(w,row++,col,"<enter> - Select.");
      mvwprintw(w,row++,col,"   %c    - User commands.",UFU_KEY_UCMD);
      mvwprintw(w,row++,col,"   %c    - Configuration.",UFU_KEY_CONFIG);
      mvwprintw(w,row++,col,"   %c    - Settings.",UFU_KEY_SETTING);
      mvwprintw(w,row++,col,"   %c    - Info.",UFU_KEY_INFO);
      mvwprintw(w,row++,col,"   %c    - View file.",UFU_KEY_VIEW);
      mvwprintw(w,row++,col,"   %c    - Edit file.",UFU_KEY_EDIT);
      mvwprintw(w,row++,col,"   %c    - Go to seqno.",UFU_KEY_GO);
      mvwprintw(w,row++,col,"   %c    - View users.",UFU_KEY_PLUS);
      mvwprintw(w,row++,col,"   %c    - View groups.",UFU_KEY_MINUS);
      mvwprintw(w,row++,col,"   %c    - View messages.",UFU_KEY__);
      mvwprintw(w,row++,col,"   %c    - Search pattern.",UFU_KEY_SEARCH);
      mvwprintw(w,row++,col,"   %c    - View logfile \"%s\".",UFU_KEY_LOG,env->logname);
      break;

    case UFU_HELP_PANEL:
      mvwprintw(env->top,0,0,"[%s] Help for select panel window",env->nodename);
      mvwprintw(w,row++,col,"   %c    - Quit panel.",UFU_KEY_QUIT);
      mvwprintw(w,row++,col," <up>   - To previous panel.");
      mvwprintw(w,row++,col," <dn>   - To next panel.");
      mvwprintw(w,row++,col," <pp>   - To previous page of panels.");
      mvwprintw(w,row++,col," <np>   - To next page of panels.");
      mvwprintw(w,row++,col,"   %c    - To first panel.",UFU_KEY_FIRST);
      mvwprintw(w,row++,col,"   %c    - To last panel.",UFU_KEY_LAST);
      mvwprintw(w,row++,col,"   %c    - Add new panel.",UFU_KEY_APPEND);
      mvwprintw(w,row++,col,"   %c    - Delete panel.",UFU_KEY_ERASE);
      mvwprintw(w,row++,col,"   %c    - Info panel.",UFU_KEY_INFO);
      mvwprintw(w,row++,col,"   %c    - To remote entries.",UFU_KEY_REMOTE);
      mvwprintw(w,row++,col,"<space> - Mark/Unmark panel.");
      mvwprintw(w,row++,col,"<enter> - Select.");
      mvwprintw(w,row++,col,"   %c    - View logfile \"%s\".",UFU_KEY_LOG,env->logname);
      break;

    case UFU_HELP_TC:
      mvwprintw(env->top,0,0,"[%s] Help for trash window",env->nodename);
      mvwprintw(w,row++,col,"   %c    - Quit trashcan.",UFU_KEY_QUIT);
      mvwprintw(w,row++,col," <up>   - To previous trash.");
      mvwprintw(w,row++,col," <dn>   - To next trash.");
      mvwprintw(w,row++,col," <pp>   - To previous page of trash.");
      mvwprintw(w,row++,col," <np>   - To next page of trash.");
      mvwprintw(w,row++,col,"   %c    - To first trash.",UFU_KEY_FIRST);
      mvwprintw(w,row++,col,"   %c    - To last trash.",UFU_KEY_LAST);
      mvwprintw(w,row++,col,"   %c    - View logfile \"%s\".",UFU_KEY_LOG,env->logname);
      break;

    case UFU_HELP_XINFO:
      mvwprintw(env->top,0,0,"[%s] Help for xinfo window",env->nodename);
      mvwprintw(w,row++,col,"   %c    - Quit xinfo.",UFU_KEY_QUIT);
      mvwprintw(w,row++,col," <up>   - To previous entry.");
      mvwprintw(w,row++,col," <dn>   - To next entry.");
      mvwprintw(w,row++,col," <pp>   - To previous page of entries.");
      mvwprintw(w,row++,col," <np>   - To next page of entries.");
      mvwprintw(w,row++,col,"   %c    - To first entry.",UFU_KEY_FIRST);
      mvwprintw(w,row++,col,"   %c    - To last panel.",UFU_KEY_LAST);
      mvwprintw(w,row++,col,"   %c    - Info entry.",UFU_KEY_INFO);
      mvwprintw(w,row++,col,"<enter> - Info entry.");
      break;

    case UFU_HELP_ID:
    case UFU_HELP_SELECT_ACTION:
    case UFU_HELP_SELECT_PANEL:
    case UFU_HELP_SELECT_PATH:
      mvwprintw(env->top,0,0,"[%s] Help for select window",env->nodename);
      mvwprintw(w,row++,col,"   %c    - Quit.",UFU_KEY_QUIT);
      mvwprintw(w,row++,col," <up>   - To previous line.");
      mvwprintw(w,row++,col," <dn>   - To next line.");
      mvwprintw(w,row++,col,"   %c    - To first line.",UFU_KEY_FIRST);
      mvwprintw(w,row++,col,"   %c    - To last line.",UFU_KEY_LAST);
      mvwprintw(w,row++,col,"<enter> - Select.");
      break;

    case UFU_HELP_CONFIG:
      mvwprintw(env->top,0,0,"[%s] Help for program configuration",env->nodename);
      mvwprintw(w,row++,col,"   %c    - Quit.",UFU_KEY_QUIT);
      mvwprintw(w,row++,col,"   %c    - Show standard key hit ratio.",UFU_KEY_1);
      mvwprintw(w,row++,col,"   %c    - Show usercommand key hit ratio.",UFU_KEY_2);
      mvwprintw(w,row++,col,"   %c    - Show history key hit ratio.",UFU_KEY_3);
      break;

    case UFU_HELP_MARK:
      mvwprintw(env->top,0,0,"[%s] Help for marked entries",env->nodename);
      mvwprintw(w,row++,col,"   %c    - Quit marked entries.",UFU_KEY_QUIT);
      mvwprintw(w,row++,col," <up>   - To previous marked entry.");
      mvwprintw(w,row++,col," <dn>   - To next marked entry.");
      mvwprintw(w,row++,col," <pp>   - To previous page of marked entries.");
      mvwprintw(w,row++,col," <np>   - To next page of marked entries.");
      mvwprintw(w,row++,col,"   %c    - To first set of marked entries.",UFU_KEY_FIRST);
      mvwprintw(w,row++,col,"   %c    - To next set of marked entries.",UFU_KEY_LAST);
      mvwprintw(w,row++,col,"   %c    - Info entry.",UFU_KEY_INFO);
      mvwprintw(w,row++,col,"   %c    - Configuration.",UFU_KEY_CONFIG);
      mvwprintw(w,row++,col,"   %c    - Settings.",UFU_KEY_SETTING);
      mvwprintw(w,row++,col,"<space> - Unmark entry.");
      mvwprintw(w,row++,col,"   %c    - Search marked entries.",UFU_KEY_SEARCH);
      mvwprintw(w,row++,col,"   %c    - Expunge marked entries.",UFU_KEY_EXPUNGE);
      mvwprintw(w,row++,col,"   %c    - View file.",UFU_KEY_VIEW);
      mvwprintw(w,row++,col,"   %c    - Edit file.",UFU_KEY_EDIT);
      mvwprintw(w,row++,col,"   %c    - View logfile \"%s\".",UFU_KEY_LOG,env->logname);
      mvwprintw(w,row++,col,"   %c    - Actions!",UFU_KEY_MARK_ACTION);
      break;

    case UFU_HELP_REMOTE:
      mvwprintw(env->top,0,0,"[%s] Help for remote entries",env->nodename);
      mvwprintw(w,row++,col,"   %c    - Quit remote entries.",UFU_KEY_QUIT);
      mvwprintw(w,row++,col," <up>   - To previous remote entry.");
      mvwprintw(w,row++,col," <dn>   - To next remote entry.");
      mvwprintw(w,row++,col," <pp>   - To previous page of remote entries.");
      mvwprintw(w,row++,col," <np>   - To next page of remote entries.");
      mvwprintw(w,row++,col,"   %c    - To first set of remote entries.",UFU_KEY_FIRST);
      mvwprintw(w,row++,col,"   %c    - To next set of remote entries.",UFU_KEY_LAST);
      mvwprintw(w,row++,col,"   %c    - Connect to remote entry.",UFU_KEY_CONNECT);
      mvwprintw(w,row++,col,"   %c    - Edit remote entry.",UFU_KEY_EDIT);
      mvwprintw(w,row++,col,"   %c    - Add remote entry.",UFU_KEY_APPEND);
      mvwprintw(w,row++,col,"   %c    - Remove remote entry.",UFU_KEY_ERASE);
      mvwprintw(w,row++,col,"   %c    - Enter password.",UFU_KEY_CLEAR);

      break;

    case UFU_HELP_UCMD:
      mvwprintw(env->top,0,0,"[%s] Help for usercommands",env->nodename);
      mvwprintw(w,row++,col,"   %c    - Quit usercommands.",UFU_KEY_QUIT);
      mvwprintw(w,row++,col," <up>   - To previous usercommand.");
      mvwprintw(w,row++,col," <dn>   - To next usercommand.");
      mvwprintw(w,row++,col," <pp>   - To previous page of usercommands.");
      mvwprintw(w,row++,col," <np>   - To next page of usercommands.");
      mvwprintw(w,row++,col,"   %c    - To first set of usercommands.",UFU_KEY_FIRST);
      mvwprintw(w,row++,col,"   %c    - To next set of usercommands.",UFU_KEY_LAST);
      mvwprintw(w,row++,col,"   %c    - Info usercommand.",UFU_KEY_INFO);
      mvwprintw(w,row++,col,"   %c    - Edit usercommand.",UFU_KEY_EDIT);
      mvwprintw(w,row++,col,"   %c    - Add usercommand.",UFU_KEY_APPEND);
      mvwprintw(w,row++,col,"   %c    - Remove usercommand.",UFU_KEY_ERASE);
      mvwprintw(w,row++,col,"<enter> - Execute usercommand.");
      mvwprintw(w,row++,col,"");
      mvwprintw(w,row++,col-6,"Substitutions - {A} will be replaced by the absolute filename.");
      mvwprintw(w,row++,col-6,"              - {R} will be replaced by the relative filename.");
      mvwprintw(w,row++,col-6,"              - {B} will be replaced by the basename.");
      break;

    case UFU_HELP_U:
      mvwprintw(env->top,0,0,"[%s] Help for viewing users",env->nodename);
      mvwprintw(w,row++,col,"   %c    - Quit view.",UFU_KEY_QUIT);
      mvwprintw(w,row++,col," <up>   - To previous user.");
      mvwprintw(w,row++,col," <dn>   - To next user.");
      mvwprintw(w,row++,col," <pp>   - To previous page of users.");
      mvwprintw(w,row++,col," <np>   - To next page of users.");
      mvwprintw(w,row++,col,"   %c    - To first set of users.",UFU_KEY_FIRST);
      mvwprintw(w,row++,col,"   %c    - To last set of users.",UFU_KEY_LAST);
      mvwprintw(w,row++,col,"   %c    - Show directory/shell.",UFU_KEY_1);
      mvwprintw(w,row++,col,"   %c    - Show secondary groups.",UFU_KEY_2);
      mvwprintw(w,row++,col,"   %c    - Switch sort between uid & username.",UFU_KEY_0);
      mvwprintw(w,row++,col," <left> - Shift secondary groups to the left.");
      mvwprintw(w,row++,col,"<right> - Shift secondary groups to the right.");
      break;

    case UFU_HELP_G:
      mvwprintw(env->top,0,0,"[%s] Help for viewing users",env->nodename);
      mvwprintw(w,row++,col,"   %c    - Quit view.",UFU_KEY_QUIT);
      mvwprintw(w,row++,col," <up>   - To previous user.");
      mvwprintw(w,row++,col," <dn>   - To next user.");
      mvwprintw(w,row++,col," <pp>   - To previous page of users.");
      mvwprintw(w,row++,col," <np>   - To next page of users.");
      mvwprintw(w,row++,col,"   %c    - To first set of users.",UFU_KEY_FIRST);
      mvwprintw(w,row++,col,"   %c    - To last set of users.",UFU_KEY_LAST);
      mvwprintw(w,row++,col,"   %c    - Switch sort between gid & groupname.",UFU_KEY_0);
      mvwprintw(w,row++,col," <left> - Shift users to the left.");
      mvwprintw(w,row++,col,"<right> - Shift users to the right.");
      break;

    case UFU_HELP_LOG:
      mvwprintw(env->top,0,0,"[%s] Help for viewing log",env->nodename);
      mvwprintw(w,row++,col,"   %c    - Quit view.",UFU_KEY_QUIT);
      mvwprintw(w,row++,col," <up>   - To previous line.");
      mvwprintw(w,row++,col," <dn>   - To next line.");
      mvwprintw(w,row++,col," <pp>   - To previous page of lines.");
      mvwprintw(w,row++,col," <np>   - To next page of lines.");
      mvwprintw(w,row++,col,"   %c    - To first set of lines.",UFU_KEY_FIRST);
      mvwprintw(w,row++,col,"   %c    - To last set of lines.",UFU_KEY_LAST);
      mvwprintw(w,row++,col,"   %c    - Toggle seqno on/off.",UFU_KEY_1);
      mvwprintw(w,row++,col,"   %c    - Toggle timestamp on/off.",UFU_KEY_2);
      mvwprintw(w,row++,col,"   %c    - Forward search.",UFU_KEY_SCROLL_DOWN);
      mvwprintw(w,row++,col,"   %c    - Backward search.",UFU_KEY_SCROLL_UP);
      mvwprintw(w,row++,col," <left> - Shift text to the left.");
      mvwprintw(w,row++,col,"<right> - Shift text to the right.");
      break;

    case UFU_HELP_RL:
      mvwprintw(env->top,0,0,"[%s] Help for input",env->nodename);
      mvwprintw(w,row++,col,"   %c    - Quit input.",UFU_KEY_QUIT);
      mvwprintw(w,row++,col," <pp>   - To previous word.");
      mvwprintw(w,row++,col,"   b    - To previous word.");
      mvwprintw(w,row++,col," <np>   - To next word.");
      mvwprintw(w,row++,col,"   w    - To next word.");
      mvwprintw(w,row++,col,"<bspace>- Delete character before cursor.");
      mvwprintw(w,row++,col,"   i    - Insert mode on.");
      mvwprintw(w,row++,col," <enter>- Insert mode off.");
      mvwprintw(w,row++,col,"   x    - Delete character under cursor.");
      mvwprintw(w,row++,col,"   D    - Delete to end of line.");
      mvwprintw(w,row++,col,"   d    - Delete to beginning of line.");
      mvwprintw(w,row++,col," <left> - Character left.");
      mvwprintw(w,row++,col," <right>- Character right.");
      break;

    default:
      mvwprintw(env->top,0,0,"[%s] Help!",env->nodename);
      mvwprintw(w,4,(env->cols-34)/2,"No help defined for this function!");
      break;

  }

  ufu_wrefresh(w);
  ufu_wrefresh(env->top);

  ufu_popdown(env,w);
  ufu_wrefresh(env->body);

  ufu_any_key(env);

}

