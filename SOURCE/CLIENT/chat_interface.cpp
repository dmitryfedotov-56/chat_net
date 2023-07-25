	/***********************************************************/
	/*                     Chat Interface                      */
	/***********************************************************/

	#define _CRT_SECURE_NO_WARNINGS

	#include <iostream>
	using namespace std;

	#include <time.h>
	#include <conio.h>
	#include <stdlib.h>
	#include <string>

	#include "chat_interface.h"

	/***********************************************************/
	/*
		locale data
	*/
#if RUSSIAN

	#define OK				"OK"
	#define NOT_FOUND		"пользователь не найден"
	#define USER_EXISTS		"пользователь уже существует"
	#define WRONG_PASS		"неверный пароль"
	#define UNKNOWN_ERR		"неизвестная ошибка"
	#define DATABASE_ERR	"ошибка доступа к БД"
	#define EXCHANGE_ERR	"ошибка обмена с сервером"
	#define PROTOCOL_ERR	"ошибка протокола"
	#define CONFDATA_ERR	"ошибочные конфигурационные данные"


	#define OPERATION		"операция"
	#define SIGN_IN			"вход"
	#define SIGN_UP			"регистрация пользователя"
	#define EXIT_APP		"выход из приложения"
	#define USERNAME		"имя"
	#define PASSWORD		"пароль"

	#define CHANGEPASS		"изменение пароля"
	#define PASSCHANGED		"пароль изменен" 
	#define ENDSESSION		"завершение сессии"
	#define SENDMESSAGE		"отправить сообщение"
	#define BROADCAST		"отправить всем"
	#define ALLMESSAGES		"все сообщения"
	#define SENTMESSAGES	"отправленные сообщения"
	#define RCVDMESSAGES	"полученные сообщения"
	#define CHATUSER		"пользователь (Enter - все)"
	#define NMESSAGE		"число сообщений (Enter - все)"
	#define MESSTEXT		"текст"
	#define ALLOFTHEM		"все"
	#define NOMESSAGES		"сообщения отсутствуют"

#endif

#ifdef	ENGLISH

	#define OK				"OK"
	#define NOT_FOUND		"user not found"
	#define USER_EXISTS		"user already exists"
	#define WRONG_PASS		"wrong password"
	#define UNKNOWN_ERR		"unknown error"
	#define DATABASE_ERR	"database error"
	#define EXCHANGE_ERR	"communication error"
	#define PROTOCOL_ERR	"protocol error"
	#define CONFDATA_ERR	"wrong configuration data"

	#define OPERATION		"operation"
	#define SIGN_IN			"sign in"
	#define SIGN_UP			"sign up"
	#define EXIT_APP		"exit application"
	#define USERNAME		"login"
	#define PASSWORD		"password"

	#define CHANGEPASS		"change password"
	#define PASSCHANGED		"password changed" 
	#define ENDSESSION		"end session"
	#define SENDMESSAGE		"send message"
	#define BROADCAST		"send to all"
	#define ALLMESSAGES		"all messages"
	#define SENTMESSAGES	"sent messages"
	#define RCVDMESSAGES	"received messages"
	#define CHATUSER		"user (Enter - all)"
	#define NMESSAGE		"number of messages (Enter - все)"
	#define MESSTEXT		"text"
	#define ALLOFTHEM		"all"
	#define NOMESSAGES		"no messages"

#endif

	/***********************************************************/

	void show_Result(const char* s)
	{
		cout << " " << s << endl;
	};

	/***********************************************************/

	void show_Bar()
	{
		// cout << " ";
		for (unsigned i = 0; i < 60; i++)cout << "-";
		cout << endl;
	};

	/***********************************************************/
	/*                  Session Operations                     */
	/***********************************************************/

	string current_user = "";

	char get_Session_Operation()
	{
		show_Bar();
		cout << " 0 : " << ENDSESSION << endl;
		cout << " 1 : " << SENDMESSAGE << endl;
		cout << " 2 : " << BROADCAST << endl;
		cout << " 3 : " << ALLMESSAGES << endl;
		cout << " 4 : " << RCVDMESSAGES << endl;
		cout << " 5 : " << SENTMESSAGES << endl;
		cout << " 6 : " << CHANGEPASS << endl;

		show_Bar();

		cout << " " << current_user << " " << OPERATION << " : ";

		char c;
		while (true)	// get valid input
		{
			c = _getch();
			switch (c)
			{
			case '0':
				cout << ENDSESSION << endl;
				return c;
			case '1':
				cout << SENDMESSAGE << endl;
				return c;
			case '2':
				cout << BROADCAST << endl;
				return c;
			case '3':
				cout << ALLMESSAGES << endl;
				return c;
			case '4':
				cout << RCVDMESSAGES << endl;
				return c;
			case '5':
				cout << SENTMESSAGES << endl;
				return c;
			case '6':
				cout << CHANGEPASS << endl;
				return c;
			};
		};
	};

	/***********************************************************/

	void send_Message() 
	{
		string user;
		string text;
		cout << " " << CHATUSER << ": ";
		getline(cin, user);
		cout << " " << MESSTEXT << ": ";
		getline(cin, text);
		send_Message(user.c_str(), text.c_str());
	};

	/***********************************************************/

	void broadcast_Message() 
	{
		string text;
		cout << " " << MESSTEXT << ": ";
		getline(cin, text);
		broadcast_Message(text.c_str());
	};

	/***********************************************************/

	bool nomessages = true;

	#define BUFFSIZE	40

	void show_Message(Message& message)
	{

		if (nomessages) nomessages = false;

		show_Bar();

		struct tm* local;
		char s[BUFFSIZE];
		const time_t time = message.time;	// message time
		local = localtime(&time);
		strftime(s, BUFFSIZE, "%d.%m.%Y %H:%M:%S", local);
				cout << " " << s << " ";

		if (message.sent)					// sender and recipient
		{
			cout << " ";
			cout << message.sender;
			cout << " -> ";
			cout << message.recipient;
		};

		if (message.received)				//  recipient and sender
		{
			cout << " ";
			cout << message.recipient;
			cout << " <- ";
			cout << message.sender;
		};

		cout << endl;

		cout << " ";
		cout << message.text << endl;		// message text
	};

	/***********************************************************/

