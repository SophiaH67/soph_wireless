#include "device_provider.h"
#include <thread>
#include <format>
#include "packet.h"
#include <unordered_map>
#include <string>

void vserver() {
	WSADATA wsa_data;

	int res;
	vr::VRDriverLog()->Log("VServer Initializing...");

	// Initialize Winsock
	res = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (res != 0) {
		vr::VRDriverLog()->Log(std::format("WSAStartup failed: {}\n", res).c_str());
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

	char RecvBuf[sizeof(Packet)];
	int BufLen = sizeof(Packet);
	struct sockaddr_in sender_addr {};
	int sender_addr_size = sizeof(sender_addr);

	vr::VRDriverLog()->Log("VServer Initialized!");

	std::unordered_map<std::string, std::unique_ptr<ControllerDevice>> known_trackers;

	while (true) {
		recvfrom(server_socket, RecvBuf, BufLen, 0, (SOCKADDR*)&sender_addr, &sender_addr_size);
		vr::VRDriverLog()->Log("Received packet");
		Packet* packet = (struct Packet*)RecvBuf;

		switch (packet->packet_type)
		{
			case TRACKER_UPDATE: {
				TrackerUpdatePacket* tracker_message = (struct TrackerUpdatePacket*)packet->packet;
				vr::VRDriverLog()->Log(std::format("Got tracker update {} {} {} from tracker '{}'", tracker_message->vecPosition[0], tracker_message->vecPosition[1], tracker_message->vecPosition[2], tracker_message->serial).c_str());
				std::string serial(tracker_message->serial);

				if (known_trackers.find(tracker_message->serial) == known_trackers.end()) {
					known_trackers[serial] = std::make_unique<ControllerDevice>(tracker_message->serial);

					vr::VRServerDriverHost()->TrackedDeviceAdded(tracker_message->serial,
						vr::TrackedDeviceClass_GenericTracker,
						known_trackers[serial].get());
					vr::VRDriverLog()->Log("Sleeping for 1 second to avoid calling into uninitialized controller...");
					std::this_thread::sleep_for(std::chrono::seconds(1));
				}
				vr::VRDriverLog()->Log("Updating pose...");
				if (known_trackers[serial] != nullptr) {
					vr::VRDriverLog()->Log("Sending tracker update...");
					known_trackers[serial]->ReceivedTrackerUpdate(tracker_message);
				}

				break;
			}
			default: {
				vr::VRDriverLog()->Log(std::format("Received unknown packet: {}", (int)packet->packet_type).c_str());
				break;
			}
		}
	}
}

vr::EVRInitError DeviceProvider::Init(vr::IVRDriverContext* pDriverContext) {
	VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

	vr::VRDriverLog()->Log("Hello world!");

	// Start a thread
	std::thread my_pose_update_thread_ = std::thread(vserver);
	my_pose_update_thread_.detach();

	return vr::VRInitError_None;
}

void DeviceProvider::Cleanup() {
	VR_CLEANUP_SERVER_DRIVER_CONTEXT();
}

const char* const* DeviceProvider::GetInterfaceVersions() {
	return vr::k_InterfaceVersions;
}

void DeviceProvider::RunFrame() {}

bool DeviceProvider::ShouldBlockStandbyMode() {
	return false;
}

void DeviceProvider::EnterStandby() {

}

void DeviceProvider::LeaveStandby() {

}