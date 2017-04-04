#ifndef HIDEMYASS_CORE_H
#define HIDEMYASS_CORE_H

// shut up compiler warning
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#include <time.h>
#endif
#include <sys/stat.h>

#include "option_parser.h"

#define TIME_FORMAT "%Y:%m:%d:%H:%M:%S"
#define XTMP_FORMAT "%-10s%-10s%-10s%-20s%-10s\n"
#define LLOG_FORMAT "%-15s%-10s%-15s%-10s\n"
const char *seconds2str(time_t seconds);
time_t str2seconds(const char *strtime);
int name2id(const char *username);
int restore_time(const char *filepath, struct stat *saved_stat);

int print_xtmp(const char *filepath);
int print_lastlog(const char *filepath, const char *username);
int print_byopt(hma_option *poption);


int clear_xtmp(const char *filepath, hma_option *poption);
int clear_lastlog(const char *filepath, hma_option *poption);
int clear_byopt(hma_option *poption);
#endif
