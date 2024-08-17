
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
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xmu/CurUtil.h>
#include <X11/Xcursor/Xcursor.h>

#define debug(x) printf("%s\n", x)
#define IBUF_LEN (10 * (sizeof(struct inotify_event) + strlen(BAT0_DIR) + 1))

volatile int need_exit = 0;

struct gc_data {
        int             *openfd;
        char            *bat0bufptr;
        off_t            file_sz;
};

struct xorg_data {
        Display *dpy;
        int screen;
        Window root;
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

static void exec_notify_send(struct gc_data *gc, char** envp, char* text)
{

        char* args[] = {
                NOTIFY_SEND_PATH, text, NULL
        };

        execve(NOTIFY_SEND_PATH, args, envp);
}


static void exec_xsetroot(struct gc_data *gc, char** envp, char* text)
{

        char* args[] = {
                XSETROOT_PATH, "-name", text, NULL
        };

        execve(XSETROOT_PATH, args, envp);
}

int main(int argc, char **argv, char *envp[])
{
        signal(SIGINT, signal_handler);
        struct gc_data gc;

        char randombuf[100];

        setup_fd(&gc);

        while (!need_exit) {
                memset(randombuf, '\0', 100);

                get_bat_num(&gc);
                char* battery = newline_cut(gc.bat0bufptr, &gc);
                char* time = newline_cut(get_time(), &gc);
                
                snprintf(randombuf, 100, "%s %s %s", time, battery, DESKTOP_NAME);
                
                exec_xsetroot(&gc, envp, randombuf);
                
                free(battery);
                free(time);

                sleep(1);
        }
        

        run_garbage_collector_clear(&gc);
        return 0;
}