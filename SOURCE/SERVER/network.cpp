	/***********************************************************/
	/*                            Network                      */
	/***********************************************************/

	#include <WinSock2.h>
	#include <WS2tcpip.h>

	#pragma comment(lib, "Ws2_32.lib")

	#include "network.h"

	/***********************************************************/
	in_addr ip_to_num;
	WSADATA wsData;
	short port_num;

	/***********************************************************/

	int open_Network(const char* ip_addr, short port)
	{
		int status;
		port_num = port;

		// convert ip address
		status = inet_pton(AF_INET, ip_addr, &ip_to_num);
		if (status <= 0) return BADADDR;


		// get WS32 library
		status = WSAStartup(MAKEWORD(2, 2), &wsData); // WinSock initialization
		if (status != 0) return WSERROR;

		return 0;
	};

	/***********************************************************/

	SOCKET server_socket;
	SOCKET client_socket;

	void close_Connection()
	{
		closesocket(server_socket);
		closesocket(client_socket);
	};

	/***********************************************************/

	int wait_Connection(char* client_ip)
	{
		int status;

		server_socket = socket(AF_INET, SOCK_STREAM, 0);		// socket initialization
		if (server_socket == INVALID_SOCKET) {
			closesocket(server_socket);
			return SOCKERR;
		};

		// server socket binding
		sockaddr_in servInfo;
		ZeroMemory(&servInfo, sizeof(servInfo));	// Initializing servInfo structure

		servInfo.sin_family = AF_INET;
		servInfo.sin_addr = ip_to_num;
		servInfo.sin_port = htons(port_num);

		status = bind(server_socket, (sockaddr*)&servInfo, sizeof(servInfo));
		if (status != 0) {
			closesocket(server_socket);
			return BINDERR;
		};

		// start to listen 
		status = listen(server_socket, SOMAXCONN);
		if (status != 0) {
			closesocket(server_socket);
			return LISTERR;
		};

		// client socket creation and acception in case of connection
		sockaddr_in clientInfo;
		ZeroMemory(&clientInfo, sizeof(clientInfo));	// Initializing clientInfo structure

		int clientInfo_size = sizeof(clientInfo);

		client_socket = accept(server_socket, (sockaddr*)&clientInfo, &clientInfo_size);

		if (client_socket == INVALID_SOCKET) {
			close_Connection();
			return SOCKERR;
		};

		inet_ntop(AF_INET, &clientInfo.sin_addr, client_ip, INET_ADDRSTRLEN);

		return 0;
	};
	/***********************************************************/

	int open_Connection() 
	{
		int status;

		// socket initialization
		client_socket = socket(AF_INET, SOCK_STREAM, 0);

		if (client_socket == INVALID_SOCKET) {
			closesocket(client_socket);
			return SOCKERR;
		};

		// establishing a connection to server
		sockaddr_in servInfo;
		ZeroMemory(&servInfo, sizeof(servInfo));

		servInfo.sin_family = AF_INET;
		servInfo.sin_addr = ip_to_num;
		servInfo.sin_port = htons(port_num);

		status = connect(client_socket, (sockaddr*)&servInfo, sizeof(servInfo));
		if (status != 0) {
			closesocket(client_socket);
			return CONNERR;
		};
	};

	/***********************************************************/

	int wait_Packet(char* buffer, short size)
	{
		short packet_size = recv(client_socket, buffer, size, 0);
		if (packet_size == SOCKET_ERROR) {
			close_Connection();
			return -1;
		};
		return packet_size;
	};

	/***********************************************************/

	int send_Packet(char* buffer, short size)
	{
		short packet_size = send(client_socket, buffer, size, 0);
		if (packet_size == SOCKET_ERROR) {
			close_Connection();
			return -1;
		};
		return packet_size;
	};

	/***********************************************************/

	void close_Network() { WSACleanup(); };

	/***********************************************************/
