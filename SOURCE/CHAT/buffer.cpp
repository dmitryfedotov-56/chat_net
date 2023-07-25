	#include <iostream>
	#include <string.h>
	#include "../CHAT/buffer.h"

	/***********************************************************/

	vector <char> request(BUFF_SIZE);
	vector <char> reply(BUFF_SIZE);

	/***********************************************************/

	static unsigned cntr = 0;

	void start_Reading() { cntr = 0; };

	bool end_Buffer(vector <char>& buffer)
	{
		int size = buffer.size();
		if (cntr >= buffer.size())return true;
		return false;
	};

	const char* get_Line(vector <char>& buffer)
	{
		if (cntr == buffer.size())return "";
		const char* s = (const char*)buffer.data();
		s = &s[cntr];
		cntr += strlen(s) + 1;
		return s;
	};

	void put_Line(vector <char>& buffer, const char* s)
	{
		unsigned leng = strlen(s) + 1;
		for (unsigned i = 0; i < leng; i++)
		{
			buffer.push_back(s[i]); cntr++;
		};
	};

	void show_Buffer(vector <char>& buffer, unsigned size)
	{
		const char* s;
		start_Reading();
		while (cntr < size)
		{
			s = get_Line(buffer);
			cout << " " << s << endl;
		};
		cout << endl;
	};

	/***********************************************************/

