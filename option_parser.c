#include "option_parser.h"
#include <paths.h>
#include <stdlib.h>
#include <stddef.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include "core.h"
int init_option(hma_option *poption)
{
    if (!poption)
        return -1;
    memset(poption, 0, sizeof(hma_option));
    snprintf(poption->utmp.path, sizeof(poption->utmp.path), "%s", _PATH_UTMP);
    snprintf(poption->wtmp.path, sizeof(poption->wtmp.path), "%s", _PATH_WTMP);
    snprintf(poption->btmp.path, sizeof(poption->btmp.path), "%s", "/var/log/btmp");
    snprintf(poption->lastlog.path, sizeof(poption->lastlog.path), _PATH_LASTLOG);
    //snprintf(poption->name.key, sizeof(poption->name.key), "%s", getenv("USER"));
    //snprintf(poption->address.key, sizeof(poption->address.key), "");
    //snprintf(poption->time.key, sizeof(poption->time.key), "");
    return 0;
}
void list_option(hma_option *poption)
{
    printf("action: %d\n", poption->action);
    printf("=== list files ===\n");
    printf("utmp(%d), path=%s\n", poption->utmp.enable, poption->utmp.path);
    printf("wtmp(%d), path=%s\n", poption->wtmp.enable, poption->wtmp.path);
    printf("btmp(%d), path=%s\n", poption->btmp.enable, poption->btmp.path);
    printf("lastlog(%d), path=%s\n", poption->lastlog.enable, poption->lastlog.path);
    printf("=== list filters ===\n");
    printf("name(%d), key=%s\n", poption->name.enable, poption->name.key);
    printf("address(%d), key=%s\n", poption->address.enable, poption->address.key);
    printf("time(%d), key=%s\n", poption->time.enable, poption->time.key);
}
int parse_option(int argc, char **argv, hma_option *poption)
{
    static const char *OPT_STR = "uwbln:a:t:pch";
    static const struct option LONG_OPT[] = {
        {"utmp", optional_argument,  NULL, 'u'},
        {"wtmp", optional_argument,  NULL, 'w'},
        {"btmp", optional_argument,  NULL, 'b'},
        {"lastlog", optional_argument,  NULL, 'l'},
        {"name",    required_argument,  NULL, 'n'},
        {"address", required_argument,  NULL, 'a'},
        {"time", required_argument,  NULL, 't'},
        {"print", no_argument,  NULL, 'p'},
        {"clear", no_argument,  NULL, 'c'},
        {"help", no_argument,  NULL, 'h'},
        {0,0,0,0}
    };

    int valid = 0;
    int optindex = 0;
    int short_opt = 0;
    while ((short_opt = getopt_long(argc , argv, OPT_STR, LONG_OPT, &optindex)) != -1)
    {
        switch (short_opt)
        {
            case 'u':
                if (optarg) {
                    snprintf(poption->utmp.path, sizeof(poption->utmp.path), "%s", optarg);
                }
                poption->utmp.enable = 1;
                break;
            case 'w':
                if (optarg) {
                    snprintf(poption->wtmp.path, sizeof(poption->wtmp.path), "%s", optarg);
                }
                poption->wtmp.enable = 1;
                break;
            case 'b':
                if (optarg) {
                    snprintf(poption->btmp.path, sizeof(poption->btmp.path), "%s", optarg);
                }
                poption->btmp.enable = 1;
                break;
            case 'l':
                if (optarg) {
                    snprintf(poption->lastlog.path, sizeof(poption->lastlog.path), "%s", optarg);
                }
                poption->lastlog.enable = 1;
                break;
            case 'n':
                if ( -1 == name2id(optarg) ) {
                    printf("invalid user %s\n", optarg);
                    valid = -1;
                } else {
                    snprintf(poption->name.key, sizeof(poption->name.key), "%s", optarg);
                    poption->name.enable = 1;
                }
                break;
            case 'a':
                snprintf(poption->address.key, sizeof(poption->address.key), "%s", optarg);
                poption->address.enable = 1;
                break;
            case 't':
                if ( -1 == str2seconds(optarg) ) {
                    printf("invalid timestamp %s\n", optarg);
                    valid = -1;
                } else {
                    snprintf(poption->time.key, sizeof(poption->time.key), "%s", optarg);
                    poption->time.enable = 1;
                }
                break;
            case 'p':
                poption->action = ACTION_PRINT;
                break;
            case 'c':
                poption->action = ACTION_CLEAR;
                break;
            case 'h':
                usage();
                valid = -2;
                break;
            default:
                //printf("error option %s\n", optarg);
                valid = -1;
                break;
        }
    }
    return valid;
}

void usage()
{
    const static char *usage = "\
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\
轻轻地我走了     正如给我轻轻地来                  \n\
        我轻轻地挥手     作别西边的云彩            \n\
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\
Usage: ./hidemyass [ENTRIES] [FILTERS] ACTIONS        \n\
ENTRIES:                                \n\
  -u, --utmp=utmp_file                \n\
      specify the path to utmp file, which is /var/run/utmp by default \n\
      utmp file is read by 'who','w' and other commands \n\
  -w, --wtmp=wtmp_file                \n\
      specify the path to wtmp file, which is /var/log/wtmp by default \n\
      wtmp is read by 'last' and other commands\n\
  -b, --btmp=btmp_file                \n\
      specify the path to btmp file, which is /var/log/btmp by default \n\
      btmp is read by 'lastb' and other commands\n\
  -l, --lastlog=lastlog_file          \n\
      specify the path to lastlog file, which is /var/log/lastlog by default \n\
      lastlog is read by 'lastlog' and other commands\n\
FILTERS:                                \n\
  -n, --name=username                   \n\
      specify log record by username \n\
  -a, --address=host                    \n\
      specify log record by host ip address  \n\
  -t, --time=time                       \n\
      specify log record by time (YYYY:MM:DD:HH:MM:SS) \n\
ACTIONS: \n\
  -p, --print                           \n\
      print records for specified ENTRIES \n\
  -c, --clear                           \n\
      clear records for specified ENTRIES with FILTERS\n\
      usually you need permission to edit log \n\
  -h, --help                            \n\
      show this message and exit\n\
 ";               
    fprintf(stderr,"%s\n",usage);                       
}
