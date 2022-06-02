
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <poll.h>
#include <pwd.h>
#include <grp.h>
#include <crypt.h>
#include <errno.h>
#include <libgen.h>

#include "ufu.build"
#include "ufu.k"

#define	UFU_VERSION			"Version 0 Release 0 Patchlevel 0"
#define UFU_COPYRIGHT			"(c) 2006..2022 WNW"
#define	UFU_LEN_VERSION			80

#define	UFU_CR_LINE1			"This program is free software: you can redistribute it and/or modify it under"
#define	UFU_CR_LINE2			"the terms of the GNU General Public License as published by the Free Software"
#define	UFU_CR_LINE3			"Foundation, either version 3 of the License, or any later version."
#define	UFU_CR_LINE4			"This program is distributed in the hope that it will be useful, but WITHOUT"
#define	UFU_CR_LINE5			"ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS"
#define	UFU_CR_LINE6			"FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details."


#define	UFU_MIN_ROWS			24
#define	UFU_MIN_COLS			80

#define	UFU_USR_FILE			"/etc/passwd"
#define	UFU_GRP_FILE			"/etc/group"

#define	UFU_LEN_UNAME			16
#define	UFU_LEN_GROUP			16
#define	UFU_LEN_PWORD			32
#define	UFU_LEN_UDIR			64
#define	UFU_LEN_USH				32
#define	UFU_LEN_ERROR			80

#define	UFU_GLOBAL_MSG			"/etc/ufu/ufu.msg"
#define	UFU_GLOBAL_CONFIG		"/etc/ufu/ufu.cfg"
#define	UFU_LOCAL_CONFIG		"ufu.cfg"
#define	UFU_CFGDIR			".ufu"
#define	UFU_LOGDIR			"log"
#define	UFU_LOGFILE			"log"
#define	UFU_TCDIR			"trashcan"
#define	UFU_TCFILE			"trashcan.text"
#define	UFU_HISTORY_FILE		".bash_history"

#define	UFU_MIN_PANEL			0
#define	UFU_MAX_PANEL			99
#define	UFU_MAX_PANEL_LOW		9

#define	UFU_LEN_FNAME			96
#define	UFU_LEN_DNAME			128
#define	UFU_LEN_NAME			(UFU_LEN_FNAME+UFU_LEN_DNAME+1)

#define	UFU_LEN_TS			16

#define	UFU_LEN_PERM			11
#define	UFU_LEN_MSG				255
#define	UFU_LEN_FUN				64

#define	UFU_LEN_UTS				128

#define	UFU_LEN_MSGID				16

#define	UFU_POLLIN				1
#define	UFU_POLLOUT				2

#define	UFU_LEN_MASTER			(UFU_LEN_UNAME+UFU_LEN_UTS+8)

#define	UFU_UCMD_MIN			0
#define	UFU_UCMD_MAX			9
#define	UFU_LEN_UCMD			256
#define	UFU_LEN_UCMD_VIS		68
#define	UFU_LEN_RL_STEP			4
#define	UFU_LEN_SEARCH			64

#define	UFU_HELP_MAIN			0
#define	UFU_HELP_PANEL			1
#define	UFU_HELP_ENTRY			2
#define	UFU_HELP_MARK			3
#define	UFU_HELP_UCMD			4
#define	UFU_HELP_REMOTE			5
#define	UFU_HELP_XINFO			6
#define	UFU_HELP_RL			7
#define	UFU_HELP_TC			8
#define	UFU_HELP_ID			9
#define	UFU_HELP_CONFIG			10
#define	UFU_HELP_U			11
#define	UFU_HELP_G			12
#define	UFU_HELP_SELECT_ACTION		13
#define	UFU_HELP_SELECT_PANEL		14
#define	UFU_HELP_SELECT_PATH		15
#define	UFU_HELP_LOG			16


#define	UFU_KEY_0			'0'
#define	UFU_KEY_1			'1'
#define	UFU_KEY_2			'2'
#define	UFU_KEY_3			'3'
#define	UFU_KEY_4			'4'
#define	UFU_KEY_5			'5'
#define	UFU_KEY_6			'6'
#define	UFU_KEY_7			'7'
#define	UFU_KEY_8			'8'
#define	UFU_KEY_9			'9'

