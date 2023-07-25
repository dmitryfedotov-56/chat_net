	#pragma once
	/***********************************************************/
	/*                           Buffers                       */
	/***********************************************************/

	#include <vector>

	using namespace std;
	
	#define BUFF_SIZE	1024

	void start_Reading();

	const char* get_Line(vector <char>& buffer);

	void put_Line(vector <char>& buffer, const char* s);

	void show_Buffer(vector <char>& buffer, unsigned size);

	extern vector <char> request;
	extern vector <char> reply;

	/***********************************************************/



