	#pragma once
	/***********************************************************/
	/*                   Protocol Definitions                  */
	/***********************************************************/

	/*
		Command
	*/
	#define R_SIGN_UP			"sign_up"		// sign up
	#define R_SIGN_IN			"sign_in"		// sign in
	#define R_CHANGE			"change"		// change password
	#define R_SEND				"send"			// send a message
	#define R_BROADCAST			"broadcast"		// bradcast a message
	#define R_SELECT			"select"		// select messages
	#define R_NEXT				"next"			// next message
	/*
		Parameter
	*/
	#define SENT_YES			"sent_yes"		// sent messages
	#define SENT_NO				"sent_no"		// no sent mesages
	#define RCVD_YES			"rcvd_yes"		// received messages
	#define RCVD_NO				"rcvd_no"		// no received messages
	#define DIRECT				"direct"		// direct order
	#define REVERSE				"reverse"		// reverse order
	/*
		Reply
	*/
	#define R_OK					"OK"				// OK
	#define R_END					"end"				// end
	#define R_MESSAGE				"message"			// message data
	#define R_AUTHENTICATED			"authenticated"		// authenticated
	#define R_USER_NOT_FOUND		"name_not_found"	// user name not found
	#define R_ALREADY_EXISTS		"existing_name"		// user name already exists
	#define R_WRONG_PASSWORD		"wrong_password"	// wrong user password
	#define R_DATABASE_ERROR		"database_error"	// database access error
	#define R_EXCHANGE_ERROR		"exchange_error"	// communication error
	#define R_PROTOCOL_ERROR		"protocol_error"	// protocol error = for debugging

	/***********************************************************/

