	/***********************************************************/
	/*                         FILE Storage                     */
	/***********************************************************/

	#define _CRT_SECURE_NO_WARNINGS

	#include "storage_access.h"

	/***********************************************************/
/*
	CAUTION: this code could be good for VS and many compilers but
	it is not fully portable because of stat function usage.
*/
	/***********************************************************/

	#include <io.h>
	#include <stdio.h>
	#include <sys\types.h> 
	#include <sys\stat.h> 

	#include <string.h>

	#define DEBUG	0

	#if DEBUG
	#include <iostream>
	using namespace std;
	#endif

	/***********************************************************/

	class FILE_Storage : public Storage_Access {
	public:

		virtual BLOCK new_Data(unsigned size, void* src);

		virtual void del_Data(BLOCK id);

		virtual void put_Data(BLOCK id, unsigned size, void* src);

		virtual void get_Data(BLOCK id, unsigned size, void* dst);

		virtual BLOCK new_Line(const char* s);

		virtual const char* allocate_Line(BLOCK id);

		virtual void open_Storage();

		virtual void close_Storage();

		virtual bool empty_Storage();

	private:

		void read(unsigned size, void* dst)
		{
			size_t n = fread(dst, size, 1, file);
			if (n != 1)throw STORAGE_ERROR;
		};

		void write(unsigned size, void* src)
		{
			size_t n = fwrite(src, size, 1, file);
			if (n != 1)throw STORAGE_ERROR;
		};

		void seek(long offs)
		{
			int n = fseek(file, offs, SEEK_SET);
			if (n != 0)throw STORAGE_ERROR;
		};

		FILE* file = nullptr;
		size_t offset = 0;
		bool empty_storage = false;
	};

	/***********************************************************/
	/*
		data operations
	*/
	BLOCK FILE_Storage::new_Data(unsigned size, void* src)
	{
		BLOCK id = offset;
		int n = fseek(file, 0, SEEK_END);
		if (n != 0)throw STORAGE_ERROR;
		write(size, src);
		offset += size;
#if DEBUG
		cout << " block created id = " << id << " size = " << size << endl;
#endif
		return id;
	};

	void FILE_Storage::del_Data(BLOCK id) {};

	void FILE_Storage::put_Data(BLOCK id, unsigned size, void* src)
	{
		seek((long)id);
		write(size, src);
	};

	void FILE_Storage::get_Data(BLOCK id, unsigned size, void* dst)
	{
		seek((long)id);
		read(size, dst);
	};

	/***********************************************************/
	/*
		line operations
	*/
	BLOCK FILE_Storage::new_Line(const char* s)
	{
		BLOCK id = offset;
		unsigned size = strlen(s) + 1;
		int n = fseek(file, 0, SEEK_END);
		if (n != 0)throw STORAGE_ERROR;
		write(sizeof(unsigned), (void*)&size);
		offset += sizeof(unsigned);
		write(size, (void*)s);
		offset += size;
	#if DEBUG
			cout << " line  created id = " << id << " line = " << s << endl;
	#endif
		return id;
	};

	const char* FILE_Storage::allocate_Line(BLOCK id)
	{
		unsigned size = 0;
		seek((long)id);
		read(sizeof(unsigned), &size);
		void* ptr = malloc(size);
		if (ptr == nullptr)throw STORAGE_ERROR;
		read(size, ptr);
		return (const char*)ptr;
	};

	/***********************************************************/
	/*
		open and close
	*/
	#define FILENAME	"chat_data"

	void FILE_Storage::open_Storage()
	{
		if (file != nullptr)return;					// already open?

		struct stat st;
		file = fopen(FILENAME, "r+b");				// try to open
		if (file == nullptr)						// not open?
		{
			file = fopen(FILENAME, "w+b");			// create a new file
			empty_storage = true;
		};

		int code = stat(FILENAME, &st);				// try to get size
		if (code != 0) throw STORAGE_ERROR;			// on error throw exception
		offset = st.st_size;						// set offset
	};

	void FILE_Storage::close_Storage()
	{
		fclose(file);
		file = nullptr;
	};

	bool FILE_Storage::empty_Storage() { return empty_storage; };

	/***********************************************************/

	FILE_Storage storage;

	Storage_Access* open_Storage() 
	{
		storage.open_Storage();
		return &storage;
	};

	/***********************************************************/