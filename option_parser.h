#ifndef HIDEMYASS_OPTION_PARSER_H
#define HIDEMYASS_OPTION_PARSER_H

#include <linux/limits.h>
typedef struct hma_option hma_option;
typedef struct logfile logfile;
typedef struct filter filter;
typedef enum action_type action_type;

enum action_type{
    ACTION_EMPTY = 0,
    ACTION_PRINT,
    ACTION_CLEAR
};
struct logfile {
    char path[PATH_MAX];
    int enable;
};
struct filter {
    char key[NAME_MAX];
    int enable;
};

struct hma_option {
    action_type action;

    logfile utmp;
    logfile wtmp;
    logfile btmp;
    logfile lastlog;

    filter name;
    filter address;
    filter time;
};

int init_option(hma_option *poption);
int parse_option(int argc, char **argv, hma_option *poption);
void list_option(hma_option *poption);
void usage();
#endif