#define	UFU_MARK_ACTION_NONE	0
#define	UFU_MARK_ACTION_MOVE	1
#define	UFU_MARK_ACTION_COPY	2
#define	UFU_MARK_ACTION_REMOVE	3

#define	UFU_HDR_STD				0
#define	UFU_HDR_LA				1
#define	UFU_HDR_LM				2
#define	UFU_HDR_LC				3

#define	UFU_SORT_FNAME			0
#define	UFU_SORT_SIZE			1
#define	UFU_SORT_LA				2
#define	UFU_SORT_LM				3
#define	UFU_SORT_LC				4

#define	UFU_SORT_ASC			1
#define	UFU_SORT_DSC			2

#define	UFU_IS_NOT				0
#define	UFU_IS_REG				1
#define	UFU_IS_DIR				2

#define	UFU_NO_TEXT				0
#define	UFU_IS_MARKED			1
#define	UFU_IS_EXECUTED			2
#define	UFU_IS_REMOTE			3
#define	UFU_MAX_TO_MARK			4

#define	UFU_REM_HOSTNAME		16
#define	UFU_REM_USERNAME		16
#define	UFU_REM_PASSWORD		32
#define	UFU_REM_DIRNAME			64
#define	UFU_REM_PORT			5

#define	UFU_RL_REMOTE_H			0
#define	UFU_RL_REMOTE_P			1
#define	UFU_RL_REMOTE_U			2
#define	UFU_RL_REMOTE_D			3

#define	UFU_HIST_ENTRY			'e'
#define	UFU_HIST_PANEL			'p'
#define	UFU_HIST_MARK			'm'
#define	UFU_HIST_MARK_ACTION	'a'
#define	UFU_HIST_UCMD			'u'
#define	UFU_HIST_XINFO			'x'
#define	UFU_HIST_REMOTE			'r'
#define	UFU_HIST_TC				't'
#define	UFU_HIST_SETUP			's'
#define	UFU_HIST_U			'y'
#define	UFU_HIST_G			'z'

#define	UFU_SEARCH_ENTRY		'e'
#define	UFU_SEARCH_PANEL		'p'
#define	UFU_SEARCH_MARK			'm'

#define	UFU_INFO_ENTRY			0
#define	UFU_INFO_MARK			1
#define	UFU_INFO_PANEL			2
#define	UFU_INFO_XINFO			3

#define	UFU_MAX_MARK			1024
#define	UFU_MAX_TEXT			1024

#define	UFU_SEND				0
#define	UFU_RECV				1

#define	UFU_B_NONE				0
#define	UFU_B_GET_VERSION		1
#define	UFU_B_READ_DIR			2
#define	UFU_B_READ_ENTRY		3
#define	UFU_B_GET_TXTFILE		4
#define	UFU_B_PUT_TXTFILE		5
#define	UFU_B_GET_BINFILE		6
#define	UFU_B_PUT_BINFILE		7
#define	UFU_B_UNLINK_ENTRY		8

#define	UFU_VIEW				0
#define	UFU_EDIT				1

#define	UFU_ACTION_BUFFER		8192

#define	UFU_THIS_DIRECTORY		"[    this directory    ]"
#define	UFU_UP_DIRECTORY		"[     up directory     ]"

#define	UFU_NOERR				0x0000000000000000

#define UFU_X_NF				0
#define UFU_X_F_NC				1
#define	UFU_X_F_C				2

