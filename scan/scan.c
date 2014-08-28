#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include <unistd.h>

#include "scan.h"

static DBusConnection *init_dbus()
{
	DBusError err;
	DBusConnection *conn;

	dbus_error_init(&err);

	conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
	if(dbus_error_is_set(&err) || NULL == conn)
	{
		fprintf(stderr, "Connection Err : %s\n", err.message);
		dbus_error_free(&err);
		exit(1);
	}

	dbus_error_free(&err);

	return conn;
}

static void start_discovery_reply(DBusPendingCall *pcall,
		void *user_data)
{
	DBusMessage *reply;
	int type;

	printf("%s %d\n", __func__, __LINE__);

	reply = dbus_pending_call_steal_reply (pcall);
	type = dbus_message_get_type (reply);

	while(1)
	{
		if (type == DBUS_MESSAGE_TYPE_SIGNAL)
		{
			printf("%s %d get a signal\n", __func__, __LINE__);
			break;
		}
		printf("%s %d\n", __func__, __LINE__);
		sleep(1);
	}

	dbus_message_unref (reply);
}

static void start_discovery(DBusConnection *conn)
{
	DBusError err;
	DBusMessage *msg;
	DBusPendingCall *pending;

	printf("%s %d\n", __func__, __LINE__);

	dbus_error_init(&err);

	msg = dbus_message_new_method_call(DBUS_NAME, DBUS_PATH, DBUS_INTERFACE_APAPTER,
			"StartDiscovery");
	if (msg == NULL) {
		printf("%s %d msg is null\n", __func__, __LINE__);
		dbus_error_free(&err);
		return;
	}

	if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
		printf("Out of Memory!\n");
		return;
	}

	if (pending == NULL) {
		printf("Pending Call NULL: connection is disconnected\n");
		dbus_message_unref(msg);
		dbus_error_free(&err);
		return;
	}

	dbus_connection_flush(conn);
	dbus_message_unref(msg);

	printf("%s %d\n", __func__, __LINE__);

	if (!dbus_pending_call_set_notify(pending, start_discovery_reply, NULL, NULL))
		printf("No memory\n");

	printf("%s %d\n", __func__, __LINE__);
	dbus_pending_call_unref(pending);

	dbus_error_free(&err);
}

static void append_variant(DBusMessageIter *iter, int type, void *val)
{
	DBusMessageIter value_iter;
	char var_type[2] = {type, '\0'};

	dbus_message_iter_open_container(iter, DBUS_TYPE_VARIANT, var_type, &value_iter);
	dbus_message_iter_append_basic(&value_iter, type, val);
	dbus_message_iter_close_container(iter, &value_iter);
}

static DBusMessage *excute_method_a(DBusConnection *conn, DBusMessage* msg)
{
	DBusPendingCall* pending;

	if (NULL == msg) {
		printf("Message NULL\n");
		return NULL;
	}

	if (!dbus_connection_send_with_reply(conn, msg, &pending, -1))
	{
		printf("Out Of Memory!");
		return NULL;
	}

	if (NULL == pending) {
		printf("Pending Call Null");
		return NULL;
	}

	dbus_connection_flush(conn);
	dbus_message_unref(msg);

	dbus_pending_call_block(pending);
	msg = dbus_pending_call_steal_reply(pending);
	if (NULL == msg) {
		printf("Reply Null\n");
	}
	dbus_pending_call_unref(pending);

	return msg;
}

