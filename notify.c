#include <libnotify/notify.h>

int main(void) 
{
        NotifyNotification *notif;
        notify_init("some-name");
        notif = notify_notification_new("syx", "oke", NULL);

        notify_notification_show(notif, NULL);

}