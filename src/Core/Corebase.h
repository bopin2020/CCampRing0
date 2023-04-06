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
#pragma once
#include <ntddk.h>
#include <stdio.h>
#include "../DataModel.h"


class Corebase {
public:
	int ioctlCode = 0;
	virtual NTSTATUS ExecuteKernel(PVOID ioBuffer) = 0;
};

//
// 内核模型对 new delete重载
// ExAllocatePool ExFreePool
// 可以指定分页或非分页内存 
//
#define PoolTag 'ngt'
void* __cdecl operator new(size_t size, POOL_TYPE PoolType = PagedPool)
{
	return ExAllocatePoolWithTag(PoolType, size, PoolTag);
}

void __cdecl operator delete(void* pointer)
{
	ExFreePoolWithTag(pointer, PoolTag);
}