#define BS	8
#define CR	13

	unsigned get_line(char* s)
	{
		char c = 0;
		unsigned n = 0;
		while (n < BUFFSIZE - 1)
		{
			c = _getch();
			switch (c)
			{
			case 13:			// CR - end line
				s[n] = 0;
				return n;
			case BS:			// backspace - one symbol less
				if (n != 0) {
					_putch(BS);
					_putch(' ');
					_putch(BS);
					n--;
				};
				break;
			default:			// default - one more symbol
				_putch(c);
				s[n] = c;
				n++;
			};
		};
		s[n] = 0;
		return n;
	};
	
	/***********************************************************/

	void show_Messages(bool sent, bool received) 
	{
		unsigned n;
		Condition condition;
		char username[BUFFSIZE];

		nomessages = true;

		condition.sent = sent;
		condition.received = received;

		cout << " " << CHATUSER << ": ";		// get user
		n = get_line(username);
		if (n == 0)
		{
			condition.user = nullptr;			// all users
			cout << ALLOFTHEM << endl;
		}
		else 
		{
			cout << endl;
			condition.user = username;			// some particular user
		};

		// show_Bar();

		select_Messages(condition, Order::reverse);

		if (nomessages) cout << " " << NOMESSAGES << endl;
	};

	/***********************************************************/

	void change_Password() 
	{
		string password;
		cout << " " << PASSWORD << ": ";
		getline(cin, password);
		change_Password(password.c_str());
	};

	/***********************************************************/

	void start_User_Session(const char* user) 
	{
		char c;
		current_user = user;
		while (true)
		{
			c = get_Session_Operation();
 			switch (c)
			{
			case '0':
				end_Session();
				return;
			case '1':
				send_Message();					// message 
				break;
			case '2':
				broadcast_Message();
				break;
			case '3':
				show_Messages(false, false);	// all messages
				break;
			case '4':
				show_Messages(false, true);		// received mesages only
				break;
			case '5':
				show_Messages(true, false);		// sent messages only
				break;
			case '6':
				change_Password();
				break;
			};
		};
	};
	
	/***********************************************************/
	/*                    Сhat Operations                      */
	/***********************************************************/

	char get_Chat_Operation()
	{
		show_Bar();
		cout << " 0 : " << EXIT_APP << endl;
		cout << " 1 : " << SIGN_IN << endl;
		cout << " 2 : " << SIGN_UP << endl;
		show_Bar();

		cout << " " << OPERATION << " : ";

		char c;
		while (true)	// get valid input
		{
			c = _getch();
			switch (c)
			{
			case '0':
				cout << EXIT_APP << endl;
				return c;
			case '1':
				cout << SIGN_IN << endl;
				return c;
			case '2':
				cout << SIGN_UP << endl;
				return c;
			};
		};
	};

	/***********************************************************/

	void get_User_Data(string* username, string* password)
	{
		cout << " " << USERNAME << ": ";
		getline(cin, *username);
		cout << " " << PASSWORD << ": ";
		getline(cin, *password);
	};

	/***********************************************************/

	void sign_In()
	{
		string username;
		string password;
		get_User_Data(&username, &password);
		if (new_Session(username.c_str(), password.c_str()))
		{
			start_User_Session(username.c_str());
		};
	};

	/***********************************************************/

	void sign_Up()
	{
		string username;
		string password;
		get_User_Data(&username, &password);
		if (new_User(username.c_str(), password.c_str()))
		{
			start_User_Session(username.c_str());
		};
	};

	/***********************************************************/

	void start_Chat() 
	{
		char c;
		while (true)
		{
			c = get_Chat_Operation();
			switch (c)
			{
			case '0': 
				return;
			case '1': sign_In();
				break;
			case '2': sign_Up();
				break;
			};
		};
	};

	/***********************************************************/

	int main() 
	{
		setlocale(LC_ALL, "");
		system("chcp 1251");
 	
		cout << endl;

		atexit(close_Chat_Access);

		int result = open_Chat_Access();	// open chat access
		if (result < 0)	return 0;			// in case of error
	
		start_Chat();						// open chat interface

		close_Chat_Access();				// close chat access
		return 0;
	};

	/***********************************************************/