static void set_property(DBusConnection *conn, const char *name, struct ADAPTER_PROPERTIES *p)
{
	DBusError err;
	DBusMessage *msg;
	DBusMessageIter iter;
	char s[255];

	memset(&s, 0, 255);
	dbus_error_init(&err);

	msg = dbus_message_new_method_call(DBUS_NAME,DBUS_PATH, DBUS_INTERFACE_APAPTER,
			"SetProperty");
	if (msg == NULL)
	{
		printf("%s %d msg is null\n", __func__, __LINE__);
		exit(1);
	}

	/* append data */
	dbus_message_iter_init_append(msg, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &name);

	// printf("%s %d\n", __func__, __LINE__);

	if (!strcmp(name, "Name"))
	{
		// printf("SynergyDBusAdapter::SetProperty('%s', %s)\n", name, p->Name);
		strcpy(s, p->Name);
		append_variant(&iter, DBUS_TYPE_STRING, &s);
	}
	else if (!strcmp(name, "Powered"))
	{
		// printf("SynergyDBusAdapter::SetProperty('%s', %d)\n", name, p->Powered);
		append_variant(&iter, DBUS_TYPE_BOOLEAN, &p->Powered);
	}
	else if (!strcmp(name, "DiscoverableTimeout"))
	{
		// printf("SynergyDBusAdapter::SetProperty('%s', %d)\n", name, p->DiscoverableTimeout);
		append_variant(&iter, DBUS_TYPE_UINT32, &p->DiscoverableTimeout);
	}
	else if (!strcmp(name, "Discoverable"))
	{
		// printf("SynergyDBusAdapter::SetProperty('%s', %d)\n", name, p->Discoverable);
		append_variant(&iter, DBUS_TYPE_BOOLEAN, &p->Discoverable);
	}
	else if (!strcmp(name, "PairableTimeout"))
	{
		// printf("SynergyDBusAdapter::SetProperty('%s', %d)\n", name, p->PairableTimeout);
		append_variant(&iter, DBUS_TYPE_UINT32, &p->PairableTimeout);
	}
	else if (!strcmp(name, "Pairable"))
	{
		// printf("SynergyDBusAdapter::SetProperty('%s', %d)\n", name, p->Pairable);
		append_variant(&iter, DBUS_TYPE_BOOLEAN, &p->Pairable);
	}
	else
	{
		printf("SynergyDBusAdapter::SetProperty('%s') is not supported)\n", name);
		dbus_connection_flush(conn);
		dbus_message_unref(msg);
		exit(1);
	}

	msg = excute_method_a(conn, msg);

	dbus_message_unref(msg);

	dbus_error_free(&err);
}

