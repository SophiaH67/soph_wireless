#pragma once

#include "openvr_driver.h"
#include <optional>
#include "packet.h"

class ControllerDevice : public vr::ITrackedDeviceServerDriver {
public:
	ControllerDevice(char serial[32], DeviceRegisterPacket* p);

	// Inherited via ITrackedDeviceServerDriver
	virtual vr::EVRInitError Activate(uint32_t unObjectId) override;
	virtual void Deactivate() override;
	virtual void EnterStandby() override;
	virtual void* GetComponent(const char* pchComponentNameAndVersion) override;
	virtual void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) override;
	void ReceivedTrackerUpdate(const TrackerUpdatePacket* packet);
	void ReceivedPropUpdate(const PropertyUpdatePacket* packet, const std::optional<vr::PropertyContainerHandle_t> container_override);
	virtual vr::DriverPose_t GetPose() override;

private:
	vr::TrackedDeviceIndex_t device_id_;
	vr::DriverPose_t last_pose_;
	char* serial_;
	const DeviceRegisterPacket register_packet_;
	long long lastTrackerUpdateReceivedUnix_;
};