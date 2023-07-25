/***********************************************************/
/*                         Server Side                     */
/***********************************************************/
	#include <fstream>
	#include <iostream>
	#include <stdio.h>
	#include "stdlib.h"
	#include "conio.h"
	#include <string>

	#include "chat_server.h"

	#include "../CHAT/network.h"
	#include "../CHAT/buffer.h"
	#include "../CHAT/definitions.h"

	#define CONFIG	"chat_server_conf"	// configuration file name

	#define ERROR	"ERROR"				// error logging
	#define DETAIL	"DETAIL"			// detailed logging

	#define TIMEOUT	300					// time to wait package


	/***********************************************************/
	/*                      Initialization                     */
	/***********************************************************/

	static void error_Message(const char* message) 
	{
		cout << message << endl;
		cout << "press any key" << endl;
		while (!_kbhit()) {};
	};

	/***********************************************************/

	LogLevel loglevel;			// loglevel

	void log_Message(const char* s)
	{
		if (loglevel != LogLevel::detaillog) return;
		log_Line(s);
		end_Line();
	};

	void log_Error(const char* s) 
	{
		log_Line(s);
		end_Line();
	};

	/***********************************************************/

	string server_addr = "";	// server IP address
	int server_port = 0;		// server port

	int get_Config()
	{
		string s;
		string level;
		ifstream config(CONFIG);
		if (!config.is_open()) {
			s = CONFIG;
			s += " not found";
			error_Message(s.c_str());
			return -1;
		};
		config >> server_addr;
		if (config.fail()) {
			error_Message("ip address required");
			return -1;
		};
		config >> server_port;
		if (config.fail()) {
			cout << "port number required" << endl;
			return -1;
		};
		if ((server_port <= 1024) || (server_port >= 49151)) {
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

		status = open_Network(server_addr.c_str(), server_port);
		if (status < 0) {
			if(status == BADADDR) 			
				error_Message("invalid ip");
			else
				error_Message("network error");
			return -1;
		};

		return 0;
	};

	/***********************************************************/
	/*                      Exchange Cycle                     */
	/***********************************************************/

	int wait_Request(void)
	{
		int status;							// socket status
		bool finish = false;
		char client_ip[22];

		log_Message("Starting to listen");

		status = wait_Connection(client_ip);
		if (status < 0) 
		{
			log_Error("listening error");
			return -1;
		};

		string s = "Connected from IP ";
		s += client_ip;
		log_Message(s.c_str());

		short packet_size = 0;

		finish = false;

		while (true) {

			// wait request
			packet_size = wait_Packet(request.data(), BUFF_SIZE, TIMEOUT);
			if (packet_size < 0){
				end_Session();
				close_Connection();
				log_Error("unable to receive packet, connection closed");
				return 0;
			};
/*
			cout << "RCVD:" << packet_size << endl;
			show_Buffer(request, packet_size);
*/
			// request processing
			server_Processing(finish);
			if (finish) 
			{
				close_Connection();
				return 0;
			};

			// send reply
			packet_size = send_Packet(reply.data(), reply.size());
			if (packet_size < 0) {
				end_Session();
				close_Connection();
				log_Error("unable to send packet, connection closed");
				return 0;
			};

/*			cout << "SENT:" << reply.size() << endl;
			show_Buffer(reply, reply.size());
*/
		};

		close_Connection();
		return 0;
	};

	/***********************************************************/

	void clean_All() 
	{
		close_Log();
		close_Chat_Access();
		close_Network();
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
			result = wait_Request();
			if (result < 0)return 0;
		};

		return 0;
	};

	/***********************************************************/

