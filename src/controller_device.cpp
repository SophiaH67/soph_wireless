#include "controller_device.h"
#include <format>

ControllerDevice::ControllerDevice(char serial[32], DeviceRegisterPacket* p) : serial_(serial), register_packet_(*p), lastTrackerUpdateReceivedUnix_{ 0 }, last_pose_ { 0 }, device_id_(vr::k_unTrackedDeviceIndexInvalid) {};

vr::EVRInitError ControllerDevice::Activate(uint32_t unObjectId) {
    vr::VRDriverLog()->Log(std::format("ControllerDevice::Activate ({})", unObjectId).c_str());

    const vr::PropertyContainerHandle_t container = vr::VRProperties()->TrackedDeviceToPropertyContainer(unObjectId);

    for (const PropertyUpdatePacket p : register_packet_.properties)
    {
        ReceivedPropUpdate(&p, container);
    }


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

void ControllerDevice::ReceivedTrackerUpdate(const TrackerUpdatePacket* packet) {
    if (packet->unixTimestamp < lastTrackerUpdateReceivedUnix_) return;
    lastTrackerUpdateReceivedUnix_ = packet->unixTimestamp;

    vr::DriverPose_t pose = {0};
    
    pose.deviceIsConnected = packet->deviceIsConnected;
    pose.poseIsValid = packet->poseIsValid;
    pose.qRotation = packet->qRotation;
    pose.result = packet->result;
    memcpy(pose.vecPosition, packet->vecPosition, sizeof(packet->vecPosition));
    
    pose.qWorldFromDriverRotation.w = 1.f;
    pose.qDriverFromHeadRotation.w = 1.f;

    last_pose_ = pose;

    vr::VRServerDriverHost()->TrackedDevicePoseUpdated(device_id_, pose, sizeof(vr::DriverPose_t));
}

void ControllerDevice::ReceivedPropUpdate(const PropertyUpdatePacket* packet, const std::optional<vr::PropertyContainerHandle_t> container_override = std::nullopt) {
    vr::PropertyContainerHandle_t container;
    if (container_override) 
        container = container_override.value();
    
    else
        container = vr::VRProperties()->TrackedDeviceToPropertyContainer(device_id_);

    switch (packet->type)
    {
        case PropValueType::BOOOL: {
            vr::VRProperties()->SetBoolProperty(container, packet->property, packet->value_string);
            break;
        }
        case PropValueType::FLOOAT: {
            vr::VRProperties()->SetFloatProperty(container, packet->property, packet->value_float);
            break;
        }
        case PropValueType::INT32_T: {
            vr::VRProperties()->SetInt32Property(container, packet->property, packet->value_int32);
            break;
        }
        case PropValueType::MATRIX34: {
            //vr::VRProperties()->SetM34(container, packet->property, packet->value_int32);
            break;
        }
        case PropValueType::STRING: {
            vr::VRDriverLog()->Log(std::format("Got string update for prop {} to '{}'", (int)packet->property, packet->value_string).c_str());
            vr::VRProperties()->SetStringProperty(container, packet->property, packet->value_string);
            break;
        }
        case PropValueType::UINT64_T: {
            vr::VRProperties()->SetUint64Property(container, packet->property, packet->value_uint64);
            break;
        }
        default: {
            vr::VRDriverLog()->Log(std::format("Unknown value type {} for tracker '{}'", (int)packet->type, device_id_).c_str());
            break;
        }
    }
}

vr::DriverPose_t ControllerDevice::GetPose() {
    return last_pose_;
}