struct s_env {
  struct s_panel *panel[UFU_MAX_PANEL-UFU_MIN_PANEL+2];
  WINDOW *top;
  WINDOW *top_sep;
  WINDOW *body;
  WINDOW *bottom_sep;
  WINDOW *bottom;
  int cpanel;
  int mpanel;
  int cols;
  int rows;
  long int reterror;
  char *msg;
  char *fun;
  char *msgglobal;
  char *cfgglobal;
  char *cfglocal;
  char *homedir;
  char *workdir;
  char *path;
  char *tcdir;
  char *tcfile;
  char *sh_hfile;
  int timeout;
  int timeout_retry;
  int timein;
  int timein_retry;
  int pid;
  int ppid;
  char *logname;
  FILE *lognamefp;
  int uid;
  int gid;
  char *uname;
  char *gname;
  struct s_log *lfirst;
  struct s_txt *tfirst;
  struct s_mark *mfirst;
  struct s_mark *mlast;
  struct s_ucmd *ucfirst;
  struct s_ucmd *uclast;
  struct s_remote *rhfirst;
  struct s_remote *rhlast;
  struct s_hist *hfirst;
  struct s_split *sfirst;
  struct s_mark *m_cos,*m_tos;
  struct s_xcheck *xfirst;
  struct s_tc *tcfirst;
  struct s_usr *ufirst;
  struct s_grp *gfirst;
  struct s_msg *msgfirst;
  int cfg_cols;
  int cfg_rows;
  int debug;
  int history_min;
  int history_max;
  int confirmquit;
  int confirmcopy;
  int confirmmove;
  int confirmremove;
  int confirmaction;
  int savemarked;
  int writeconfig;
  int removelog;
  int incsubdir;
  int ignoreerror;
  int rb;
  char *rb_path;
  int rb_size;
  int hist_panel;
  int hist_mark;
  int hist_mark_action;
  int hist_ucmd;
  int hist_xinfo;
  int hist_remote;
  int hist_tc;
  int hist_setup;
  int hist_u;
  int hist_g;
  int hist_msg;
  int max_to_mark;
  int dirfirst;
  char *editor;
  char *viewer;
  // char *defedit;
  char *nodename;
  char *sysname;
  char *release;
  char *master;
  char *b_i,*b_o;
  struct s_buffer *b;
  int key_help;
  int key_info;
  int key_xinfo;
  int key_config;
  int key_setting;
  int key_up;
  int key_down;
  int key_scroll_up;
  int key_scroll_down;
  int key_left;
  int key_right;
  int key_first;
  int key_last;
  int key_next_page;
  int key_prev_page;
  int key_panel;
  int key_next_panel;
  int key_prev_panel;
  int key_dir_up;
  int key_dir_root;
  int key_dir_home;
  int key_dir_prev;
  int key_dir_work;
  int key_select;
  int key_edit;
  int key_view;
  int key_expunge;
  int key_show_mark;
  int key_show_mark_action;
  int key_add_mark;
  int key_rem_mark;
  int key_add_panel;
  int key_rem_panel;
  int key_add_ucmd;
  int key_rem_ucmd;
  int key_header;
  int key_sort_col;
  int key_sort_order;
  int key_go;
  int key_tc;
  int key_u;
  int key_g;
  int key_msg;
  int key_search;
  int key_config_1;
  int key_config_2;
  int key_config_3;
  int key_config_4;
  int key_config_5;
  int key_config_6;
  int key_config_7;
  int key_ucmd_0;
  int key_ucmd_1;
  int key_ucmd_2;
  int key_ucmd_3;
  int key_ucmd_4;
  int key_ucmd_5;
  int key_ucmd_6;
  int key_ucmd_7;
  int key_ucmd_8;
  int key_ucmd_9;
  long int lmem;
};

struct s_panel {
  char *dirname;
  char *dirnameprev;
  char *nodename;
  int local;
  int remote;
  int header;
  int ndirs;
  int nfiles;
  int nlinks;
  int ntotal;
  int sortc;
  int sorto;
  long int size;
  struct s_entry *info;
  struct s_entry *tos;
  struct s_entry *cos;
  struct s_entry *first;
  struct s_entry *last;
  struct s_remote *rem;
};

struct s_entry {
  int seqno;
  struct stat *fstat;
  struct stat *lstat;
  char *fname;
  char *fperm;
  char *lname;
  char *lperm;
  char *fusrname;
  char *fgrpname;
  char *lusrname;
  char *lgrpname;
  int uid;
  int gid;
  time_t fatime;
  time_t fmtime;
  time_t fctime;
  time_t latime;
  time_t lmtime;
  time_t lctime;
  int fmode;
  int lmode;
  int fino;
  int lino;
  int u_read;
  int u_write;
  int u_exec;
  int g_read;
  int g_write;
  int g_exec;
  int o_read;
  int o_write;
  int o_exec;
  int u_stick;
  int g_stick;
  int o_stick;
  long int size;
  int is_reg;
  int is_dir;
  int is_lnk;
  int is_chr;
  int is_blk;
  int is_fifo;
  int is_sock;
  int blocks;
  int links;
  int status_action;
  int marked;
  struct s_entry *next;
  struct s_entry *prev;
};

