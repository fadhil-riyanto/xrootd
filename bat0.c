#include <signal.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

volatile int need_exit = 0;

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

static int open_bat0(const char* bat0dir)
{
        return open(bat0dir, O_RDONLY);
}

static char* read_bat0(int fd)
{
        off_t size = getfile_size(fd);
        char *buf = (char*)malloc(size);
        memset(buf, '\0', size);
        read(fd, buf, size);
        return buf;
}

static int close_bat0(int fd)
{
        return close(fd);
}

void signal_cb(int signal)
{
        need_exit = 1;
}

int main()
{
        signal(SIGINT, signal_cb);

        while (!need_exit) {
                int fd = open_bat0("/sys/class/power_supply/BAT0/capacity");
                char* bat0data = read_bat0(fd);

                printf("%s\n", bat0data);
                free(bat0data);

                close_bat0(fd);
                sleep(3);
        }
}