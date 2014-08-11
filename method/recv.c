#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include <unistd.h>

static void reply_to_method_call(DBusMessage *msg, DBusConnection *conn)
{
	DBusMessage *reply;
	DBusMessageIter arg;
	char *param = NULL;
	dbus_bool_t stat = TRUE;
	dbus_uint32_t level = 2014;
	dbus_uint32_t serial = 0;

	if (!dbus_message_iter_init(msg, &arg)) {
		printf("Message has no args\n");
	} else if (dbus_message_iter_get_arg_type(&arg) != DBUS_TYPE_STRING) {
		printf("Arg is not string!\n");
	} else {
		dbus_message_iter_get_basic(&arg, &param);
		if(param)
			printf("Get param: %s\n", param);
	}

	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append(reply, &arg);

	if(!dbus_message_iter_append_basic(&arg, DBUS_TYPE_BOOLEAN, &stat))
	{
		printf("Out of Memory!\n");
		exit(1);
	}

	if(!dbus_message_iter_append_basic(&arg, DBUS_TYPE_UINT32, &level))
	{
		printf("Out of Memory!\n");
		exit(1);
	}

	if(!dbus_connection_send(conn, reply, &serial)) {
		printf("Out of Memory\n");
		exit(1);
	}

	dbus_connection_flush(conn);
	dbus_message_unref(reply);
}

static void listen_dbus()
{
	DBusMessage * msg;
	DBusMessageIter arg;
	DBusConnection * connection;
	DBusError err;
	int ret;
	char *sigvalue;

	dbus_error_init(&err);

	connection = dbus_bus_get(DBUS_BUS_SESSION, &err);
	if (dbus_error_is_set(&err) || NULL == connection)
	{
		fprintf(stderr,"Connection Error %s\n",err.message);
		dbus_error_free(&err);
		return;
	}

	ret = dbus_bus_request_name(connection, "test.wei.dest", DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
	if (dbus_error_is_set(&err) || ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
		fprintf(stderr,"Name Error %s\n",err.message);
		dbus_error_free(&err);
		return;
	}

	dbus_bus_add_match(connection, "type='signal',interface='test.signal.Type'", &err);
	dbus_connection_flush(connection);

	if(dbus_error_is_set(&err)){
		fprintf(stderr,"Match Error %s\n",err.message);
		dbus_error_free(&err);
	}

	while(1)
	{
		dbus_connection_read_write(connection, 0);
		msg = dbus_connection_pop_message(connection);

		if(msg == NULL)
		{
			sleep(1);
			continue;
		}

		if (dbus_message_is_signal(msg, "test.signal.Type", "Test")) {

			if (!dbus_message_iter_init(msg, &arg)) {
				fprintf(stderr, "Message Has no param\n");
			} else if (dbus_message_iter_get_arg_type(&arg) != DBUS_TYPE_STRING) {
				fprintf(stderr, "param is not string\n");
			} else {
				dbus_message_iter_get_basic(&arg,&sigvalue);
			}

		} else if (dbus_message_is_method_call(msg, "test.method.Type", "Method")) {

			if (!strcmp(dbus_message_get_path(msg), "/test/method/Object"))
			{
				reply_to_method_call(msg, connection);
			}
		}

		dbus_message_unref(msg);
	}


}

int main( int argc , char *argv[])
{
	listen_dbus();

	return 0;
}
