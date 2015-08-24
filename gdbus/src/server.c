#include <stdio.h>
#include <glib.h>
#include <gio/gio.h>

#include "myapp-generated.h"

#define G_DBUS_NAME "net.Corp.MyApp.Frobber"

static gboolean s_handle_hello_world(MyAppNetCorpMyAppFrobber *object,
        GDBusMethodInvocation *invocation,
        gchar *arg)
{
    my_app_net_corp_my_app_frobber_complete_hello_world(object, invocation, arg);

    printf("\033[31m jolin log   : \033[0m" "%s %s %d\n", __FILE__, __func__, __LINE__);
    return TRUE;
}

static void on_bus_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data)
{
    MyAppNetCorpMyAppFrobber *object = user_data;

    printf("\033[31m jolin log   : \033[0m" "%s %s %d\n", __FILE__, __func__, __LINE__);

    g_signal_connect(object, "handle-hello-world", G_CALLBACK(s_handle_hello_world), user_data);
    g_dbus_interface_skeleton_export((GDBusInterfaceSkeleton *)object,
            connection, "/net/Corp/MyApp/Frobber", NULL);

    printf("\033[31m jolin log   : \033[0m" "%s %s %d\n", __FILE__, __func__, __LINE__);
}

static void on_name_acquired(GDBusConnection *connection,const gchar *name, gpointer user_data)
{
    printf("\033[31m jolin log   : \033[0m" "%s %s %d\n", __FILE__, __func__, __LINE__);
}

static void on_name_lost(GDBusConnection *connection,const gchar *name, gpointer user_data)
{
    printf("\033[31m jolin log   : \033[0m" "%s %s %d\n", __FILE__, __func__, __LINE__);
}

int main(int argc, char *argv[])
{
    MyAppNetCorpMyAppFrobber *myAppObject;
    GMainLoop *mainLoop;
    guint owner_id = 0;

    g_type_init();

    myAppObject = my_app_net_corp_my_app_frobber_skeleton_new();

    owner_id = g_bus_own_name(G_BUS_TYPE_SESSION,
            "net.Corp.MyApp.Frobber",
            G_BUS_NAME_OWNER_FLAGS_NONE,
            on_bus_acquired,
            on_name_acquired,
            on_name_lost,
            myAppObject,
            NULL);

    mainLoop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(mainLoop);
    g_bus_unown_name(owner_id);

    return 0;
}
