#ifndef C4C_ARENA_H
#define C4C_ARENA_H

#include "utils.hpp"


class Arena
{
public:
	int capacity;
	int offset;
	char *mem;
	Arena(int size)
	{
		this->mem = (char *)std::malloc(size);
		if (this->mem == nullptr)
		{
			DEBUG_PANIC("unable to allocate memory ");
		}

		this->offset = 0;
		this->capacity = size;
	}


	void *alloc(int size)
	{
		if ( this->offset + size > this->capacity )
		{
			if (this->mem != nullptr)
			{
				std::free(this->mem);
				this->capacity = this->offset = 0;
				DEBUG_PRINT(" too big a size  "," freeing before panic ");
			}

			this->mem = nullptr;

			DEBUG_PANIC(" arena out of memory  : increase size ");
		}

		char *ptr = this->mem + this->offset;
		this->offset += size;

		return ptr;
	}

	void reset()
	{
		this->offset = 0;
	}

	~Arena()
	{
		this->capacity = 0;
		this->offset = 0;
		 if (this->mem != nullptr)
		 {
			 std::free(this->mem);
		 }


		 this->mem = nullptr;
	}
};

#endif
