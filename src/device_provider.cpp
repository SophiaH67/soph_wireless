#include "device_provider.h"
#include <thread>
#include <format>

void vserver() {
    WSADATA wsaData;

    int iResult;
    vr::VRDriverLog()->Log("VServer Initializing...");
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        vr::VRDriverLog()->Log(std::format("WSAStartup failed: {}\n", iResult).c_str());
        WSACleanup();
        return;
    }

    // Initialize the socket
    SOCKET server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server_socket == INVALID_SOCKET) {
        vr::VRDriverLog()->Log(std::format("Error at socket(): {}\n", WSAGetLastError()).c_str());
        WSACleanup();
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
    server_addr.sin_port = htons(6767);

    int bind_result = bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
    if (bind_result != 0) {
        vr::VRDriverLog()->Log(std::format("Error while setting up bind: {}", bind_result).c_str());
        WSACleanup();
        return;
    }

    char RecvBuf[1024];
    int BufLen = 1024;
    struct sockaddr_in SenderAddr;
    int SenderAddrSize = sizeof(SenderAddr);

    vr::VRDriverLog()->Log("VServer Initialized!");

    recvfrom(server_socket, RecvBuf, BufLen, 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);

    vr::VRDriverLog()->Log("Got something (idk how to log it)");
}

vr::EVRInitError DeviceProvider::Init(vr::IVRDriverContext* pDriverContext) {
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

    vr::VRDriverLog()->Log("Hello world!");

    // Start a thread
    std::thread my_pose_update_thread_ =std::thread(vserver);
    my_pose_update_thread_.detach();

    my_left_device_ = std::make_unique<ControllerDevice>(vr::TrackedControllerRole_LeftHand);
    vr::VRServerDriverHost()->TrackedDeviceAdded("SOPL1234",
        vr::TrackedDeviceClass_Controller,
        my_left_device_.get());

    my_right_device_ = std::make_unique<ControllerDevice>(vr::TrackedControllerRole_RightHand);
    vr::VRServerDriverHost()->TrackedDeviceAdded("SOPR5678",
        vr::TrackedDeviceClass_Controller,
        my_right_device_.get());

    return vr::VRInitError_None;
}

void DeviceProvider::Cleanup() {
    VR_CLEANUP_SERVER_DRIVER_CONTEXT();
}

const char* const* DeviceProvider::GetInterfaceVersions() {
    return vr::k_InterfaceVersions;
}

void DeviceProvider::RunFrame() {
    if (my_left_device_ != nullptr) {
        my_left_device_->RunFrame();
    }

    if (my_right_device_ != nullptr) {
        my_right_device_->RunFrame();
    }
}

bool DeviceProvider::ShouldBlockStandbyMode() {
    return false;
}

void DeviceProvider::EnterStandby() {

}

void DeviceProvider::LeaveStandby() {

}