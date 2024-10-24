#pragma once
enum PacketType {
	TRACKER_UPDATE
};

struct Packet {
	PacketType packet_type;
	char packet[512];
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
	char serial[32]; // As identification of device
};