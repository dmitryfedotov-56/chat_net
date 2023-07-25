/***********************************************************/
/*                         RAM Storage                     */
/***********************************************************/
	#include <fstream>
	#include <iostream>
	#include <WinSock2.h>
	#include <WS2tcpip.h>
	#include <stdio.h>
	#include "stdlib.h"
	#include "conio.h"
	#include <string>


	#include "chat_server.h"
	#include "../CHAT/buffer.h"
	#include "../CHAT/definitions.h"

	#pragma comment(lib, "Ws2_32.lib")

	#define CONFIG	"chat_server_conf"	// configuration file name

	#define ERROR	"ERROR"				// error logging
	#define DETAIL	"DETAIL"			// detailed logging

	LogLevel loglevel;

	string ip_addr = "";		// server IP address
	int port_num = 0;			// server port

	string level;

	in_addr ip_to_num;
	WSADATA wsData;

	/***********************************************************/
	/*                      Initialization                    */
	/***********************************************************/

	static void error_Message(const char* message) 
	{
		cout << message << endl;
		cout << "press any key" << endl;
		while (!_kbhit()) {};
	};

	/***********************************************************/

	int get_Config()
	{
		string s;
		ifstream config(CONFIG);
		if (!config.is_open()) {
			s = CONFIG;
			s += " not found";
			error_Message(s.c_str());
			return -1;
		};
		config >> ip_addr;
		if (config.fail()) {
			error_Message("ip address required");
			return -1;
		};
		config >> port_num;
		if (config.fail()) {
			cout << "port number required" << endl;
			return -1;
		};
		if ((port_num <= 1024) || (port_num >= 49151)) {
			error_Message("invalid port number");
			return -1;
		};
		config >> level;
		if (config.fail()) {
			error_Message("loglevel required");
			return -1;
		};
		if (strcmp(level.c_str(), ERROR) == 0) {	
			loglevel = LogLevel::errorlog;
			return 0; 
		};
		if (strcmp(level.c_str(), DETAIL) == 0) {
			loglevel = LogLevel::detaillog;
			return 0;
		};
		s = "loglevel should be ";
		s += ERROR;
		s += " or ";
		s += DETAIL;
		error_Message(s.c_str());
		return -1;
	};

	/***********************************************************/

	int init_Server() 
	{
		int status;							// status

		status = get_Config();				// config data
		if (status < 0)return -1;

		status = open_Log(loglevel);
		if (status < 0) {
			error_Message("logfile error");
			return -1; 
		};

		open_Chat_Access();					// open database

		// convert ip address
		status = inet_pton(AF_INET, ip_addr.c_str(), &ip_to_num);
		if (status <= 0) {
			error_Message("invalid ip address");
			return -1;
		};

		// get WS32 library
		status = WSAStartup(MAKEWORD(2, 2), &wsData); // WinSock initialization
		if (status != 0) {
			error_Message("winsocket error");
			return -1;
		};
		return 0;
	};

	/***********************************************************/
	/*                      Exchange Cycle                     */
	/***********************************************************/

	void wait_Request(void)
	{
		int status;							// socket status
		bool finish = false;

		SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);		// socket initialization
		if (server_socket == INVALID_SOCKET) {
			closesocket(server_socket);
			log_Line("wsocket error"); 
			end_Line();
			return;
		};

		// Server socket binding
		sockaddr_in servInfo;
		ZeroMemory(&servInfo, sizeof(servInfo));	// Initializing servInfo structure

		servInfo.sin_family = AF_INET;
		servInfo.sin_addr = ip_to_num;
		servInfo.sin_port = htons(port_num);

		status = bind(server_socket, (sockaddr*)&servInfo, sizeof(servInfo));
		if (status != 0) {
			closesocket(server_socket);
			log_Line("binding error");
			end_Line();
			return;
		};

		// start to listen 
		status = listen(server_socket, SOMAXCONN);
		if (status != 0) {
			closesocket(server_socket);
			log_Line("listening error");
			end_Line();
			return;
		};
	
		if(loglevel == LogLevel::detaillog)log_Line("Listening..."); 
		end_Line();

		// client socket creation and acception in case of connection
		sockaddr_in clientInfo;
		ZeroMemory(&clientInfo, sizeof(clientInfo));	// Initializing clientInfo structure

		int clientInfo_size = sizeof(clientInfo);

		SOCKET client_socket = accept(server_socket, (sockaddr*)&clientInfo, &clientInfo_size);

		if (client_socket == INVALID_SOCKET) {
			closesocket(server_socket);
			closesocket(client_socket);
			log_Line("connection error");
			end_Line();
			return ;
		};
	
		char client_ip[22];
		inet_ntop(AF_INET, &clientInfo.sin_addr, client_ip, INET_ADDRSTRLEN);	
		if(loglevel == LogLevel::detaillog)log_Line("Client connected with IP address ");
		log_Line(client_ip);
		end_Line();

		short packet_size = 0;

		finish = false;

		while (true) {

			// receive request
			packet_size = recv(client_socket, request.data(), BUFF_SIZE, 0);
			if (packet_size == SOCKET_ERROR) 
			{
				closesocket(server_socket);
				closesocket(client_socket);
				log_Line("unable to receive packet, connection closed");
				end_Line();
				return ;
			};
/*
			cout << "RCVD:" << packet_size << endl;
			show_Buffer(request, packet_size);
*/
			// request processing
			server_Processing(finish);
			if (finish) 
			{
				closesocket(server_socket);
				closesocket(client_socket);
				// log_Line("connection closed by client");
				end_Line();
				return;
			};

			// send reply
			packet_size = send(client_socket, reply.data(), reply.size(), 0);
			if (packet_size == SOCKET_ERROR) {
				closesocket(server_socket);
				closesocket(client_socket);
				log_Line("unable to send packet, connection closed");
				end_Line();
				return;
			};

/*			cout << "SENT:" << reply.size() << endl;
			show_Buffer(reply, reply.size());
*/
		};

		closesocket(server_socket);
		closesocket(client_socket);
		return;
	};

	/***********************************************************/

	void clean_All() 
	{
		close_Log();
		close_Chat_Access();
		WSACleanup();
	};

	/***********************************************************/

	int main() 
	{
		int result = init_Server();
		if (result < 0) return 0;

		atexit(clean_All);

		cout << "Server started" << endl;

		while (true) 
		{
			wait_Request();
		};

		return 0;
	};

	/***********************************************************/

