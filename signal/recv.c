#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include <unistd.h>

static void listen_signal()
{
	DBusError err;
	DBusMessageIter arg;

	DBusMessage * msg;
	DBusConnection * connection;
	int ret;
	char *sigvalue;

	dbus_error_init(&err);
	connection = dbus_bus_get(DBUS_BUS_SESSION, &err);
	if(dbus_error_is_set(&err) || NULL == connection)
	{
		fprintf(stderr,"Connection Error %s\n",err.message);
		dbus_error_free(&err);
		return;
	}

	ret = dbus_bus_request_name(connection, "test.singal.dest", \
			DBUS_NAME_FLAG_REPLACE_EXISTING,&err);
	if(dbus_error_is_set(&err) || ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
	{
		fprintf(stderr,"Name Error %s\n",err.message);
		dbus_error_free(&err);
		return;
	}

	dbus_bus_add_match(connection, "type='signal',interface='test.signal.Type'", &err);
	dbus_connection_flush(connection);
	if(dbus_error_is_set(&err))
	{
		fprintf(stderr,"Match Error %s\n",err.message);
		dbus_error_free(&err);
	}

	while(1)
	{
		dbus_connection_read_write(connection,0);
		msg = dbus_connection_pop_message(connection);
		if(msg == NULL)
		{
			sleep(1);
			continue;
		}

		if (dbus_message_is_signal(msg, "test.signal.Type", "Test"))
		{
			if(!dbus_message_iter_init(msg,&arg) )
				fprintf(stderr, "Message Has no Param\n");
			else if(dbus_message_iter_get_arg_type(&arg) != DBUS_TYPE_STRING)
				printf("Param is not string\n");
			else
				dbus_message_iter_get_basic(&arg, &sigvalue);
			printf("Got Singal with value : %s\n",sigvalue);
		}
		dbus_message_unref(msg);
	}

}

int main(int argc, char *argv[])
{
	listen_signal();

	return 0;
}
