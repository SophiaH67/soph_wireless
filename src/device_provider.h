#pragma once
#pragma comment(lib, "Ws2_32.lib")
#include <ws2tcpip.h>
#include <memory>
#include <winsock2.h>

#include "controller_device.h"
#include "openvr_driver.h"

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
};

void vserver();