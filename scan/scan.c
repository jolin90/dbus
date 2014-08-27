#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include <unistd.h>

#define DBUS_NAME								"com.csr.synergy"
#define DBUS_PATH								"/com/csr/synergy/peercom"
#define DBUS_PATH_AGENT							"/com/csr/synergy/Agent"
#define DBUS_INTERFACE_APAPTER					"com.csr.synergy.Adapter"
#define DBUS_INTERFACE_AGENT					"com.csr.synergy.Agent"
#define DBUS_INTERFACE_DEVICE					"com.csr.synergy.Device"
#define DBUS_INTERFACE_HID						"com.csr.synergy.hidd.profile"
#define DBUS_INTERFACE_A2DP						"com.csr.synergy.a2dpsink.profile"
#define DBUS_INTERFACE_HANDSFREEGATEWAY			"com.csr.synergy.handsfreeGateway"
#define DBUS_INTERFACE_HANDSFREE				"com.csr.synergy.handsfree"
#define DBUS_INTERFACE_VOICECALL				"com.csr.synergy.voicecall"
#define DBUS_INTERFACE_VOICECALLMANAGE			"com.csr.synergy.voicecallmanager"

#define true 1
#define false 0

struct ADAPTER_PROPERTIES
{
	char            Name[255];
	char            Address[50];
	dbus_uint32_t   Class;
	dbus_bool_t     Powered;
	dbus_bool_t     Pairable;
	dbus_uint32_t   PairableTimeout;
	dbus_bool_t     Discoverable;
	dbus_bool_t     Discovering;
	dbus_uint32_t   DiscoverableTimeout;
	char            Devices[50][255];   //objPath of device
	unsigned int    DeviceCount;
	char            UUIDs[50][128];
};

static DBusConnection *init_dbus()
{
	DBusError err;
	DBusConnection *conn;

	dbus_error_init(&err);
	conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
	dbus_error_free(&err);

	return conn;
}

static void start_discovery(DBusConnection *conn)
{
	DBusError err;
	DBusMessage *msg;
	DBusPendingCall *pending;

	dbus_error_init(&err);

	msg = dbus_message_new_method_call(DBUS_NAME, DBUS_PATH, DBUS_INTERFACE_APAPTER,
			"StartDiscovery");
	if (msg == NULL) {
		printf("%s %d msg is null\n", __func__, __LINE__);
		exit(1);
	}

	if (!dbus_connection_send_with_reply(conn, msg, &pending, -1)) {
		printf("Out of Memory!\n");
		exit(1);
	}

	if (pending == NULL) {
		printf("Pending Call NULL: connection is disconnected\n");
		dbus_message_unref(msg);
		exit(1);
	}

	dbus_connection_flush(conn);
	dbus_message_unref(msg);
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
	if (msg == NULL) {
		printf("%s %d msg is null\n", __func__, __LINE__);
		exit(1);
	}

	/* append data */
	dbus_message_iter_init_append(msg, &iter);
	dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &name);

	if (!strcmp(name, "Name")) {
		printf("SynergyDBusAdapter::SetProperty('%s', %s)", name, p->Name);
		strcpy(s, p->Name);
		append_variant(&iter, DBUS_TYPE_STRING, &s);
	} else if (!strcmp(name, "Powered")) {
		printf("SynergyDBusAdapter::SetProperty('%s', %d)", name, p->Powered);
		append_variant(&iter, DBUS_TYPE_BOOLEAN, &p->Powered);
	} else if (!strcmp(name, "DiscoverableTimeout")) {
		printf("SynergyDBusAdapter::SetProperty('%s', %d)", name, p->DiscoverableTimeout);
		append_variant(&iter, DBUS_TYPE_UINT32, &p->DiscoverableTimeout);
	} else if (!strcmp(name, "Discoverable")) {
		printf("SynergyDBusAdapter::SetProperty('%s', %d)", name, p->Discoverable);
		append_variant(&iter, DBUS_TYPE_BOOLEAN, &p->Discoverable);
	} else if (!strcmp(name, "PairableTimeout")) {
		printf("SynergyDBusAdapter::SetProperty('%s', %d)", name, p->PairableTimeout);
		append_variant(&iter, DBUS_TYPE_UINT32, &p->PairableTimeout);
	} else if (!strcmp(name, "Pairable")) {
		printf("SynergyDBusAdapter::SetProperty('%s', %d)", name, p->Pairable);
		append_variant(&iter, DBUS_TYPE_BOOLEAN, &p->Pairable);
	} else {
		printf("SynergyDBusAdapter::SetProperty('%s') is not supported, %d)", name);
		dbus_connection_flush(conn);
		dbus_message_unref(msg);
		exit(1);
	}

	msg = excute_method_a(conn, msg);

	dbus_message_unref(msg);
}

int main(int argc, char *argv[])
{
	DBusConnection *conn;
	struct ADAPTER_PROPERTIES *p;

	conn = init_dbus();

	p = (struct ADAPTER_PROPERTIES *) malloc (sizeof(struct ADAPTER_PROPERTIES));
	p->DiscoverableTimeout = 0;
	p->PairableTimeout = 0;
	p->Pairable = true;
	p->Discoverable = true;
	set_property(conn, "DiscoverableTimeout", p);
	set_property(conn, "PairableTimeout", p);
	set_property(conn, "Pairable", p);
	set_property(conn, "Discoverable", p);
	free(p);

	//	start_discovery(conn);

	return 0;
}
