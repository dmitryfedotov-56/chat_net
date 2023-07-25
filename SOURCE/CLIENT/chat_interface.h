	#pragma once
	/***********************************************************/
	/*                       Chat Interface                    */
	/***********************************************************/
/*
	chat interface definition
*/
	#include <time.h>

	#define ENGLISH	1	// interface language

	/***********************************************************/
	/*                       Operations                        */
	/***********************************************************/
/*
	These functions are calltd by the interface module
*/
	int open_Chat_Access();
/*
	open chat access, it can return errors
*/
	bool new_User(const char* user, const char* password);
/*
	in case if success a new user is created and user sesion is open
*/
	bool new_Session(const char* user, const char* password);
	/*
		in case if success a new sesion is open
	*/
	void change_Password(const char* password);
/*
	in case if success password is changed, the session should be open
*/
	void send_Message(const char* user, const char* text);
/*
	send a message to the user, the session should be open
*/
	void broadcast_Message(const char* text);
/*
	broadcast a message, the session should be open
*/

	struct Condition
	{
		bool sent = false;				// include sent messages only
		bool received = false;			// include received messages only
		unsigned number = 0;			// number of messages, 0 - no restriction
		const char* user = nullptr;		// user name
	};

	enum class Order { direct, reverse };

	void select_Messages(Condition& condition, Order order);
/*
	select messages that meet the condition, the session should be opeb
*/
	void end_Session();
/*
	close current user session
*/
	void close_Chat_Access();	// close chat

	/***********************************************************/
	/*                       Indication                        */
	/***********************************************************/
/*
	These functions should be implemented by the ibterface nodule
*/
	void show_Result(const char* s);	// show oparation result

	/***********************************************************/

	struct Message 
	{
		time_t time;
		bool sent;
		bool received;
		const char* sender;
		const char* recipient;
		const char* text;
	};

	void show_Message(Message& message);	// show next message

	/***********************************************************/


