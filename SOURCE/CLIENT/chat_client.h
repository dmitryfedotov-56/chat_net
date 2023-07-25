	#pragma once
	/***********************************************************/
	/*                        Chat_Client                      */
	/***********************************************************/
/*
	This file defines intefcace betwwen chat_client and client_net
*/
	/***********************************************************/
	/*                        Network Layer                    */
	/***********************************************************/

	int open_Client();			
	
	// open network access, it can return WINSOCK arror and IP_ADDR_ERROR

	void close_Client();		// close network access

	void send_Request(bool& success);			
/*
	starts a dialog with the server, returns its result
*/
	/***********************************************************/
	/*                        Protocol Layer                  */
	/***********************************************************/

	void client_Processing(bool& finish, bool& success);
/*
	this function is called by send_Request
	processing the response and forming the next request, 
	at the end of the dialog the flag is set, and the result of the dialog is determined
*/
	/***********************************************************/
	/*                        Indication                       */
	/***********************************************************/

	void error_Message(const char* s);

	/***********************************************************/


