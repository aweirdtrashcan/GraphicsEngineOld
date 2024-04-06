#ifndef _ALLOCATOR_OPERATORS_
#define _ALLOCATOR_OPERATORS_
#include <sal.h>
#include <vcruntime.h>
#include "FreeListAllocator.h"

#define LOG_DEBUG 0

#if LOG_DEBUG
#include <iostream>
#endif

static __forceinline FreeListAllocator& Get() {
	static FreeListAllocator m_Instance = FreeListAllocator(1024 * 1024, FreeListAllocator::FIND_FIRST);
	return m_Instance;
}

_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(size) _VCRT_ALLOCATOR
void* __CRTDECL operator new(size_t size) {
#if LOG_DEBUG
	std::cout << "Allocating " << size << " bytes\n";
#endif
	return Get().Allocate(size, 8);
}

_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(size) _VCRT_ALLOCATOR
void* __CRTDECL operator new[](size_t size) {
#if LOG_DEBUG
	std::cout << "Allocating " << size << " bytes\n";
#endif
	return Get().Allocate(size, 8);
}

void __CRTDECL operator delete(void* ptr) {
	Get().Free(ptr);
}

void __CRTDECL operator delete[](void* ptr) {
	Get().Free(ptr);
}
#endif