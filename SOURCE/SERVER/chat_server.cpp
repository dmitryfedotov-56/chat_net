	/***********************************************************/
	/*                             Server                      */
	/***********************************************************/

	#include <fstream>
	#include <iostream>
	#include <string>
	#include "chat_access.h"
	#include "chat_server.h"

	#include "../CHAT/buffer.h"
	#include "../CHAT/definitions.h"

	using namespace std;

	/***********************************************************/
	/*                          Log Messages                   */
	/***********************************************************/

	ofstream logfile;
	static LogLevel loglevel;

	int open_Log(LogLevel level)
	{
		loglevel = level;
		logfile.open("chat_server_log", ios::out);
		if (!logfile.is_open()) return -1;
		return 0;
	};

	void log_Line(const char* s)
	{
		cout << s;
		logfile << s;
	};

	void end_Line()
	{
		cout << endl;
		logfile << endl;
	};

	void close_Log()
	{
		logfile.close();
	};

	/***********************************************************/

	Chat_Access* chat = nullptr;
	Session_Access* session = nullptr;
	Message_Collection_Access* collection = nullptr;
	Condition condition;

	void check_Session() 
	{
		if (session == nullptr)throw PROTOCOL_ERROR;
	};

	/***********************************************************/

	static void reply_OK()
	{
		reply.clear();
		put_Line(reply, R_OK);
	};

	void reply_Authenticated() 
	{
		reply.clear();
		put_Line(reply, R_AUTHENTICATED);
	};

	static void reply_Error()
	{
		reply.clear();
		put_Line(reply, R_PROTOCOL_ERROR);
	};

	/***********************************************************/
	/*                       Chat Operations                   */
	/***********************************************************/

	static void sign_Up() 
	{
		const char* user = get_Line(request);
		const char* pass = get_Line(request);
		if (loglevel == LogLevel::detaillog) {
			log_Line(" ");
			log_Line(user);
			end_Line();
		};
		session = chat->create_User(user, pass);
		reply_Authenticated();
	};

	/***********************************************************/

	static void sign_In()
	{
		const char* user = get_Line(request);
		const char* pass = get_Line(request);
		if (loglevel == LogLevel::detaillog) {
			log_Line(" ");
			log_Line(user);
			end_Line();
		};
		session = chat->start_Session(user, pass);
		reply_Authenticated();
	};

	/***********************************************************/
	/*                       Session Operations                */
	/***********************************************************/

	static void change_Password()
	{
		check_Session();
		const char* new_pass = get_Line(request);
		session->change_Password(new_pass);
		reply_OK();
	};

	/***********************************************************/

	static void send_Message() 
	{
		check_Session();
		const char* peer = get_Line(request);
		const char* text = get_Line(request);
		session->send_Message(peer, text);
		reply_OK();
	};

	/***********************************************************/
	
	static void broadcast_Message()
	{
		check_Session();
		const char* text = get_Line(request);
		session->broadcast_Message(text);
		reply_OK();
	};

	/***********************************************************/
	/*                       Message Selection                */
	/***********************************************************/

	static void send_Message_Data() 
	{
		reply.clear();
		if (collection == nullptr)			// no collection?
		{
			throw PROTOCOL_ERROR;			// wrong state, protocol error
			return;
		};

		if (!collection->has_Next())		// no more messages?		
		{
			put_Line(reply, R_END);			// end dialog
			return;
		};

		put_Line(reply, R_MESSAGE);			// message is available

		time_t time = collection->message_Time();
		string s = to_string(time);
		put_Line(reply, s.c_str());


		bool sent = collection->message_Sent();				// sent message?
		if (sent)
			put_Line(reply, SENT_YES);
		else 
			put_Line(reply, SENT_NO);

		bool received = collection->message_Received();		// received message?
		if (received)
			put_Line(reply, RCVD_YES);
		else
			put_Line(reply, RCVD_NO);

		const char* sender = collection->sender_Name();			// sender name
		put_Line(reply, sender);

		const char* recipient = collection->recipient_Name();	// recipient name
		if(recipient == nullptr)								// no name?
			put_Line(reply, ALL);								// empty line
		else
		put_Line(reply, recipient);								// send name

		const char* text = collection->message_Text();			// message text
		put_Line(reply, text);								
	};

	/***********************************************************/

	static bool get_Sent(const char* sent) 
	{
		if (strcmp(sent, SENT_YES) == 0) return true;
		if (strcmp(sent, SENT_NO) == 0) return false;
		throw PROTOCOL_ERROR;
	};

	/***********************************************************/

	static bool get_Received(const char* rcvd)
	{
		if (strcmp(rcvd, RCVD_YES) == 0) return true;
		if (strcmp(rcvd, RCVD_NO) == 0) return false;
		throw PROTOCOL_ERROR;
	};

	/***********************************************************/

	static Order get_Order(const char* order)
	{
		if (strcmp(order, DIRECT) == 0) return Order::direct;
		if (strcmp(order, REVERSE) == 0) return Order::reverse;
		throw PROTOCOL_ERROR;
	};

	/***********************************************************/

	static void select_Messages()
	{
		check_Session();

		// message parsing

		const char* sent = get_Line(request);			// include sent messages?
		const char* rcvd = get_Line(request);			// include received messages?
		const char* peer = get_Line(request);			// peer name	
		const char* numb = get_Line(request);			// number of nessages
		const char* sord = get_Line(request);			// lookup order

		// create conditions for messages selection

		condition.sent = get_Sent(sent);				// sent messages?
		condition.received = get_Received(rcvd);		// received messages?

		if (strcmp(peer, "") == 0)						// no peer name?
			condition.user = nullptr;
		else
			condition.user = peer;						// set peer name

		condition.number = stoi(numb);					// number of mesages

		Order order = get_Order(sord);					// order

		collection = session->select_Messages(&condition, order);		// select messages

		send_Message_Data();
	};

	/***********************************************************/
	/*                       Request Processing                */
	/***********************************************************/

	void log_Sent()
	{
		log_Line("SENT:");
		start_Reading();
		const char* answer = get_Line(reply);
		log_Line(answer);
		end_Line();
	};

	/***********************************************************/

	void end_Session()						// release session resources
	{
		delete collection;
		collection = nullptr;				// delete collection
		delete session;
		session = nullptr;					// delete session
	};

	/***********************************************************/

	static void error_Message(int code)
	{
		switch (code)
		{
		case USER_NOT_FOUND: put_Line(reply, R_USER_NOT_FOUND);
			break;
		case ALREADY_EXISTS: put_Line(reply, R_ALREADY_EXISTS);
			break;
		case WRONG_PASSWORD: put_Line(reply, R_WRONG_PASSWORD);
			break;
		case DATABASE_ERROR: put_Line(reply, R_DATABASE_ERROR);
			break;
		case EXCHANGE_ERROR: put_Line(reply, R_WRONG_PASSWORD);
			break;
		case PROTOCOL_ERROR: put_Line(reply, R_PROTOCOL_ERROR);
			break;
		default:;
			put_Line(reply, R_PROTOCOL_ERROR);
		};
		log_Sent();
	};

	/***********************************************************/

	#define END			0
	#define SIGN_IN		1
	#define SIGN_UP		2
	#define CHANGE	    3
	#define SEND		4
	#define BROADCAST	5
	#define SELECT		6
	#define NEXT		7


	int get_Request(const char* code)
	{
		if (strcmp(code, R_END) == 0) return END;
		if (strcmp(code, R_NEXT) == 0) return NEXT;
		if (strcmp(code, R_SIGN_IN) == 0) return SIGN_IN;
		if (strcmp(code, R_SIGN_UP) == 0) return SIGN_UP;
		if (strcmp(code, R_CHANGE) == 0) return CHANGE;
		if (strcmp(code, R_SEND) == 0) return SEND;
		if (strcmp(code, R_BROADCAST) == 0) return BROADCAST;
		if (strcmp(code, R_SELECT) == 0) return SELECT;
		throw PROTOCOL_ERROR;
	};

	/***********************************************************/

	void server_Processing(bool& finish)
	{
		try {
			reply.clear();
			start_Reading();
			const char* code = get_Line(request);	// message code
			int operation = get_Request(code);		// into numerical code
			
			if (loglevel == LogLevel::detaillog)	// logging
			{	
				log_Line("RCVD:");
				log_Line(code);
				switch (operation) 
				{
				case SIGN_IN:;
				case SIGN_UP:;
						break;
					default:
						end_Line();
				};
			};

			switch (operation)						// perform operation
			{
			case END:;								// end received
				finish = true;						// end of dialog
				delete collection;				
				collection = nullptr;				// delete collection
				delete session;
				session = nullptr;					// delete session
				break;
			case SIGN_UP:							// new user
				sign_Up();			
				break;
			case SIGN_IN:							// authenticate
				sign_In();
				break;
			case CHANGE:							// change password
				change_Password();	
				break;
			case SEND:								// send message
				send_Message();		
				break;
			case BROADCAST:							// boadcast message
				broadcast_Message();
				break;
			case SELECT:							// select messages
				select_Messages();
				break;
			case NEXT:								// next
				send_Message_Data();				// try to send message data
			};

			if (!finish) {
				if(loglevel == LogLevel::detaillog)log_Sent();
			};
		} 
		catch (int code)							// in case of error
		{ 
			error_Message(code);					// send error reply
		};
	};

	/***********************************************************/
	/*                       Open and Close Chat               */
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

#define ENGLISH	

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
			session3->broadcast_Message("Hello! I am user3");
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
			// show_Result(code);
			cout << "error" << endl;
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


