#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xmu/CurUtil.h>
#include <X11/Xcursor/Xcursor.h>
#include <time.h>


void xsetroot(char* text)
{
        static Display *dpy;
        static int screen;

        static Window root;

        dpy = XOpenDisplay(NULL);
        screen = DefaultScreen(dpy);
        root = RootWindow(dpy, screen);

        XStoreName(dpy, root, "test aaaa name");

        XCloseDisplay(dpy);
}