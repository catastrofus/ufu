
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
#include <signal.h>
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

#include "ufud.build"

#define DEBUG						FALSE

#define	UFUD_OS_UNKNOWN				0
#define	UFUD_OS_LINUX				1
#define	UFUD_OS_HPUX				2

#define UFUD_VERSION				"Version 0 Release 0 Patchlevel 0"

#define	UFUD_USR_FILE				"/etc/passwd"
#define	UFUD_USR_FILE_SHADOW		"/etc/shadow"
#define	UFUD_GRP_FILE				"/etc/group"

#define	UFUD_LEN_UNAME				16
#define	UFUD_LEN_GROUP				16
#define	UFUD_LEN_PWORD				32
#define	UFUD_LEN_UDIR				64
#define	UFUD_LEN_USH				32
#define	UFUD_LEN_ERROR				80

#define	UFUD_LEN_UTS				128

#define	UFUD_POLLIN					1
#define	UFUD_POLLOUT				2

// Default values, to be overruled by the configfile.
#define	UFUD_PORT					4444
#define	UFUD_BACKLOG				16
#define UFUD_LOGFILE				"/var/log/ufu/ufud.log"
#define	UFUD_CFGFILE				"/etc/ufu/ufud.cfg"

#define UFUD_LEN_FNAME				96
#define UFUD_LEN_DNAME				128
#define UFUD_LEN_NAME				(UFUD_LEN_FNAME+UFUD_LEN_DNAME+1)

#define	UFUD_LEN_PERM				11
#define	UFUD_LEN_MSG				255

#define	UFUD_LEN_IPNO				18
#define	UFUD_LEN_IPNAME				64

#define	UFUD_ENTRY					0
#define	UFUD_ENTRIES				1

#define	UFUD_B_NONE					0
#define	UFUD_B_GET_VERSION			1
#define	UFUD_B_READ_DIR				2
#define	UFUD_B_READ_ENTRY			3
#define	UFUD_B_GET_TXTFILE			4
#define	UFUD_B_PUT_TXTFILE			5
#define	UFUD_B_GET_BINFILE			6
#define	UFUD_B_PUT_BINFILE			7
#define	UFUD_B_UNLINK_ENTRY			8

#define	UFUD_B_MIN				UFUD_B_NONE
#define	UFUD_B_MAX				UFUD_B_UNLINK_ENTRY

#define	UFUD_SEND				0
#define	UFUD_RECV				1

#define	UFUD_ACTION_BUFFER		8192

struct s_env {
  int os;
  char *msg;
  int port;
  int backlog;
  int debug;
  int encryption;
  int timeout;
  int timeout_retry;
  int timein;
  int timein_retry;
  pid_t ppid;
  pid_t pid;
  int auth_ip;
  char *ipno;
  char *ipname;
  char *cfgfile;
  char *logname;
  FILE *lognamefp;
  struct s_usr *ufirst;
  struct s_grp *gfirst;
  struct s_ip *ipfirst;
  int uid;
  int gid;
  char *dirname;
  int ndirs;
  int nfiles;
  int nlinks;
  int ntotal;
  struct s_entry *efirst;
};

struct s_usr {
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
  int gid;
  char *gname;
  struct s_grp *next;
  struct s_grp *prev;
};

struct s_ip {
  char *ipno;
  struct s_ip *next;
  struct s_ip *prev;
};

struct s_entry {
  int seqno;
  struct stat *fstat;
  struct stat *lstat;
  char *fname;
  char *fperm;
  char *lname;
  char *lperm;
  int fuid;
  int fgid;
  int luid;
  int lgid;
  time_t fatime;
  time_t fmtime;
  time_t fctime;
  time_t latime;
  time_t lmtime;
  time_t lctime;
  int fmode;
  int lmode;
  int u_read;
  int u_write;
  int u_exec;
  int g_read;
  int g_write;
  int g_exec;
  int o_read;
  int o_write;
  int o_exec;
  long int size;
  int blocks;
  int links;
  int fino;
  int lino;
  struct s_entry *next;
  struct s_entry *prev;
};

struct s_buffer {
  // Common.
  int action;
  int last;
  int error;
  char username[UFUD_LEN_UNAME];
  char password[UFUD_LEN_PWORD];
  char dirname[UFUD_LEN_NAME];
  char filename[UFUD_LEN_NAME];
  char errmsg[UFUD_LEN_ERROR];
  // Entry.
  int e_seqno;
  int e_ndirs;
  int e_nfiles;
  int e_nlinks;
  int e_ntotal;
  char e_fname[UFUD_LEN_NAME];
  char e_fperm[UFUD_LEN_PERM];
  char e_lname[UFUD_LEN_NAME];
  char e_lperm[UFUD_LEN_PERM];
  char e_fusername[UFUD_LEN_UNAME];
  char e_fgroupname[UFUD_LEN_GROUP];
  char e_lusername[UFUD_LEN_UNAME];
  char e_lgroupname[UFUD_LEN_GROUP];
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
  char line[UFUD_LEN_MSG];
};

struct s_buffer_action {
  // Common.
  int action;
  int last;
  int error;
  char username[UFUD_LEN_UNAME];
  char password[UFUD_LEN_PWORD];
  char dirname[UFUD_LEN_NAME];
  char filename[UFUD_LEN_NAME];
  char errmsg[UFUD_LEN_ERROR];
  // Action buffer.
  char line[UFUD_ACTION_BUFFER];
};

