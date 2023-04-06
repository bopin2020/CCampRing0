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
#pragma
#include <ntddk.h>
#include "../DataModel.h"
#include "Corebase.h"
#include "Core/Test/IOTest.hpp"
#include "Core/IOCTLProxy.hpp"


#define POOL_TAG 'bp20'
#define MAX_PATH 256

#pragma warning(disable: 4996)

class ioctl {
public:
    static NTSTATUS DispatchCreate(PDEVICE_OBJECT pDevObj, PIRP pIrp);
    static NTSTATUS DispatchClose(PDEVICE_OBJECT pDevObj, PIRP pIrp);
    static NTSTATUS DispatchIoctl(PDEVICE_OBJECT pDevObj, PIRP pIrp);
    static NTSTATUS CreateDriverObject(PDRIVER_OBJECT pDriver);
    static void DriverUnload(PDRIVER_OBJECT DriverObject);

    static LPWSTR DeviceName;
    static LPWSTR SymbolicLinkName;
    static LPWSTR DriverShortName;
    static void Init(LPWSTR name){}
private:

};

LPWSTR ioctl::DeviceName = L"\\Device\\CCampRing0";
LPWSTR ioctl::SymbolicLinkName = L"\\??\\CCampRing0";
LPWSTR ioctl::DriverShortName = L"CCampRing0";


void ioctl::DriverUnload(PDRIVER_OBJECT DriverObject)
{
    PDEVICE_OBJECT pDev;                                                
    UNICODE_STRING SymLinkName;                                         
    pDev = DriverObject->DeviceObject;
    IoDeleteDevice(pDev);                                               
    RtlInitUnicodeString(&SymLinkName, ioctl::SymbolicLinkName);     
    IoDeleteSymbolicLink(&SymLinkName);  
    DbgPrint("driver unload...");
}

NTSTATUS ioctl::DispatchCreate(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
    pIrp->IoStatus.Status = STATUS_SUCCESS;          
    DbgPrint("Dispatch function IRP_MJ_CREATE Ö´ÐÐ \n");
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);        
    return STATUS_SUCCESS;                   
}

NTSTATUS ioctl::DispatchClose(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
    pIrp->IoStatus.Status = STATUS_SUCCESS;          
    DbgPrint("Dispatch function IRP_MJ_CLOSE Ö´ÐÐ \n");
    IoCompleteRequest(pIrp, IO_NO_INCREMENT);        
    return STATUS_SUCCESS;                    
}

NTSTATUS ioctl::CreateDriverObject(PDRIVER_OBJECT pDriver)
{
    NTSTATUS status;
    PDEVICE_OBJECT pDevObj;
    UNICODE_STRING DeviceName;
    UNICODE_STRING SymLinkName;

    RtlInitUnicodeString(&DeviceName,ioctl::DeviceName);
    status = IoCreateDevice(pDriver,
            0,
            &DeviceName,
            FILE_DEVICE_UNKNOWN,
            0,
            TRUE,
            &pDevObj
        );

    pDevObj->Flags |= DO_BUFFERED_IO;
    RtlInitUnicodeString(&SymLinkName, ioctl::SymbolicLinkName);
    status = IoCreateSymbolicLink(&SymLinkName, &DeviceName);

    return status;
}

NTSTATUS ioctl::DispatchIoctl(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
    PIO_STACK_LOCATION pIrpStack;
    ULONG uIoControlCode;
    PVOID pIoBuffer;
    ULONG uInSize;
    ULONG uOutSize;

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

    uIoControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;

    pIoBuffer = pIrp->AssociatedIrp.SystemBuffer;

    uInSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;

    uOutSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;

    // ioctl queryinterface

    Corebase* corebase = IOCTLProxy::QueryInterface(uIoControlCode);
    if (corebase != nullptr)
    {
        status = corebase->ExecuteKernel(pIoBuffer);
        ExFreePool(corebase);
    }
    else
    {
        status = STATUS_INVALID_DEVICE_REQUEST;
    }
    if (status == STATUS_SUCCESS)
        pIrp->IoStatus.Information = uOutSize;
    else
        pIrp->IoStatus.Information = 0;

    pIrp->IoStatus.Status = status;
    IofCompleteRequest(pIrp, IO_NO_INCREMENT);
    return status;
}
