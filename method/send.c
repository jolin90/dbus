#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include <unistd.h>

DBusConnection *connect_dbus()
{
	DBusError err;
	DBusConnection * connection;
	int ret;

	/* initialise the erroes */
	dbus_error_init(&err);

	connection = dbus_bus_get(DBUS_BUS_SESSION, &err);
	if(dbus_error_is_set(&err) || NULL == connection)
	{
		fprintf(stderr, "Connection Err : %s\n", err.message);
		dbus_error_free(&err);
		return NULL;
	}

	ret = dbus_bus_request_name(connection, "test.wei.source",DBUS_NAME_FLAG_REPLACE_EXISTING,&err);
	if (dbus_error_is_set(&err) || ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) 
	{
		fprintf(stderr,"Name Err : %s\n", err.message);
		dbus_error_free(&err);
		return NULL;
	}

	return connection;
}

void send_a_method_call(DBusConnection * connection,char * param)
{
	DBusError err;
	DBusMessageIter arg;
	DBusMessage *msg;
	DBusPendingCall *pending;
	dbus_bool_t stat;
	dbus_uint32_t level;

	dbus_error_init(&err);

	msg = dbus_message_new_method_call("test.wei.dest", "/test/method/Object", "test.method.Type", "Method");
	if(msg == NULL) {
		printf("Message NULL\n");
		return;
	}

	dbus_message_iter_init_append(msg, &arg);
	if(!dbus_message_iter_append_basic(&arg, DBUS_TYPE_STRING, &param))
	{
		printf("Out of Memory!\n");
		exit(1);
	}

	if(!dbus_connection_send_with_reply(connection, msg, &pending, -1)){
		printf("Out of Memory!\n");
		exit(1);
	}

	if(pending == NULL){
		printf("Pending Call NULL: connection is disconnected \n");
		dbus_message_unref(msg);
		return;
	}

	dbus_connection_flush(connection);
	dbus_message_unref(msg);

	// block until we recieve a reply, Block until the pending call is completed.
	dbus_pending_call_block(pending);

	// get the reply message,Gets the reply, or returns NULL if none has been received yet.
	msg = dbus_pending_call_steal_reply(pending);
	if (msg == NULL)
	{
		fprintf(stderr, "Reply Null\n");
		exit(1);
	}

	// free the pending message handle
	dbus_pending_call_unref(pending);

	// read the parameters
	if (!dbus_message_iter_init(msg, &arg))
		fprintf(stderr, "Message has no arguments!\n");
	else if (dbus_message_iter_get_arg_type (&arg) != DBUS_TYPE_BOOLEAN)
		fprintf(stderr, "Argument is not boolean!\n");
	else
		dbus_message_iter_get_basic (&arg, &stat);

	if (!dbus_message_iter_next(&arg))
		fprintf(stderr, "Message has too few arguments!\n");
	else if ( dbus_message_iter_get_arg_type (&arg) != DBUS_TYPE_UINT32 )
		fprintf(stderr, "Argument is not int!\n");
	else
		dbus_message_iter_get_basic (&arg, &level);

	printf("Got Reply: %d, %d\n", stat, level);

	dbus_message_unref(msg);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("invalid param\n");
		exit(1);
	}

	DBusConnection *connection;

	connection = connect_dbus();
	if(connection == NULL)
		return -1;

	send_a_method_call(connection, argv[1]);

	return 0;
}
