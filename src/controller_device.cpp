#include "controller_device.h"

ControllerDevice::ControllerDevice(vr::ETrackedControllerRole role) : role_(role), device_id_(vr::k_unTrackedDeviceIndexInvalid) {};

vr::EVRInitError ControllerDevice::Activate(uint32_t unObjectId) {
    vr::VRDriverLog()->Log("ControllerDevice::Activate");

    const vr::PropertyContainerHandle_t container = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);
    vr::VRProperties()->SetInt32Property(container, vr::Prop_ControllerRoleHint_Int32, role_);
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

vr::DriverPose_t ControllerDevice::GetPose() {
    vr::DriverPose_t pose = { 0 };

    pose.poseIsValid = true;
    pose.result = vr::TrackingResult_Running_OK;
    pose.deviceIsConnected = true;

    pose.qWorldFromDriverRotation.w = 1.f;
    pose.qDriverFromHeadRotation.w = 1.f;

    pose.qRotation.w = 1.f;

    vr::TrackedDevicePose_t hmd_pose{};
    vr::VRServerDriverHost()->GetRawTrackedDevicePoses(0.f, &hmd_pose, 1);

    pose.vecPosition[0] = role_ == vr::TrackedControllerRole_LeftHand
        ? hmd_pose.mDeviceToAbsoluteTracking.m[0][3] - 0.2f
        : hmd_pose.mDeviceToAbsoluteTracking.m[0][3] + 0.2f; // set the x position offset based on whether we are a left or right hand (so the controllers don't appear on top of eachother). This will
    // make it look like two controllers appearing side by side, 0.4 metres apart.

    pose.vecPosition[1] = hmd_pose.mDeviceToAbsoluteTracking.m[1][3];		// no vertical offset
    pose.vecPosition[2] = hmd_pose.mDeviceToAbsoluteTracking.m[2][3] - 0.5f; // make the controller appear half a metre in front of the hmd (-z is forward in space)

    return pose;
}

void ControllerDevice::RunFrame() {
    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(device_id_, GetPose(), sizeof(vr::DriverPose_t));
}