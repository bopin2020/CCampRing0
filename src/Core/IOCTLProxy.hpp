/*
MIT License

Copyright (c) 2022 bopin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "Corebase.h"

typedef struct _IteratorIOCTL {
	LIST_ENTRY ListEntry;
	Corebase* Base;
}IteratorIOCTL, * PIteratorIOCTL;

class IOCTLProxy
{
public:
	static void Init();
	template <typename T>
	static void Insert(LIST_ENTRY* HeadList,int ioctl);
	static Corebase* QueryInterface(int ioctl);

public:
	static PIteratorIOCTL HeadIOCTL;
	static BOOLEAN HasInit;
};

PIteratorIOCTL IOCTLProxy::HeadIOCTL = nullptr;
BOOLEAN IOCTLProxy::HasInit = FALSE;

template <typename T>
void IOCTLProxy::Insert(LIST_ENTRY* HeadList,int ioctl)
{
	PIteratorIOCTL pIOCTL = (PIteratorIOCTL)ExAllocatePoolWithTag(PagedPool, sizeof(IteratorIOCTL), 'nn');
	pIOCTL->Base = new(PagedPool) T(ioctl);
	InsertHeadList(HeadList, &pIOCTL->ListEntry);
	if (!IOCTLProxy::HasInit)
	{
		IOCTLProxy::HasInit = TRUE;
		IOCTLProxy::HeadIOCTL = pIOCTL;
	}
}

// 
// Initialize CodeBase concrete object
//
void IOCTLProxy::Init()
{
	LIST_ENTRY    linkListHead;
	InitializeListHead(&linkListHead);
	Insert<IOTest>(&linkListHead, IOCTL_IO_Test);
}

Corebase* IOCTLProxy::QueryInterface(int ioctl)
{
	PIteratorIOCTL piteratorIOCTL = NULL;
	PLIST_ENTRY pListEntryStart;
	PLIST_ENTRY pListEntryEnd;
	pListEntryStart = pListEntryEnd = &IOCTLProxy::HeadIOCTL->ListEntry;
	do
	{
		piteratorIOCTL = (PIteratorIOCTL)CONTAINING_RECORD(pListEntryStart, IteratorIOCTL, ListEntry);
		if (piteratorIOCTL->Base->ioctlCode == ioctl)
		{
			break;
		}
		pListEntryStart = pListEntryStart->Flink;

	} while (pListEntryStart != pListEntryEnd);

	Corebase* base = piteratorIOCTL->Base;
	DbgBreakPoint();
	return base;
}
