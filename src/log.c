#include <stdio.h>
#include <stdint.h>
#include <ncurses.h>
#include <stdlib.h>
#include <ncurses/panel.h>

#include "6502.h"
#include "nc_ui.h"
#include "c64_colors.h"

static char *log_file = "./log-6502.txt";


static void _log(char *str) {
	const char fopen_flags[3] = "a+";	// a+ = append
        
        static FILE *log_file_ptr;

	time_t now;
	struct tm *tmnow;
        char timestr[32];

	time(&now);
	tmnow = localtime(&now);
        sprintf(timestr, "%02d.%02d.%04d %02d:%02d:%02d : ", tmnow->tm_mday, tmnow->tm_mon + 1, tmnow->tm_year + 1900, tmnow->tm_hour, tmnow->tm_min, tmnow->tm_sec);

	log_file_ptr = fopen(log_file, fopen_flags);
	if(log_file_ptr == NULL) {
		printf("error in log(), cant open logfile '%s' width flags '%s'\n", log_file, fopen_flags);
	} else {
		fprintf(log_file_ptr, "%s%s\n", timestr, str);
		fclose(log_file_ptr);
	}
		
}

void _logf(char *fmt, ...) {
	char temp[385];
	va_list args;
	va_start(args, fmt);
	vsprintf(temp, fmt, args);
	va_end(args);
	_log(temp);
}

