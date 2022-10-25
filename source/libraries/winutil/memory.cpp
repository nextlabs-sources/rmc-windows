

#include "stdafx.h"

#include <nx\winutil\memory.h>

using namespace NX;
using namespace NX::win;


MemoryStatus::MemoryStatus()
{
	memset(&_status, 0, sizeof(MEMORYSTATUSEX));
	_status.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&_status);
}

MemoryStatus::MemoryStatus(const MemoryStatus& rhs)
{
	memcpy(&_status, &rhs._status, sizeof(MEMORYSTATUSEX));
}

MemoryStatus::MemoryStatus(MemoryStatus&& rhs)
{
	memcpy(&_status, &rhs._status, sizeof(MEMORYSTATUSEX));
	memset(&rhs._status, 0, sizeof(MEMORYSTATUSEX));
}

MemoryStatus::~MemoryStatus()
{
}

MemoryStatus& MemoryStatus::operator = (const MemoryStatus& rhs)
{
	if (this != &rhs) {
		memcpy(&_status, &rhs._status, sizeof(MEMORYSTATUSEX));
	}
	return *this;
}

MemoryStatus& MemoryStatus::operator = (MemoryStatus&& rhs)
{
	if (this != &rhs) {
		memcpy(&_status, &rhs._status, sizeof(MEMORYSTATUSEX));
		memset(&rhs._status, 0, sizeof(MEMORYSTATUSEX));
	}
	return *this;
}
