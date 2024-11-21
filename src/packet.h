#pragma once

enum PacketType {
	TRACKER_UPDATE,
	PROP_UPDATE
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
};

enum UpdateValueType {
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
	UpdateValueType type;
};

struct Packet {
	PacketType type;
	char serial[64]; // As identification of device
	union {
		TrackerUpdatePacket tracker_update;
		PropertyUpdatePacket property_update;
	};
};
