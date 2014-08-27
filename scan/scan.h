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


