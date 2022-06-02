
void ufu_msg(struct s_env *);
void ufu_alert(struct s_env *);
void ufu_logo(struct s_env *);
void ufu_logo_name(struct s_env *,int);
void ufu_any_key(struct s_env *);
void ufu_wrong_key(struct s_env *);
int ufu_get_key(struct s_env *,int,char *);
int ufu_get_yn(struct s_env *);
int ufu_get_y(struct s_env *);
void ufu_cdate(struct s_env *);
void ufu_sdate(struct s_env *,time_t *);
char *ufu_concat_dir(struct s_env *,char *,char *,char *);
int ufu_exec_ext(struct s_env *,int,char *,char *,char *);
char *ufu_clear_string(struct s_env *,char *,int);
int ufu_display_header(struct s_env *);
void ufu_display_size(struct s_env *,long int,int,char *);
void ufu_resize_string(struct s_env *,char *,int,char *);
void ufu_goto_seqno(struct s_env *,char,int,int);
void ufu_goto_fname(struct s_env *,char,int,char *);
char *ufu_lower(struct s_env *,char *);
int ufu_direxist(struct s_env *,char *);
int ufu_set_action_status(struct s_env *,char *,char *,int);

void ufu_not_implemented(struct s_env *);
int ufu_get_arg(int,char **);
int ufu_get_rows(struct s_env *);
int ufu_get_cols(struct s_env *);

void ufu_cmd_view(struct s_env *,char *,char *);
void ufu_cmd_edit(struct s_env *,char *,char *);
void ufu_cmd_exec(struct s_env *,int,char *);
void ufu_cmd_exec_main(struct s_env *,int,char);
void ufu_cmd_exec_key(struct s_env *,int,int,char *);

void ufu_clear(struct s_env *);
void ufu_wclear(WINDOW *);
void ufu_wrefresh(WINDOW *);
WINDOW *ufu_popup(struct s_env *,int,int,int,int,char *);
void ufu_popdown(struct s_env *,WINDOW *);

void ufu_open_log(struct s_env *);
void ufu_close_log(struct s_env *);
void ufu_log(struct s_env *);
void ufu_dbg(struct s_env *);
void ufu_wai(struct s_env *);
int ufu_read_log(struct s_env *);
void ufu_view_log(struct s_env *);
int ufu_search_log(struct s_env *,int,char *,struct s_log *);

int ufu_read_config(struct s_env *,int,char *,int,char **);
void ufu_write_config(struct s_env *);
void ufu_show_setting(struct s_env *);
void ufu_show_config(struct s_env *);
void ufu_findext(struct s_env *,char *,char *);

void ufu_read_panels(struct s_env *);
int ufu_show_panels(struct s_env *,int);
int ufu_select_panel(struct s_env *,int);

int ufu_read_entries(struct s_env *,int);
void ufu_size_entry(struct s_env *,int,struct s_entry *);
void ufu_stat_entry(struct s_env *,int,struct s_entry *);
void ufu_info_entry(struct s_env *,struct s_entry *,int,struct s_mark *,struct s_panel *,int);
void ufu_xinfo_entry(struct s_env *,int,char *,int);
struct s_entry *ufu_show_entries(struct s_env *,int,int,int);

void ufu_sort(struct s_env *,int);
void ufu_presort(struct s_env *,int);
void ufu_bubblesort(struct s_env *,int,int,int,int);
int ufu_bubblesort_entries(struct s_env *,int,struct s_entry *,struct s_entry *);
void ufu_sort_swap(struct s_env *,struct s_entry *,struct s_entry *);

int ufu_mark_level(struct s_env *,char *);
int ufu_try2mark(struct s_env *,int,struct s_entry *);
int ufu_can_mark(struct s_env *,struct s_entry *);
int ufu_mark(struct s_env *,int,char *,struct s_entry *);
void ufu_rem_mark_from_panel(struct s_env *,int);
void ufu_copy_marked(struct s_env *,struct s_mark *, struct s_entry *);
int ufu_rem_mark(struct s_env *,struct s_mark *,int,char *,char *);
void ufu_show_mark(struct s_env *);
void ufu_expunge_mark(struct s_env *);
int ufu_is_marked(struct s_env *,int,struct s_entry *);
void ufu_find_entry_from_mark(struct s_env *,struct s_mark *,int);

int ufu_edit_ucmd(struct s_env *,struct s_ucmd *,int,int,int,int);
void ufu_add_ucmd(struct s_env *,struct s_ucmd *);
int ufu_rem_ucmd(struct s_env *,struct s_ucmd *);
void ufu_show_ucmd(struct s_env *,int,char *);
void ufu_show_txt(struct s_env *,int,char *);
void ufu_write_ucmd(struct s_env *);
void ufu_renum_ucmd(struct s_env *);

int ufu_split_remote(struct s_env *,struct s_remote *,char *,int);
int ufu_edit_remote(struct s_env *,struct s_remote *,int);
void ufu_add_remote(struct s_env *,struct s_remote *,int);
int ufu_rem_remote(struct s_env *,struct s_remote *);
void ufu_free_remote_hosts(struct s_env *);
void ufu_show_remote(struct s_env *,char *);
void ufu_set_pwd(struct s_env *,struct s_remote *);

void ufu_main(struct s_env *);
void ufu_help(struct s_env *,int,int);

void ufu_init_curses(struct s_env *);
void ufu_deinit_curses(struct s_env *);

