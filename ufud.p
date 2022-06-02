
struct s_env *ufud_alloc_env(void);
char *ufud_alloc_string(struct s_env *,int);
struct s_usr *ufud_alloc_usr(struct s_env *);
struct s_grp *ufud_alloc_grp(struct s_env *);
struct s_ip *ufud_alloc_ip(struct s_env *);
struct s_buffer *ufud_alloc_buffer(struct s_env *,char *);
struct s_buffer_action *ufud_alloc_buffer_action(struct s_env *,char *);
struct s_entry *ufud_alloc_entry(struct s_env *);
struct dirent *ufud_alloc_dirent(struct s_env *);
struct stat *ufud_alloc_stat(struct s_env *);

void ufud_free_env(struct s_env *);
void ufud_free_usr(struct s_env *,struct s_usr *);
void ufud_free_grp(struct s_env *,struct s_grp *);
void ufud_free_ip(struct s_env *,struct s_ip *);
void ufud_free_buffer(struct s_env *,struct s_buffer *,char *);
void ufud_free_buffer_action(struct s_env *,struct s_buffer_action *,char *);
void ufud_free_entries(struct s_env *,char *);
void ufud_free_entry(struct s_env *,struct s_entry *,char *);
void ufud_free_dirent(struct s_env *,struct dirent *);
void ufud_free_stat(struct s_env *,struct stat *);
void ufud_free_string(struct s_env *,char *);

void ufud_open_log(struct s_env *);
void ufud_close_log(struct s_env *);
void ufud_log(struct s_env *);

void ufud_read_config(struct s_env *);

int ufud_read_entries(struct s_env *,struct s_buffer *,int,char *);
int ufud_read_entry(struct s_env *,struct s_buffer *,int,char *);
void ufud_stat_entry(struct s_env *,struct s_entry *,int);
int ufud_get_version(struct s_env *,struct s_buffer *,int,char *);

int ufud_get_txtfile(struct s_env *,struct s_buffer *,int,char *);
int ufud_put_txtfile(struct s_env *,struct s_buffer *,int,char *);

int ufud_get_binfile(struct s_env *,struct s_buffer *,int,char *);
int ufud_put_binfile(struct s_env *,struct s_buffer *,int,char *);

int ufud_unlink_entry(struct s_env *,struct s_buffer *,int,char *);

void ufud_cdate(struct s_env *);
void ufud_sdate(struct s_env *,time_t *);
char *ufud_concat_dir(struct s_env *,char *,char *,char *);
char *ufud_clear_string(struct s_env *,char *,int);
void ufud_resize_string(struct s_env *,char *,int,char *);
char *ufud_lower(struct s_env *,char *);

int ufud_setuid(struct s_env *,int,char *);
int ufud_setgid(struct s_env *,int,char *);

int ufud_poll(struct s_env *,int,int,char *);
void ufud_errormsg(struct s_env *,int,int,char *);
int ufud_send(struct s_env *,struct s_buffer *,unsigned int,char *);
int ufud_recv(struct s_env *,struct s_buffer *,unsigned int,char *);
int ufud_send_action(struct s_env *,struct s_buffer_action *,unsigned int,char *);
int ufud_recv_action(struct s_env *,struct s_buffer_action *,unsigned int,char *);

void ufud_add_ip(struct s_env *,char *);
int ufud_auth_ip(struct s_env *,char *);
void ufud_free_ips(struct s_env *);

void ufud_read_users(struct s_env *);
void ufud_read_groups(struct s_env *);
void ufud_free_users(struct s_env *);
void ufud_free_groups(struct s_env *);
int ufud_check_user(struct s_env *,struct s_buffer *,char *);
char *ufud_search_user(struct s_env *,int,char *);
char *ufud_search_group(struct s_env *,int,char *);

void sigchld_handler(int);

