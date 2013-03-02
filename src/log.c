#include <stdio.h>
#include <stdint.h>
#include <ncurses.h>
#include <stdlib.h>
#include <panel.h>

#include "6502.h"
#include "nc_ui.h"
#include "c64_colors.h"

#define LOG_CONSOLE 0x00000001
#define LOG_CURSES  0x00000002	// TODO
#define LOG_FILE    0x00000004
#define LOG_GFX     0x00000008	// TODO

static uint32_t log_config = LOG_CONSOLE | LOG_FILE | LOG_CURSES;
static char *log_file = "./log-6502.txt";


void _log(char *str) {
	static char fopen_flags[3] = "a+";	// a+ = append
        
        static FILE *log_file_ptr;

	time_t now;
	struct tm *tmnow;
        char timestr[32];

	time(&now);
	tmnow = localtime(&now);
        sprintf(timestr, "%02d.%02d.%04d %02d:%02d:%02d : ", tmnow->tm_mday, tmnow->tm_mon + 1, tmnow->tm_year + 1900, tmnow->tm_hour, tmnow->tm_min, tmnow->tm_sec);

	if(log_config & LOG_CONSOLE) printf("%s\n", str);
	if(log_config & LOG_FILE) {
		log_file_ptr = fopen(log_file, fopen_flags);
		if(log_file_ptr == NULL) {
			printf("error in log(), cant open logfile '%s' width flags '%s'\n", log_file, fopen_flags);
		} else {
			fprintf(log_file_ptr, "%s%s\n", timestr, str);
			fclose(log_file_ptr);
		}
		
	}
}

