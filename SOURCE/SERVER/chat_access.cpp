	/***********************************************************/
	/*                     Chat Access                      */
	/***********************************************************/

	#include "chat_access.h"
	#include "chat_interface.h"

	/***********************************************************/

	static Chat_Access* chat = nullptr;
	static Session_Access* session = nullptr;

	/***********************************************************/
	/*                    Session Operations                   */
	/***********************************************************/

	void change_Password(const char* password) 
	{
		try {
			session->change_Password(password);
		}
		catch (int code) { show_Result(code); };
	};

	/***********************************************************/

	void send_Message(const char* user, const char* text)
	{
		try {
			session->send_Message(user, text);
		}
		catch (int code) { show_Result(code); }
	};

	/***********************************************************/

	void broadcast_Message(const char* text)
	{
		try {
			session->broadcast_Message(text);
		}
		catch (int code) { show_Result(code); }
	};

	/***********************************************************/

	void select_Messages(Condition& condition, Order order) 
	{
		Message message;
		Message_Collection_Access* collection = nullptr;
		try {
			collection = session->select_Messages(&condition, Order::reverse);
		}
		catch (int code) { show_Result(code); }
	
		while (collection->has_Next())	// retrieve messages
		{
			message.time = collection->message_Time();
			message.sender = collection->sender_Name();
			message.recipient = collection->recipient_Name();
			message.sent = collection->message_Sent();
			message.received = collection->message_Received();
			message.text = collection->message_Text();
			show_Message(message);
		};
		delete collection;
	};


	/***********************************************************/

	void end_Session() 
	{
		delete session;
		session = nullptr;
	};

	/***********************************************************/
	/*                      Chat Operations                    */
	/***********************************************************/

	bool new_User(const char* user, const char* password) 
	{
		try {
			session = chat->create_User(user, password);
			return true;
		}
		catch (int code) 
		{
			show_Result(code);
			return false;
		};
	};

	/***********************************************************/

	bool new_Session(const char* user, const char* password) 
	{
		try {
			session = chat->start_Session(user, password);
			return true;
		}
		catch (int code)
		{
			show_Result(code);
			return false;
		};
	};
	
	/***********************************************************/
	/*                   Open and Close Chat                   */
	/***********************************************************/

	#ifdef	RUSSIAN

	void test_Messages()
	{
		Session_Access* session1 = nullptr;
		Session_Access* session2 = nullptr;
		Session_Access* session3 = nullptr;

		try {
			session1 = chat->create_User("user1", "pass1");
			session2 = chat->create_User("user2", "pass2");
			session3 = chat->create_User("user3", "pass3");

			session1->broadcast_Message("первый-всем");
			session2->broadcast_Message("второй-всем");
			session3->broadcast_Message("третий-всем");

			session1->send_Message("user2", "первый-второму");
			session2->send_Message("user1", "второй-первому");

			session3->broadcast_Message("снова третий-всем");

			session1->send_Message("user2", "снова первый-второму");
			session2->send_Message("user1", "снова второй-первому");

			session1->send_Message("user3", "первый-третьему");
			session2->send_Message("user3", "второй-третьему");
		}
		catch (int code)
		{
			error_Message(code);
			delete session1;
			delete session2;
			delete session3;
		};

		delete session1;
		delete session2;
		delete session3;
	};

	#endif

	/***********************************************************/

	#ifdef	ENGLISH

	void test_Messages()
	{
		Session_Access* session1 = nullptr;
		Session_Access* session2 = nullptr;
		Session_Access* session3 = nullptr;

		try {
			session1 = chat->create_User("user1", "pass1");
			session2 = chat->create_User("user2", "pass2");
			session3 = chat->create_User("user3", "pass3");

			session1->broadcast_Message("Hello! I am user1");
			session2->broadcast_Message("Hello! I am user2");
			session3->broadcast_Message("Hello! I am user1");
/*
			session1->send_Message("user2", "Hello, user2! This is user1");
			session2->send_Message("user1", "Hello, user1!");

			session3->broadcast_Message("Hello, everybody! I am user3");

			session1->send_Message("user2", "Hello, user2! How are you?");
			session2->send_Message("user1", "Hello, user1! I am OK");

			session1->send_Message("user3", "Hello, user3! This is user1");
			session2->send_Message("user3", "Hello, user1!");
*/
		}
		catch (int code)
		{
			show_Result(code);
			delete session1;
			delete session2;
			delete session3;
		};

		delete session1;
		delete session2;
		delete session3;
	};

	#endif

	/***********************************************************/

	void open_Chat_Access() 
	{
		chat = open_Chat();
		test_Messages();
	};

	void close_Chat_Access() { close_Chat(); };

	/***********************************************************/

