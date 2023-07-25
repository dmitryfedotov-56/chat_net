	/***********************************************************/
	/*                             Client                      */
	/***********************************************************/

	#include <iostream>
	#include <string>
	#include "chat_interface.h"
	#include "chat_client.h"
	#include "../CHAT/buffer.h"
	#include "../CHAT/definitions.h"

	vector <char> userdata;

	/***********************************************************/
	/*                    Open and Close Access                */
	/***********************************************************/

	int open_Chat_Access() 
	{
		return open_Client();
	};

	void close_Chat_Access() { close_Client(); };

	/***********************************************************/
	/*                       Reply Processing                  */
	/***********************************************************/
/*
	positive result codes
*/
	#define OK				0
	#define MESSAGE			1
	#define END				2
	#define AUTHENTICATED	3
/*
	negative result codes
*/
	#define USER_NOT_FOUND		-1
	#define ALREADY_EXISTS		-2
	#define WRONG_PASSWORD		-3
	#define DATABASE_ERROR		-4
	#define PROTOCOL_ERROR		-5

	/***********************************************************/


	#define M_USER_NOT_FOUND		"user not found"
	#define M_ALREADY_EXISTS		"user already exists"
	#define M_WRONG_PASSWORD		"wrong user password"
	#define M_DATABASE_ERROR		"database error"
	#define M_PROTOCOL_ERROR		"protocol error"
	#define M_UNKNOWN_EVENT			"unknown event"

	const char* result_Message(int result)
	{
		switch (result)
		{
		case USER_NOT_FOUND: return M_USER_NOT_FOUND; break;
		case ALREADY_EXISTS: return M_ALREADY_EXISTS; break;
		case WRONG_PASSWORD: return M_WRONG_PASSWORD; break;
		case DATABASE_ERROR: return M_DATABASE_ERROR; break;
		case PROTOCOL_ERROR: return M_PROTOCOL_ERROR; break;
		default:
			return M_UNKNOWN_EVENT;
		};
	};

	/***********************************************************/

	void error_Message(const char* s) 
	{ 
		show_Result(s);		// pass message to the interface layer
	};

	/***********************************************************/

	int get_Reply(const char* code)
	{
		if (strcmp(code, R_OK) == 0) return OK;
		if (strcmp(code, R_END) == 0) return END;
		if (strcmp(code, R_MESSAGE) == 0) return MESSAGE;
		if (strcmp(code, R_AUTHENTICATED) == 0) return AUTHENTICATED;
		if (strcmp(code, R_USER_NOT_FOUND) == 0) return USER_NOT_FOUND;
		if (strcmp(code, R_ALREADY_EXISTS) == 0) return ALREADY_EXISTS;
		if (strcmp(code, R_WRONG_PASSWORD) == 0) return WRONG_PASSWORD;
		if (strcmp(code, R_DATABASE_ERROR) == 0) return DATABASE_ERROR;
		if (strcmp(code, R_PROTOCOL_ERROR) == 0) return PROTOCOL_ERROR;
		return PROTOCOL_ERROR;
	};

	/***********************************************************/

	void message_Data() 
	{
		Message message;

		const char* time = get_Line(reply);		// get timestamp
		message.time = stoull(time);			// to time_t;


		const char* sent = get_Line(reply);		// is it sent?

		if (strcmp(sent, SENT_YES) == 0)		// message sent
			message.sent = true;
		else 
		{
			if (strcmp(sent, SENT_NO) == 0)		// message not sent
				message.sent = false;
			else
				throw PROTOCOL_ERROR;			// protocol error
		};

		const char* rcvd = get_Line(reply);		// is it received?

		if (strcmp(rcvd, RCVD_YES) == 0)		// message received
			message.received = true;
		else
		{
			if (strcmp(rcvd, RCVD_NO) == 0)		// message not received
				message.received = false;
			else
				throw PROTOCOL_ERROR;			// protocol error
		};

		message.sender = get_Line(reply);		// message sender
		message.recipient = get_Line(reply);	// message recipient
		message.text = get_Line(reply);			// message text
		
		show_Message(message);
	};

	/***********************************************************/

	void send_End() 
	{ 
		request.clear(); 
		put_Line(request, R_END);		// send R_END
	};

	void send_Next() 
	{ 
		request.clear();
		put_Line(request, R_NEXT);		// send R_NEXT
	};

	void send_Data() { request = userdata; };	

	/***********************************************************/

	bool start_Dialog()
	{
		bool success = false;
		send_Request(success);
		return success;
	};

	/***********************************************************/
	/*
		operation codes
	*/
	#define SIGN_UP		1
	#define	SIGN_IN		2
	#define	CHANGE		3
	#define	SEND		4
	#define BROADCAST	5
	#define SELECT		6

	/***********************************************************/

	int operation = 0;

	void client_Processing(bool& finish, bool& success) 
	{
		start_Reading();
	
		const char* code = get_Line(reply);
		int result = get_Reply(code);

		// in case of error show result
		if(result < 0) show_Result(result_Message(result));

		switch (result)
		{
		case MESSAGE:;			// message received
			message_Data();		// get message data
			send_Next();		// send next
			break;

		case AUTHENTICATED:;	// user is authenticated
			switch (operation)
			{
			case SIGN_UP:;		// in case of SIGN_IP
			case SIGN_IN:;		// in case of SIGN_IN
				finish = true;	// operation is finished
			};
			send_Data();		// send user data
			break;

		default:				// else
			send_End();			// send end
			finish = true;		// dialog finish
		};
	
		switch (result)			// general result
		{
		case OK:;				
		case END:;
		case AUTHENTICATED:;
			success = true;
			break;
		};

		operation = 0;
	};

	/***********************************************************/
	/*                       Chat Operations                   */
	/***********************************************************/

	string current_username = "";
	string current_password = "";

	bool start_Auth() 
	{
		request.clear();
		put_Line(request, R_SIGN_IN);					// send sign in
		put_Line(request, current_username.c_str());	// current username
		put_Line(request, current_password.c_str());	// current pasword
		return start_Dialog();
	};

	/***********************************************************/

	bool new_User(const char* username, const char* password)
	{
		operation = SIGN_UP;
		current_username = username;		// store username
		current_password = password;		// store password
		request.clear();
		put_Line(request, R_SIGN_UP);		// send sign up
		put_Line(request, username);		// username
		put_Line(request, password);		// password
		userdata.clear();
		put_Line(userdata, R_END);			// end on completion
		return start_Dialog();
	};

	/***********************************************************/

	bool new_Session(const char* username, const char* password)
	{
		operation = SIGN_UP;
		current_username = username;	// store username
		current_password = password;	// store password
		userdata.clear();
		put_Line(userdata, R_END);		// end on completion
		return start_Auth();
	};

	/***********************************************************/
	/*                     Session Operations                  */
	/***********************************************************/

	void change_Password(const char* password) 
	{
		operation = CHANGE;
		userdata.clear();
		put_Line(userdata, R_CHANGE);
		put_Line(userdata, password);				// new pasword
		if (start_Auth()) {							// on success
			current_password = password;			// change password
		};
	};

	/***********************************************************/

	void send_Message(const char* peer, const char* text)
	{
		operation = SEND;
		userdata.clear();
		put_Line(userdata, R_SEND);					// command
		put_Line(userdata, peer);					// recipient
		put_Line(userdata, text);					// message text
		start_Auth();
	};

	/***********************************************************/

	void broadcast_Message(const char* text) 
	{
		operation = BROADCAST;
		userdata.clear();
		put_Line(userdata, R_BROADCAST);			// command
		put_Line(userdata, text);					// message text
		start_Auth();		
	};

	/***********************************************************/

	void select_Messages(Condition& condition, Order order) 
	{
		operation = SELECT;
		userdata.clear();
		put_Line(userdata, R_SELECT);				// command

		if (condition.sent)
			put_Line(userdata, SENT_YES);			// sent mesages
		else
			put_Line(userdata, SENT_NO);			// no sent messages

		if (condition.received)
			put_Line(userdata, RCVD_YES);			// received messages
		else
			put_Line(userdata, RCVD_NO);			// no received messages

		if (condition.user == nullptr)			
			put_Line(userdata, "");					// no user name
		else
			put_Line(userdata, condition.user);		// include user name

		string s = to_string(condition.number);		// number of messages
		put_Line(userdata, s.c_str());

		switch (order)								// order
		{
		case Order::direct:							
			put_Line(userdata, DIRECT);				// direct order
			break;
		case Order::reverse:
			put_Line(userdata, REVERSE);			// reverse order
			break;
		};
	
		start_Auth();
	};

	/***********************************************************/

	void end_Session() 	
	{
		string current_username = "";
		string current_password = "";
	};

	/***********************************************************/
