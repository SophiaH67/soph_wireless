#pragma once

#include "openvr_driver.h"
#include "packet.h"

class ControllerDevice : public vr::ITrackedDeviceServerDriver {
public:
    ControllerDevice(char serial[32]);

    // Inherited via ITrackedDeviceServerDriver
    virtual vr::EVRInitError Activate(uint32_t unObjectId) override;
    virtual void Deactivate() override;
    virtual void EnterStandby() override;
    virtual void* GetComponent(const char* pchComponentNameAndVersion) override;
    virtual void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) override;
    void ReceivedTrackerUpdate(TrackerUpdatePacket* packet);
    virtual vr::DriverPose_t GetPose() override;

private:
    vr::TrackedDeviceIndex_t device_id_;
    vr::DriverPose_t last_pose_;
    char* serial_;
};