void ufu_mod_lmem(struct s_env *,long int);
struct s_env *ufu_alloc_env(int,char **);
struct s_panel *ufu_alloc_panel(struct s_env *,int);
struct s_entry *ufu_alloc_entry(struct s_env *);
struct s_split *ufu_alloc_split(struct s_env *,int);
struct s_trash *ufu_alloc_trash(struct s_env *);
char *ufu_alloc_string(struct s_env *,int);
struct dirent *ufu_alloc_dirent(struct s_env *);
struct stat *ufu_alloc_stat(struct s_env *);
struct s_usr *ufu_alloc_usr(struct s_env *);
struct s_grp *ufu_alloc_grp(struct s_env *);
struct s_grpusr *ufu_alloc_grpusr(struct s_env *);
struct s_msg *ufu_alloc_msg(struct s_env *);
struct s_log *ufu_alloc_log(struct s_env *,int);
struct s_txt *ufu_alloc_txt(struct s_env *);
struct s_mark *ufu_alloc_mark(struct s_env *);
struct s_ucmd *ufu_alloc_ucmd(struct s_env *);
struct s_remote *ufu_alloc_remote(struct s_env *);
struct s_hist *ufu_alloc_hist(struct s_env *);
struct s_xcheck *ufu_alloc_xcheck(struct s_env *);
struct s_tc *ufu_alloc_tc(struct s_env *);
struct s_buffer *ufu_alloc_buffer(struct s_env *,int);
struct s_buffer_action *ufu_alloc_buffer_action(struct s_env *,int);

void ufu_free_env(struct s_env *);
void ufu_free_panel(struct s_env *,int);
void ufu_free_entries(struct s_env *,int);
void ufu_free_entry(struct s_env *,struct s_entry *);
void ufu_free_split(struct s_env *,struct s_split *);
void ufu_free_trash(struct s_env *,struct s_trash *);
void ufu_free_string(struct s_env *,char *);
void ufu_free_dirent(struct s_env *,struct dirent *);
void ufu_free_stat(struct s_env *,struct stat *);
void ufu_free_usr(struct s_env *,struct s_usr *);
void ufu_free_grp(struct s_env *,struct s_grp *);
void ufu_free_grpusr(struct s_env *,struct s_grpusr *);
void ufu_free_msg(struct s_env *,struct s_msg *);
void ufu_free_log(struct s_env *,struct s_log *);
void ufu_free_txt(struct s_env *,struct s_txt *);
void ufu_free_mark(struct s_env *,struct s_mark *);
void ufu_free_ucmd(struct s_env *,struct s_ucmd *);
void ufu_free_remote(struct s_env *,struct s_remote *);
void ufu_free_hist(struct s_env *,struct s_hist *);
void ufu_free_xcheck(struct s_env *,struct s_xcheck *);
void ufu_free_tc(struct s_env *,struct s_tc *);
void ufu_free_buffer(struct s_env *,struct s_buffer *,int);
void ufu_free_buffer_action(struct s_env *,struct s_buffer_action *,int);

int ufu_poll(struct s_env *,struct s_remote *,int);
void ufu_errormsg(struct s_env *,int,int);
int ufu_send(struct s_env *,struct s_remote *,struct s_buffer *);
int ufu_recv(struct s_env *,struct s_remote *,struct s_buffer *);
int ufu_send_action(struct s_env *,struct s_remote *,struct s_buffer_action *);
int ufu_recv_action(struct s_env *,struct s_remote *,struct s_buffer_action *);

void ufu_crypt_passwd(struct s_env *,struct s_buffer *,struct s_remote *);
int ufu_com_read_dir(struct s_env *,char *,int);
int ufu_com_read_entry(struct s_env *,char *,int,struct s_entry *);
int ufu_com_get_txtfile(struct s_env *,char *,char *,int,int);
int ufu_com_get_version(struct s_env *,struct s_remote *);
int ufu_com_get_binfile(struct s_env *,char *,char *,int,int);
int ufu_com_put_binfile(struct s_env *,char *,char *,int,int);

void ufu_read_users(struct s_env *);
void ufu_read_groups(struct s_env *);
void ufu_free_users(struct s_env *);
void ufu_free_groups(struct s_env *);
char *ufu_search_user(struct s_env *,int);
int ufu_search_user_gid(struct s_env *,int);
char *ufu_search_group(struct s_env *,int);
int ufu_search_uid(struct s_env *env,char *);
int ufu_search_gid(struct s_env *env,char *);

void ufu_show_u(struct s_env *);
void ufu_sort_u(struct s_env *,int);
void ufu_sort_u_swap(struct s_env *,struct s_usr *, struct s_usr *);
void ufu_show_g(struct s_env *);
void ufu_sort_g(struct s_env *,int);
void ufu_sort_g_swap(struct s_env *,struct s_grp *, struct s_grp *);

void ufu_read_msg(struct s_env *);
void ufu_show_msg(struct s_env *);
void ufu_sort_msg(struct s_env *);
char *ufu_find_msg(struct s_env *,char *); 

void ufu_show_tc(struct s_env *);
int ufu_read_tc(struct s_env *);
int ufu_write_tc(struct s_env *);

int ufu_rl(struct s_env *,WINDOW *,int,int,int,int,int,char *,int,int);
int ufu_rl_insert(struct s_env *,char *,char,int,int);
int ufu_rl_delete(struct s_env *,char *,int);

void ufu_split_buffer(struct s_env *);
int ufu_receive_buffer(struct s_env *,unsigned int);
int ufu_send_buffer(struct s_env *,unsigned int);

int ufu_com_init(struct s_env *,struct s_remote *);
int ufu_com_exit(struct s_env *,struct s_remote *);

void ufu_add_hist(struct s_env *,char,char *,int);
int ufu_find_hist(struct s_env *,char,char *);
void ufu_clean_hist(struct s_env *);
