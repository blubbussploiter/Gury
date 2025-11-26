
#include <G3D/System.h>
#include "luaAllocator.h"

void* LuaAllocator::_alloc(LuaAllocator* ud, void* ptr, unsigned int osize, unsigned int nsize)
{
	void* result;

	result = realloc(ptr, nsize);
	ud->heapSize += nsize - osize;
	if (!osize)
	{
		++ud->heapCount;
	}
	if (!nsize)
	{
		--ud->heapCount;
	}
	if (ud->maxHeapSize < ud->heapSize)
	{
		ud->maxHeapSize = ud->heapSize;
	}
	if (ud->maxHeapCount <= ud->heapCount)
	{
		ud->maxHeapCount = ud->heapCount;
	}
	return result;
}

void* LuaAllocator::alloc(void* ud, void* ptr, unsigned int osize, unsigned int nsize)
{
	return _alloc((LuaAllocator*)ud, ptr, osize, nsize);
}
