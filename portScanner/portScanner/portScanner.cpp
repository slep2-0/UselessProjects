#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <Windows.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

int initWSA() {
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		std::cout << "Failed to load and start the WSA DLL. Error Code: " << GetLastError() << std::endl;
		return 1;
	}
	return 0;
}

int ScanPort(std::string& ip, int port) {
	SOCKET sock = socket(2, 1, 6); // 2 - AF_INET = IPv4 , 1 - SOCK_STREAM = TCP Transport Method (so we scan TCP Ports.), 6 - IPPROTO_TCP = Use the TCP Protocol, idk wtf is the difference between the second and third.
	if (sock == INVALID_SOCKET) {
		return 0;
	}
	sockaddr_in addr; // IPv4 Address
	addr.sin_family = AF_INET; // Use IPv4
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

	// set timeout to give the system time to respond
	DWORD timeout = 1500; // 1.5 sec.
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));// sock - Use our socket we declared, SOL_SOCKET - Use socket options (broadcasts, timeouts, etc), Convert timeout to char, and also gets its size for optlen
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
	
	bool open = (connect(sock, (SOCKADDR*)&addr, sizeof(addr)) == 0); // check if open

	closesocket(sock); // cleanup

	if (open) {
		return 1;
	}
	else {
		return 2;
	}
}

int main() {
	if (initWSA() == 1) {
		std::cout << "Program is now exiting because the WSA DLL has not been loaded.\n";
		return 1;
	}
	while (true) {
		std::cout << "Specify an IP to scan: ";
		std::string ip;
		std::cin >> ip;
		std::cout << "\nSpecify a PORT to scan: ";
		int port;
		std::cin >> port;
		switch (ScanPort(ip, port)) {
		case 0:
			std::cout << "\nFailed to start scanning, INVALID_SOCKET returned, exiting...\n";
			return 1;
			break;
		case 1:
			std::cout << "\nPort " << port << " is open.\n";
			break;
		case 2:
			std::cout << "\nPort " << port << " is closed.\n";
			break;
		}
		Sleep(2000);
		system("cls");
	}
}