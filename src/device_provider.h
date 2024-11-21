#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <ws2tcpip.h>
#include <memory>
#include <winsock2.h>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <string>
#include <thread>

#include "controller_device.h"
#include "openvr_driver.h"
#include "packet.h"

class DeviceProvider : public vr::IServerTrackedDeviceProvider {
public:
    // Inherited via IServerTrackedDeviceProvider
    vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext) override;
    void Cleanup() override;
    const char* const* GetInterfaceVersions() override;
    void RunFrame() override;
    bool ShouldBlockStandbyMode() override;
    void EnterStandby() override;
    void LeaveStandby() override;

private:
    std::mutex known_trackers_mutex;
    std::unordered_map<std::string, std::unique_ptr<ControllerDevice>> known_trackers{};
    std::thread udp_server_thread;
    std::thread tcp_server_thread;

    void handle_packet(Packet* packet);
    void udp_vserver();
    void tcp_vserver();
};
