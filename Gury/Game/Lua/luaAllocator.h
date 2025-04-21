#pragma once

#include <cstdint>

class LuaAllocator
{
private:

	uint32_t heapSize;
	uint32_t heapCount;
	uint32_t maxHeapSize;
	uint32_t maxHeapCount;

public:

	void getHeapStats(uint32_t *heapSize, uint32_t * heapCount, uint32_t* maxHeapSize, uint32_t* maxHeapCount)
	{
		*heapSize = this->heapSize;
		*heapCount = this->heapCount;
		*maxHeapSize = this->maxHeapSize;
		*maxHeapCount = this->maxHeapCount;
	}
		void clearHeapMax()
	{
		maxHeapCount = 0;
		maxHeapSize = 0;
	}

	static void* _alloc(LuaAllocator* ud, void* ptr, unsigned int osize, unsigned int nsize);
	static void* alloc(void* ud, void* ptr, unsigned int osize, unsigned int nsize);
};