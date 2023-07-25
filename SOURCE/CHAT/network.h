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
	#define READERR	-7		// reading error
	#define SENDERR	-8		// sending error

	/***********************************************************/

	int open_Network(const char* ip_addr, short port);	
/*
		Returns 0 on success. 
		In case of failure it may return BADADDR or WSERROR
*/

	int open_Connection();
/*
		The function puts the process in a waiting state for a short time.
		Returns 0 on success, in case of failure it may return SOCKERR or CONNERR.
*/
	int wait_Connection(char* client_ip);
/*
		the function puts the process in a waiting state. 
		In case of successful connection returns 0, client_ip is the client ip address. 
		In case of failure it may return SOCKERR, LISTERR, BINDERR
*/
	int wait_Packet(char* buffer, short size, short timeout);
/*
		The function puts the process in a waiting state.
		The waiting time is limited by the value of timeout (msec).
		If timeout == 0 waiting time is not limited.
		If the timer has expired, the function returns TIMEEXP.
		Buffer size is limited by size parameter. In case of success returns packet size.
		In case of error it may return READERR.
*/
	int send_Packet(char* buffer, short size);
/*
		On success returns the number of bytes transmitted,
		in case of error it may return SENDERR.
*/	

	void close_Connection();								// close current connection

	void close_Network();									// release WSA


	/***********************************************************/



