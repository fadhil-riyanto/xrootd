
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



static void get_bat_num(struct gc_data *gc)
{
        read(*gc->openfd, gc->bat0bufptr, gc->file_sz);
}


// static void run_thread()
// {
//         pthread_t *
// }


int main(void)
{
        signal(SIGINT, signal_handler);
        struct gc_data gc;

        setup_fd(&gc);

        get_bat_num(&gc);

        printf("%s\n", gc.bat0bufptr);

        run_garbage_collector_clear(&gc);
        return 0;
}