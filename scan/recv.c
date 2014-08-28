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

struct adapter_device {
	char Name[255];
	char Address[255];
	char Alise[255];
	char Adapter[255];
	char UUIDs[10][255];

	int Paired;
	int Connected;
	unsigned int Class;
};

static void adapter_signal_device_found(DBusMessage* msg)
{
	printf("adapter_signal_DeviceFound - start\n");

	DBusMessageIter iter;
	DBusMessageIter dict,dict_key,dict_value;
	DBusMessageIter device_array;

	struct adapter_device device;

	char key[255];
	char value[255];
	char *ckey;
	char *cValue;
	dbus_bool_t bValue;
	dbus_uint32_t uint32Value;

	if (dbus_message_iter_init(msg, &iter))
	{
		dbus_message_iter_get_basic(&iter, &ckey);
		dbus_message_iter_next(&iter);
		dbus_message_iter_recurse(&iter, &dict);

		do{
			memset(key, 0, 255);
			memset(value, 0, 255);

			dbus_message_iter_recurse(&dict, &dict_key);
			dbus_message_iter_get_basic(&dict_key, &ckey);
			strcpy(key, ckey);
			//Name
			if (!strcmp(key,"Name"))
			{
				dbus_message_iter_next(&dict_key);
				dbus_message_iter_recurse(&dict_key, &dict_value);
				dbus_message_iter_get_basic(&dict_value, &cValue);
				strcpy(value, cValue);
				printf("%s:  %s\n",key, value);
			}
			//Address
			else if(!strcmp(key,"Address"))
			{
				dbus_message_iter_next(&dict_key);
				dbus_message_iter_recurse(&dict_key, &dict_value);
				dbus_message_iter_get_basic(&dict_value, &cValue);
				strcpy(value, cValue);
				printf("%s:  %s\n",key,value);
			}
			//Paired
			else if(!strcmp(key,"Paired"))
			{
				dbus_message_iter_next(&dict_key);
				dbus_message_iter_recurse(&dict_key, &dict_value);
				dbus_message_iter_get_basic(&dict_value, &bValue);
				device.Paired = bValue;
				printf("%s:  %d\n",key,bValue);
			}
			//Connected
			else if(!strcmp(key,"Connected"))
			{
				dbus_message_iter_next(&dict_key);
				dbus_message_iter_recurse(&dict_key, &dict_value);
				dbus_message_iter_get_basic(&dict_value, &bValue);
				device.Connected = bValue;
				printf("%s:  %d\n",key,bValue);
			}
			//Class
			else if(!strcmp(key,"Class"))
			{
				dbus_message_iter_next(&dict_key);
				dbus_message_iter_recurse(&dict_key, &dict_value);
				dbus_message_iter_get_basic(&dict_value, &uint32Value);
				device.Class = uint32Value;
				printf("%s:  %d\n",key,uint32Value);
			}
			//Alise
			else if(!strcmp(key,"Alise"))
			{
				dbus_message_iter_next(&dict_key);
				dbus_message_iter_recurse(&dict_key, &dict_value);
				dbus_message_iter_get_basic(&dict_value, &cValue);
				strcpy(value, cValue);
				printf("%s:  %s\n",key,value);
			}
			//Adapter
			else if(!strcmp(key,"Adapter"))
			{
				dbus_message_iter_next(&dict_key);
				dbus_message_iter_recurse(&dict_key, &dict_value);
				dbus_message_iter_get_basic(&dict_value, &cValue);
				strcpy(value, cValue);
				printf("%s:  %s\n",key,value);
			}
			//UUID
			else if(!strcmp(key,"UUIDs"))
			{
				dbus_message_iter_next(&dict_key);
				dbus_message_iter_recurse(&dict_key, &dict_value);
				dbus_message_iter_recurse(&dict_value, &device_array);

				int i = 0;
				do
				{
					dbus_message_iter_get_basic(&device_array, &cValue);
					strcpy(device.UUIDs[i], cValue);
					printf("%s_%d:  %s\n",key,++i,device.UUIDs[i]);
				}while (dbus_message_iter_next(&device_array));
			}
			//Other
			else
			{
				printf("%s:  unhandled\n",key);
			}

		}while(dbus_message_iter_next(&dict));

	}

}

int main(int argc, char *argv[])
{
	DBusConnection *conn;
	DBusError error;
	DBusMessage *msg;

	dbus_error_init(&error);
	conn = init_dbus();

	dbus_bus_add_match(conn, "type='signal', sender='com.csr.synergy'", &error);
	dbus_connection_flush(conn);

	while (1)
	{
		dbus_connection_read_write(conn,0);
		msg = dbus_connection_pop_message(conn);

		if(msg == NULL) {
			sleep(1);
			continue;
		}

		if (dbus_message_is_signal(msg, DBUS_INTERFACE_APAPTER, "DeviceFound"))
		{
			printf("%s %d DeviceFound\n", __func__, __LINE__);
			adapter_signal_device_found( msg);
		}
		dbus_message_unref(msg);
	}

	return 0;
}
