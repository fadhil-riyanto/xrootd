
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/inotify.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <string.h>
#include "config.h"
#include <stdlib.h>

#define debug(x) printf("%s\n", x)
#define IBUF_LEN (10 * (sizeof(struct inotify_event) + strlen(BAT0_DIR) + 1))

volatile int need_exit = 0;

struct gc_data {
        int             *openfd;
        char            *bat0bufptr;
        off_t            file_sz;
};

void signal_handler(int signal)
{
        need_exit = 1;
}

static inline void run_garbage_collector_clear(struct gc_data *gc)
{
        /* close(*gc->epfd); */
        close(*gc->openfd);
        free(gc->bat0bufptr);
}

static __off_t getfile_size(int fd)
{
        struct stat st;
        int ret;

        ret = fstat(fd, &st);
        if (ret == -1) {
                perror("fstat");
                return -1;
        }

        return st.st_size;
}

static inline int setup_fd(struct gc_data *gc)
{
        int ret;
        ret = open(BAT0_DIR, O_RDONLY);

        if (ret == -1) {
                perror("open");
                return -1;
        }

        gc->openfd = &ret;

        off_t size = getfile_size(*gc->openfd);
        gc->bat0bufptr = (char*)malloc(size * sizeof(char));

        gc->file_sz = size;
}

static char* get_time()
{
        time_t rawtime;
        struct tm * timeinfo;

        time (&rawtime);
        timeinfo = localtime(&rawtime);

        return asctime(timeinfo);
}

char* newline_cut(char *str, struct gc_data *gc)
{
        char* data = (char*)malloc(gc->file_sz);
        memset(data, '\0', gc->file_sz);

        for(int i = 0 ; i < gc->file_sz; i++) {
                if (*(str + i) != '\n') {
                        *(data + i) = *(str + i);
                }
        }
        return data;
        
}

static void get_bat_num(struct gc_data *gc)
{
        read(*gc->openfd, gc->bat0bufptr, gc->file_sz);
}

static void exec(struct gc_data *gc, char* bat0)
{
        int len = 2 + 1 + gc->file_sz;

        char* snprintfbuf = (char*)malloc(len);
        memset(snprintfbuf, '\0', len);

        snprintf(snprintfbuf, len, "%s", bat0);

        char* args[] = {
                NOTIFY_SEND_PATH, snprintfbuf, NULL
        };

        char *env[] = { 
                "DISPLAY=:0.0", 
                "DBUS_SESSION_BUS_ADDRESS=unix:path=/tmp/dbus-YY3FKxW0FU",
                "guid=89e1272fd6c1e0b8c15834a666c048cc",
                "DBUS_SESSION_BUS_PID=68930", NULL
         };

        execve(NOTIFY_SEND_PATH, args, env);

        free(snprintfbuf);
}


int main(void)
{
        signal(SIGINT, signal_handler);
        struct gc_data gc;

        setup_fd(&gc);

        // while (!need_exit) {
        //         get_bat_num(&gc);
        //         char* cropped = newline_cut(gc.bat0bufptr, &gc);
                
        //         free(cropped);
        // }
        exec(&gc, "test test test");

        run_garbage_collector_clear(&gc);
        return 0;
}