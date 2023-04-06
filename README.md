# CCampRing0

> All in code~



## How to use

> Firstly,please provide IOCTL Code with a datamodel structure when calling `DeviceIoControl`

```c
#define IOCTL_IO_Test CTL_CODE(FILE_DEVICE_UNKNOWN,0x800,METHOD_BUFFERED,FILE_ANY_ACCESS)
typedef struct _DataModel {
	ULONG Size;
	PVOID Data;
	ULONG ResultSize;
	PVOID Result;
	ULONG PID;
} DataModel, * PDataModel;
```



> Secondly, inherit Corebase abstract class

```c++
class IOTest : public Corebase
{
public:
	NTSTATUS ExecuteKernel(PVOID pIoBuffer)
	{
		// your code
	}
    IOTest(int ioctl)
	{
		ioctlCode = ioctl;
	}
}
```



> Finally, insert your datamodel - ioctl into IOCTLProxy.hpp

```c++
// 
// Initialize CodeBase concrete object
//
void IOCTLProxy::Init()
{
	LIST_ENTRY    linkListHead;
	InitializeListHead(&linkListHead);
	Insert<IOTest>(&linkListHead, IOCTL_IO_Test);
    // such as
    Insert<IOTest2>(&linkListHead, IOCTL_IO_Test2);
}
```

