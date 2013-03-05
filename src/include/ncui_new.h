#ifndef __NCUI_NEW_H__
#define __NCUI_NEW_H__


extern int ncui();

/* only for interal use */
static int init_ncui();
static int deinit_ncui();
static int window_create_main();
static int window_update_main();

#endif // __NCUI_NEW_H__
