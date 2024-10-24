#include "controller_device.h"
#include <format>

ControllerDevice::ControllerDevice(char serial[32]) : serial_(serial), last_pose_{0}, device_id_(vr::k_unTrackedDeviceIndexInvalid) {};

vr::EVRInitError ControllerDevice::Activate(uint32_t unObjectId) {
    vr::VRDriverLog()->Log(std::format("ControllerDevice::Activate ({})", unObjectId).c_str());

    const vr::PropertyContainerHandle_t container = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);
    vr::VRProperties()->SetStringProperty(container, vr::Prop_ModelNumber_String, "soph_tracker");

    device_id_ = unObjectId;
    return vr::VRInitError_None;
}

void ControllerDevice::Deactivate() {
}

void ControllerDevice::EnterStandby() {
}

void* ControllerDevice::GetComponent(const char* pchComponentNameAndVersion) {
    return nullptr;
}

void ControllerDevice::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) {
    if (unResponseBufferSize >= 1)
        pchResponseBuffer[0] = 0;
}

void ControllerDevice::ReceivedTrackerUpdate(TrackerUpdatePacket* packet) {
    vr::DriverPose_t pose = {0};
    
    pose.deviceIsConnected = packet->deviceIsConnected;
    pose.poseIsValid = packet->poseIsValid;
    pose.qRotation = packet->qRotation;
    pose.result = packet->result;
    memcpy(pose.vecPosition, packet->vecPosition, sizeof(packet->vecPosition));
    
    pose.qWorldFromDriverRotation.w = 1.f;
    pose.qDriverFromHeadRotation.w = 1.f;

    last_pose_ = pose;

    vr::VRDriverLog()->Log("Survived till right before function call...");
    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(device_id_, pose, sizeof(vr::DriverPose_t));
}

vr::DriverPose_t ControllerDevice::GetPose() {
    return last_pose_;
}