
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
#include <libnotify/notify.h>

#define debug(x) printf("%s\n", x)
#define IBUF_LEN (10 * (sizeof(struct inotify_event) + strlen(BAT0_DIR) + 1))


int sec_counter = 0;
volatile int need_exit = 0;

int charging_state = 0;


struct xorg_data {
        Display *dpy;
        int screen;
        Window root;
};

static void x_start(struct xorg_data *xorg_data)
{
        xorg_data->dpy = XOpenDisplay(NULL);
        xorg_data->screen = DefaultScreen(xorg_data->dpy);
        xorg_data->root = RootWindow(xorg_data->dpy, xorg_data->screen);
}

static void xsetroot_summon(struct xorg_data *xorg_data, char* text)
{
        XStoreName(xorg_data->dpy, xorg_data->root, text);
}

static void x_end(struct xorg_data *xorg_data)
{
        XCloseDisplay(xorg_data->dpy);
}


void signal_handler(int signal)
{
        need_exit = 1;
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

static char* get_time()
{
        time_t rawtime;
        struct tm * timeinfo;

        time (&rawtime);
        timeinfo = localtime(&rawtime);

        return asctime(timeinfo);
}

char* newline_cut(char *str, int size)
{
        char* data = (char*)malloc(size);
        memset(data, '\0', size);

        for(int i = 0 ; i < size; i++) {
                if (*(str + i) != '\n') {
                        *(data + i) = *(str + i);
                }
        }
        return data;
        
}

static void notify_exec(char *title, char* body)
{
        NotifyNotification *notif;
        notif = notify_notification_new(title, body, NULL);

        notify_notification_show(notif, NULL);
}

static void call_when_battery_full(int current)
{
        sec_counter = sec_counter + 1;
        if (sec_counter == 4) {
                if (current < 25) {
                        if (charging_state == 0)
                                notify_exec("warning", "Battery low, Insert charger now...... ASAP");
                        
                } else if (current > 95) {
                        if (charging_state == 1)
                                notify_exec("Note", "Battery full, Please unplug the cable NOW ....");
                        
                }
                sec_counter = 0;
        }
}

static int open_bat0(const char* bat0dir)
{
        return open(bat0dir, O_RDONLY);
}

static char* read_bat0(int fd, int *sizeptr)
{
        off_t size = getfile_size(fd);

        *sizeptr = size;

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

int main(int argc, char **argv, char *envp[])
{
        signal(SIGINT, signal_handler);
        notify_init("xrootdaemon");
        struct xorg_data xorg_data;

        char randombuf[100];

        int bat0size = 0;
        int bat0charging_size = 0;

        while (!need_exit) {
                int fd = open_bat0(BAT0_DIR);
                char* bat0data = read_bat0(fd, &bat0size);
                char* bat2cutted = newline_cut(bat0data, bat0size);

                int fd_charging_state = open_bat0(BAT_CHARGING_STATE_0_DIR);
                char* bat0chargingdata = read_bat0(fd_charging_state, &bat0charging_size);
                char* bat2chargingcutted = newline_cut(bat0chargingdata, bat0charging_size);

                if (strcmp(bat2chargingcutted, "Charging") == 0) {
                        charging_state = 1;
                } else {
                        charging_state = 0;
                }

                char* timestr = get_time();
                char* time = newline_cut(timestr, strlen(timestr) + 1);

                snprintf(randombuf, 100, "%s %d (%s) %s", time, atoi(bat2cutted), bat2chargingcutted, DESKTOP_NAME);

                x_start(&xorg_data);
                xsetroot_summon(&xorg_data, randombuf);
                x_end(&xorg_data);
                call_when_battery_full(atoi(bat2cutted));

                free(bat0data);
                free(bat2cutted);

                free(bat0chargingdata);
                free(bat2chargingcutted);

                free(time);
                close_bat0(fd);
                close_bat0(fd_charging_state);
                sleep(1);
        }

        notify_uninit();
        return 0;
}