static void get_properties(DBusConnection *conn)
{
	DBusError err;
	DBusMessage *msg;
	DBusMessageIter iter, dict, dict_key, dict_value;
	dbus_bool_t bValue;
	dbus_uint32_t iValue;

	char key[255];
	char value[255];
	char *ckey;
	char *cValue;
	// int i = 0;

	dbus_error_init(&err);

	msg = dbus_message_new_method_call(DBUS_NAME,DBUS_PATH, DBUS_INTERFACE_APAPTER,
			"GetProperties");
	if (msg == NULL)
	{
		printf("%s %d msg is null\n", __func__, __LINE__);
		exit(1);
	}

	msg = excute_method_a(conn, msg);

	dbus_message_iter_init(msg, &iter);
	dbus_message_iter_recurse(&iter, &dict);

	do
	{
		memset(key, 0, 255);
		memset(value, 0, 255);

		dbus_message_iter_recurse(&dict, &dict_key);
		dbus_message_iter_get_basic(&dict_key, &ckey);

		strcpy(key, ckey);

		// printf("%s %d key:%s\n", __func__, __LINE__, key);

		if (!strcmp(key, "Name"))
		{
			dbus_message_iter_next(&dict_key);
			dbus_message_iter_recurse(&dict_key, &dict_value);
			dbus_message_iter_get_basic(&dict_value, &cValue);
			strcpy(value, cValue);
			printf("%s:  %s\n", key, value);
		}
		else if(!strcmp(key, "Address"))
		{
			dbus_message_iter_next(&dict_key);
			dbus_message_iter_recurse(&dict_key, &dict_value);
			dbus_message_iter_get_basic(&dict_value, &cValue);
			strcpy(value, cValue);
			printf("%s:  %s\n", key, value);
		}
		//Discoverable
		else if(!strcmp(key, "Discoverable"))
		{
			dbus_message_iter_next(&dict_key);
			dbus_message_iter_recurse(&dict_key, &dict_value);
			dbus_message_iter_get_basic(&dict_value, &bValue);
			printf("%s:  %d\n", key, bValue);
		}
		//DiscoverableTimeout
		else if(!strcmp(key, "DiscoverableTimeout"))
		{
			dbus_message_iter_next(&dict_key);
			dbus_message_iter_recurse(&dict_key, &dict_value);
			dbus_message_iter_get_basic(&dict_value, &iValue);
			printf("%s:  %d\n", key, iValue);
		}
		//Discovering
		else if(!strcmp(key, "Discovering"))
		{
			dbus_message_iter_next(&dict_key);
			dbus_message_iter_recurse(&dict_key, &dict_value);
			dbus_message_iter_get_basic(&dict_value, &bValue);
			printf("%s:  %d\n", key, bValue);
		}
		//Pairable
		else if(!strcmp(key, "Pairable"))
		{
			dbus_message_iter_next(&dict_key);
			dbus_message_iter_recurse(&dict_key, &dict_value);
			dbus_message_iter_get_basic(&dict_value, &bValue);
			printf("%s:  %d\n", key, bValue);
		}
		//PairableTimeout
		else if(!strcmp(key, "PairableTimeout"))
		{
			dbus_message_iter_next(&dict_key);
			dbus_message_iter_recurse(&dict_key, &dict_value);
			dbus_message_iter_get_basic(&dict_value, &iValue);
			printf("%s:  %d\n", key, iValue);
		}
		//Powerd
		else if(!strcmp(key, "Powered"))
		{
			dbus_message_iter_next(&dict_key);
			dbus_message_iter_recurse(&dict_key, &dict_value);
			dbus_message_iter_get_basic(&dict_value, &bValue);
			printf("%s:  %d\n", key, bValue);
		}
		//Devices
		else if(!strcmp(key, "Devices"))
		{
			DBusMessageIter device_array;
			dbus_message_iter_next(&dict_key);
			dbus_message_iter_recurse(&dict_key, &dict_value);
			dbus_message_iter_recurse(&dict_value, &device_array);

			if (dbus_message_iter_get_arg_type(&device_array) != DBUS_TYPE_OBJECT_PATH)
			{
				printf("%s:  0\n", key);
				continue;
			}

			// i = 0;
			do
			{
				dbus_message_iter_get_basic(&device_array, &cValue);
				strcpy(value, cValue);
				// printf("%s %d %s %d: %s\n", __func__, __LINE__, key, ++i, value);
			} while (dbus_message_iter_next(&device_array));

		}
		//UUIDs
		else if(!strcmp(key, "UUIDs"))
		{
			DBusMessageIter device_array;
			dbus_message_iter_next(&dict_key);
			dbus_message_iter_recurse(&dict_key, &dict_value);
			dbus_message_iter_recurse(&dict_value, &device_array);

			do
			{
				dbus_message_iter_get_basic(&device_array, &cValue);
			} while(dbus_message_iter_next(&device_array));
		}
		else
		{
			printf("%s: Not to supported \n",key);
		}

	} while (dbus_message_iter_next(&dict));

	dbus_error_free(&err);
}

int main(int argc, char *argv[])
{
	DBusConnection *conn;
	struct ADAPTER_PROPERTIES *p;

	conn = init_dbus();

	p = (struct ADAPTER_PROPERTIES *)malloc(sizeof(struct ADAPTER_PROPERTIES));
	p->DiscoverableTimeout = 0;
	p->PairableTimeout = 0;
	p->Pairable = true;
	p->Discoverable = true;
	set_property(conn, "DiscoverableTimeout", p);
	set_property(conn, "PairableTimeout", p);
	set_property(conn, "Pairable", p);
	set_property(conn, "Discoverable", p);
	free(p);

	get_properties(conn);

	printf("%s %d\n", __func__, __LINE__);
	start_discovery(conn);
	printf("%s %d\n", __func__, __LINE__);

	return 0;
}
