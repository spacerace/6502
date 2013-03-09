#ifndef __NCUI_NEW_H__
#define __NCUI_NEW_H__

#define SH_COMMAND	"/bin/bash"

extern int ncui();

/* only for interal use */
int get_system_shell(char *cmd_str);
int init_ncui();
int deinit_ncui();
int window_create_main();
int window_update_main();

#endif // __NCUI_NEW_H__