struct s_usr {
  int seqno;
  int uid;
  int gid;
  char *uname;
  char *pword;
  char *dir;
  char *sh;
  struct s_usr *next;
  struct s_usr *prev;
};

struct s_grp {
  int seqno;
  int gid;
  char *gname;
  struct s_grpusr *fgrpusr;
  struct s_grp *next;
  struct s_grp *prev;
};

struct s_grpusr {
  int seqno;
  char *uname;
  struct s_grpusr *next;
  struct s_grpusr *prev;
};

struct s_log {
  int seqno;
  char *ts;
  char *text;
  struct s_log *next;
  struct s_log *prev;
};

struct s_txt {
  int seqno;
  char *line;
  struct s_txt *next;
  struct s_txt *prev;
};

struct s_mark {
  int seqno;
  int panel;
  int level;
  char *dname;
  char *msg;
  struct s_entry *entry;
  struct s_mark *next;
  struct s_mark *prev;
};

struct s_split {
  int seqno;
  char *name;
  struct s_split *next;
  struct s_split *prev;
};

struct s_ucmd {
  int seqno;
  int local;
  int history;
  char key;
  char *exec;
  struct s_ucmd *next;
  struct s_ucmd *prev;
};

struct s_msg {
  int seqno;
  char *id;
  char *msg;
  struct s_msg *next;
  struct s_msg *prev;
};

struct s_remote {
  int seqno;
  int local;
  char *hostname;
  int port;
  char *username;
  char *password;
  char *dirname;
  int sockfd;
  char *dversion;
  struct s_remote *next;
  struct s_remote *prev;
};

struct s_comm_auth {
  char cmd;
  char *uname;
  char *pword;
};

struct s_hist {
  char what;
  char *dname;
  int seqno;
  struct s_hist *next;
  struct s_hist *prev;
};

struct s_buffer {
  // Common.
  int action;
  int last;
  int error;
  char username[UFU_LEN_UNAME];
  char password[UFU_LEN_PWORD];
  char dirname[UFU_LEN_NAME];
  char filename[UFU_LEN_NAME];
  char errmsg[UFU_LEN_ERROR];
  // Entry.
  int e_seqno;
  int e_ndirs;
  int e_nfiles;
  int e_nlinks;
  int e_ntotal;
  char e_fname[UFU_LEN_NAME];
  char e_fperm[UFU_LEN_PERM];
  char e_lname[UFU_LEN_NAME];
  char e_lperm[UFU_LEN_PERM];
  char e_fusername[UFU_LEN_UNAME];
  char e_fgroupname[UFU_LEN_GROUP];
  char e_lusername[UFU_LEN_UNAME];
  char e_lgroupname[UFU_LEN_GROUP];
  time_t e_fatime;
  time_t e_fmtime;
  time_t e_fctime;
  time_t e_latime;
  time_t e_lmtime;
  time_t e_lctime;
  int e_fmode;
  int e_lmode;
  int e_u_read;
  int e_u_write;
  int e_u_exec;
  int e_g_read;
  int e_g_write;
  int e_g_exec;
  int e_o_read;
  int e_o_write;
  int e_o_exec;
  long int e_size;
  int e_is_dir;
  int e_is_lnk;
  int e_blocks;
  int e_links;
  int e_fino;
  int e_lino;
  char line[UFU_LEN_MSG];
};

struct s_buffer_action {
  // Common.
  int action;
  int last;
  int error;
  char username[UFU_LEN_UNAME];
  char password[UFU_LEN_PWORD];
  char dirname[UFU_LEN_NAME];
  char filename[UFU_LEN_NAME];
  char errmsg[UFU_LEN_ERROR];
  // Action buffer.
  char line[UFU_ACTION_BUFFER];
};

struct s_xcheck {
  char *username;
  char *nodename;
  int connected;
  struct s_xcheck *next;
};

struct s_tc {
  int seqno;
  char *nodename;
  char *dir;
  char *file;
  char *alias;
  time_t date;
  struct s_entry *entry;
  struct s_tc *next;
  struct s_tc *prev;
};

