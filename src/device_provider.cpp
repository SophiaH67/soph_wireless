#include "device_provider.h"
#include <format>

void DeviceProvider::handle_packet(Packet *packet)  {
	std::string serial(packet->serial); 

	std::lock_guard<std::mutex> known_trackers_lock(known_trackers_mutex);

	switch (packet->type)
	{
		case SMALL_TRACKER_UPDATE: {
			if (known_trackers.find(serial) == known_trackers.end()) return;
			TrackerUpdatePacket* tracker_message = &packet->tracker_update;
			//vr::VRDriverLog()->Log(std::format("Got tracker update {} {} {} from tracker '{}'", tracker_message->vecPosition[0], tracker_message->vecPosition[1], tracker_message->vecPosition[2], packet->serial).c_str());
			if (known_trackers[serial] != nullptr) {
				known_trackers[serial]->ReceivedTrackerUpdate(tracker_message);
			}
			else {
				vr::VRDriverLog()->Log(std::format("Attempted to update position on nullptr tracker with serial '{}'", serial).c_str());
			}

			break;
		}
		case SMALL_PROP_UPDATE: {
			if (known_trackers.find(serial) == known_trackers.end()) return;
			PropertyUpdatePacket* prop_message = &packet->property_update;
			//vr::VRDriverLog()->Log(std::format("Got prop update of type '{}' of property '{}' from tracker '{}'", (int)prop_message->type, (int)prop_message->property, serial).c_str());
			if (known_trackers[serial] != nullptr) {
				known_trackers[serial]->ReceivedPropUpdate(prop_message, std::nullopt);
			}
			else {
				vr::VRDriverLog()->Log(std::format("Attempted to update prop on nullptr tracker with serial '{}'", serial).c_str());
			}
			break;
		}
		case BIG_DEVICE_REGISTER: {
			BigPacket* big_packet = (BigPacket*) packet;
			DeviceRegisterPacket* register_message = &big_packet->device_register;
			if (known_trackers.find(serial) == known_trackers.end()) {
				known_trackers.insert({ serial, std::make_unique<ControllerDevice>(packet->serial, register_message) });

				vr::VRServerDriverHost()->TrackedDeviceAdded(packet->serial,
					register_message->device_class,
					known_trackers.at(serial).get());
			}
			else {
				vr::VRDriverLog()->Log(std::format("Got request to re-register tracker '{}'. Ignoring", serial).c_str());
			}
			break;
		}
		default: {
			vr::VRDriverLog()->Log(std::format("Received unknown packet: {}", (int)packet->type).c_str());
			break;
		}
	}
}

void DeviceProvider::udp_vserver() {
	// Initialize the socket
	SOCKET server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (server_socket == INVALID_SOCKET) {
		vr::VRDriverLog()->Log(std::format("Error at UDP socket(): {}\n", WSAGetLastError()).c_str());
		WSACleanup();
		return;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
	server_addr.sin_port = htons(6767);

	int bind_result = bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (bind_result != 0) {
		vr::VRDriverLog()->Log(std::format("Error while setting up bind on UDP: {}\n", bind_result).c_str());
		WSACleanup();
		return;
	}

	char RecvBuf[sizeof(BigPacket)];
	int BufLen = sizeof(BigPacket);
	struct sockaddr_in sender_addr {};
	int sender_addr_size = sizeof(sender_addr);

	vr::VRDriverLog()->Log("UDP VServer Initialized!");

	while (true) {
		recvfrom(server_socket, RecvBuf, BufLen, 0, (SOCKADDR*)&sender_addr, &sender_addr_size);
		Packet* packet = (struct Packet*)RecvBuf;

		handle_packet(packet);
	}
}

void DeviceProvider::tcp_vserver() {
	SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_socket == INVALID_SOCKET) {
		vr::VRDriverLog()->Log(std::format("Error at TCP socket(): {}\n", WSAGetLastError()).c_str());
		WSACleanup();
		return;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY; // 0.0.0.0
	server_addr.sin_port = htons(6767);

	int bind_result = bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (bind_result != 0) {
		vr::VRDriverLog()->Log(std::format("Error while setting up bind on TCP: {}\n", bind_result).c_str());
		WSACleanup();
		return;
	}

	if (listen(server_socket, 1) == SOCKET_ERROR) {
		vr::VRDriverLog()->Log(std::format("Error listening on socket: {}\n", WSAGetLastError()).c_str());
	}
	else {
		vr::VRDriverLog()->Log("TCP listening");
	}

	while (true) {
		vr::VRDriverLog()->Log("TCP Waiting on connection...");
		struct sockaddr_in sender_addr {};
		int sender_addr_size = sizeof(sender_addr);
		SOCKET socket;
		socket = accept(server_socket, (SOCKADDR*)&sender_addr, &sender_addr_size);
		vr::VRDriverLog()->Log("After accept()");

		char ipStr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &sender_addr.sin_addr, ipStr, sizeof(ipStr));
		vr::VRDriverLog()->Log(std::format("Got TCP connection from {}:{}", ipStr, sender_addr.sin_port).c_str());

		char RecvBuf[sizeof(BigPacket)];
		int BufLen = sizeof(BigPacket);

		while (true) {
			int rbyteCount = recv(socket, RecvBuf, BufLen, 0);
			if (rbyteCount < 1) { // 0 = disconnected gracefully
				vr::VRDriverLog()->Log(std::format("Error receiving on socket: {}\n", WSAGetLastError()).c_str());
				break;
			}
			Packet* packet = (struct Packet*)RecvBuf;
			
			handle_packet(packet);
		}
	}
}

vr::EVRInitError DeviceProvider::Init(vr::IVRDriverContext* pDriverContext) {
	VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

	vr::VRDriverLog()->Log("Hello world!");

	// Init windows network garbage
	WSADATA wsa_data;

	int res;
	vr::VRDriverLog()->Log("Windows Networking Initializing...");

	res = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (res != 0) {
		vr::VRDriverLog()->Log(std::format("Windows Networking Failed: {}\n", res).c_str());
		WSACleanup();
		return vr::VRInitError_Driver_Failed;
	}

	// Start a thread
	udp_server_thread = std::thread(&DeviceProvider::udp_vserver, this);
	tcp_server_thread = std::thread(&DeviceProvider::tcp_vserver, this);
	udp_server_thread.detach();
	tcp_server_thread.detach();

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