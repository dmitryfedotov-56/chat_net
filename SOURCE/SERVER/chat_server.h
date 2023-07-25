	#pragma once
	/***********************************************************/
	/*                             Server                      */
	/***********************************************************/

	void open_Chat_Access();				// open chat access

	void close_Chat_Access();				// close chat access

	void server_Processing(bool& finish);
/*
	this finction is called by send_Request function 
	for client request processing and reply forming 
*/
	void end_Session();
/*
	this function id called in case of error to close client session
*/
	/***********************************************************/
/*
	log messages
*/
	enum class LogLevel { errorlog, detaillog };

	int open_Log(LogLevel level);

	void log_Line(const char* s);

	void end_Line();

	void close_Log();

	/***********************************************************/


