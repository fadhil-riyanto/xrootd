#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>

#define debug(x) printf("%s\n", x)

static char* get_time()
{
        time_t rawtime;
        struct tm * timeinfo;

        time (&rawtime);
        timeinfo = localtime(&rawtime);

        return asctime(timeinfo)
}

int main(void)
{
        get_time();
        return 0;
}