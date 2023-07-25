	#pragma once
	/***********************************************************/
	/*                            Network                      */
	/***********************************************************/
/*
	list of errors
*/
	#define BADADDR	-1		// invalid ip address
	#define WSERROR	-2		// WSA error
	#define SOCKERR -1		// socket error
	#define BINDERR	-2		// binding error
	#define LISTERR	-3		// listening error
	#define CONNERR	-4		// connection error
	#define WAITERR	-5		// waiting error
	#define TIMEEXP	-6		// timer expired

	/***********************************************************/

	int open_Network(const char* ip_addr, short port);	// initialize

	int open_Connection();		
/*
	The function puts the process in a waiting state for a short time.
	In case of failure it may return CONNERR. 
*/
	int wait_Connection(char* client_ip);
/*
	the function puts the process in a waiting state
*/
	int wait_Packet(char* buffer, short size);
/*
	the function puts the process in a waiting state. 
	The waiting time is limited by the value of timeout (msec). 
	If timeout == 0 waiting time is not limited.
	If the timer has expired, the function returns TIMEEXP.
*/
	int send_Packet(char* buffer, short size);			// send packet

	void close_Connection();							// close current connection

	void close_Network();								// release WSA

	/***********************************************************/



