#ifndef __NCUI_NEW_H__
#define __NCUI_NEW_H__

#define SH_COMMAND	"/bin/bash"

extern int ncui();

/* only for interal use */
static int get_system_shell(char *cmd_str);
static int init_ncui();
static int deinit_ncui();
static int window_create_main();
static int window_update_main();

#endif // __NCUI_NEW_H__
