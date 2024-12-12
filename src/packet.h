#pragma once

const vr::ETrackedDeviceProperty PROPS_TO_TRACK_I32[] = {
	vr::ETrackedDeviceProperty::Prop_DeviceClass_Int32,
	vr::ETrackedDeviceProperty::Prop_ControllerRoleHint_Int32,
};
const vr::ETrackedDeviceProperty PROPS_TO_TRACK_STRING[] = {
	vr::ETrackedDeviceProperty::Prop_RenderModelName_String,
	vr::ETrackedDeviceProperty::Prop_ModelNumber_String,
	vr::ETrackedDeviceProperty::Prop_TrackingSystemName_String,
	vr::ETrackedDeviceProperty::Prop_IconPathName_String,
	vr::ETrackedDeviceProperty::Prop_NamedIconPathDeviceOff_String,
	vr::ETrackedDeviceProperty::Prop_NamedIconPathDeviceSearching_String,
	vr::ETrackedDeviceProperty::Prop_NamedIconPathDeviceSearchingAlert_String,
	vr::ETrackedDeviceProperty::Prop_NamedIconPathDeviceReady_String,
	vr::ETrackedDeviceProperty::Prop_NamedIconPathDeviceReadyAlert_String,
	vr::ETrackedDeviceProperty::Prop_NamedIconPathDeviceNotReady_String,
	vr::ETrackedDeviceProperty::Prop_NamedIconPathDeviceStandby_String,
	vr::ETrackedDeviceProperty::Prop_NamedIconPathDeviceAlertLow_String,
	vr::ETrackedDeviceProperty::Prop_NamedIconPathDeviceStandbyAlert_String,
	vr::ETrackedDeviceProperty::Prop_RegisteredDeviceType_String,
	vr::ETrackedDeviceProperty::Prop_ResourceRoot_String,
	vr::ETrackedDeviceProperty::Prop_RegisteredDeviceType_String,
	vr::ETrackedDeviceProperty::Prop_InputProfilePath_String,
	vr::ETrackedDeviceProperty::Prop_ControllerType_String,
	vr::ETrackedDeviceProperty::Prop_RenderModelName_String,
	vr::ETrackedDeviceProperty::Prop_ManufacturerName_String,

	vr::ETrackedDeviceProperty::Prop_TrackingFirmwareVersion_String,
	vr::ETrackedDeviceProperty::Prop_HardwareRevision_String,
	vr::ETrackedDeviceProperty::Prop_ConnectedWirelessDongle_String,
};
const vr::ETrackedDeviceProperty PROPS_TO_TRACK_BOOL[] = {
	vr::ETrackedDeviceProperty::Prop_WillDriftInYaw_Bool,
	vr::ETrackedDeviceProperty::Prop_DeviceIsWireless_Bool,
	vr::ETrackedDeviceProperty::Prop_DeviceIsCharging_Bool,
	vr::ETrackedDeviceProperty::Prop_Identifiable_Bool,
};
const vr::ETrackedDeviceProperty PROPS_TO_TRACK_FLOAT[] = {
	vr::ETrackedDeviceProperty::Prop_DeviceBatteryPercentage_Float
};

const vr::ETrackedDeviceProperty PROPS_TO_TRACK_UINT64[] = {
	vr::ETrackedDeviceProperty::Prop_HardwareRevision_Uint64,
	vr::ETrackedDeviceProperty::Prop_FirmwareVersion_Uint64,
	vr::ETrackedDeviceProperty::Prop_FPGAVersion_Uint64,
	vr::ETrackedDeviceProperty::Prop_VRCVersion_Uint64,
	vr::ETrackedDeviceProperty::Prop_RadioVersion_Uint64,
	vr::ETrackedDeviceProperty::Prop_DongleVersion_Uint64,
};

const vr::ETrackedDeviceProperty PROPS_TO_TRACK_M34[] = {
	vr::ETrackedDeviceProperty::Prop_StatusDisplayTransform_Matrix34
};


const size_t PROPS_TO_TRACK_I32_LENGTH = sizeof(PROPS_TO_TRACK_I32) / sizeof(vr::ETrackedDeviceProperty);
const size_t PROPS_TO_TRACK_STRING_LENGTH = sizeof(PROPS_TO_TRACK_STRING) / sizeof(vr::ETrackedDeviceProperty);
const size_t PROPS_TO_TRACK_BOOL_LENGTH = sizeof(PROPS_TO_TRACK_BOOL) / sizeof(vr::ETrackedDeviceProperty);
const size_t PROPS_TO_TRACK_FLOAT_LENGTH = sizeof(PROPS_TO_TRACK_FLOAT) / sizeof(vr::ETrackedDeviceProperty);
const size_t PROPS_TO_TRACK_UINT64_LENGTH = sizeof(PROPS_TO_TRACK_UINT64) / sizeof(vr::ETrackedDeviceProperty);
const size_t PROPS_TO_TRACK_M34_LENGTH = sizeof(PROPS_TO_TRACK_M34) / sizeof(vr::ETrackedDeviceProperty);


enum PacketType {
	SMALL_TRACKER_UPDATE,
	SMALL_PROP_UPDATE,

	BIG_DEVICE_REGISTER,
};

// Required params for the driver to register and track a tracker
struct TrackerUpdatePacket {
	double vecPosition[3];
	double vecVelocity[3];
	double vecAcceleration[3];
	double vecAngularVelocity[3];
	double vecAngularAcceleration[3];
	vr::HmdQuaternion_t qRotation;
	vr::ETrackingResult result;
	bool poseIsValid;
	bool deviceIsConnected;
	long long unixTimestamp;
};

enum PropValueType {
	UNINITIALIZED,
	INT32_T,
	STRING,
	BOOOL, // Can't name it bool because c++
	FLOOAT,
	UINT64_T,
	MATRIX34,
};

struct PropertyUpdatePacket {
	vr::ETrackedDeviceProperty property;
	union {
		int32_t value_int32;
		char value_string[64];
		bool value_bool;
		float value_float;
		uint64_t value_uint64;
		float value_m34[3][4];

	};
	PropValueType type;
};

struct DeviceRegisterPacket {
	vr::ETrackedDeviceClass device_class;
	PropertyUpdatePacket properties[PROPS_TO_TRACK_I32_LENGTH + PROPS_TO_TRACK_STRING_LENGTH + PROPS_TO_TRACK_BOOL_LENGTH + PROPS_TO_TRACK_FLOAT_LENGTH + PROPS_TO_TRACK_UINT64_LENGTH + PROPS_TO_TRACK_M34_LENGTH];
};

struct Packet {
	PacketType type;
	char serial[64]; // As identification of device
	union {
		TrackerUpdatePacket tracker_update;
		PropertyUpdatePacket property_update;
	};
};

struct BigPacket {
	PacketType type;
	char serial[64]; // As identification of device
	union {
		DeviceRegisterPacket device_register;
	};
};
