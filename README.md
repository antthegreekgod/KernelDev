# Kernel Driver Development

This repo includes some basic kernel drivers developed for learning purposes.
Current goals:

- Understand Driver Skeleton (Calling convention, Important Stuctures, etc.)
- Master BYOVD
- Read about Reflective Driver Injection (projects like kdmapper)

##  Driver Structure

**Entry Point** calling convention:

```c
#include <ntddk.h>

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
    NTSTATUS STATUS = STATUS_SUCCESS;
    
    // initialization code

    return STATUS;
}
```

The `DriverEntry` is the standard entry point for windows Kernel Drivers. This function is in charge of initializing the DriverObject (`DRIVER_OBJECT`). This includes setting up all desired callbacks, dispatching routines, etc. 
Lastly, this function should return an `NTSTATUS` value.

If we were to attempt to compile the previous snippet on Visual Studio 2026, we would get a warning preventing the final build.

- *Remarks*: With Kernel Driver Development, any Warning is treated as an Error, this behaviour can be overriden or disabled although it is not recommended.

The warning is raised because `DriverObject` and `RegistryPath` are declared within the driver's entry point but never used. The `UNREFERENCED_PARAMETER()` macro can be used in these situations to explicitly mark these parameters as unused and suppress the compiler warning:

```c
#include <ntddk.h>

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
    NTSTATUS STATUS = STATUS_SUCCESS;
    
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);

    KdPrint(("[+] Hello World From the Kernel!\n"));

    return STATUS;
}
```

Our initial basic driver (HelloWorld.sys) is now complete. After disabling the driver-signing policy, one can load the driver using the Service Configuration built-in tool `sc.exe`, and view the kernel traces with [`DebugView.exe`](https://learn.microsoft.com/es-es/sysinternals/downloads/debugview) from sysinternals.

**Unload** routine:

Attempting to stop the previous kernel driver will fail. This is because we have not declared the Unload routine.

- `PDRIVER_OBJECT->DriverUnload`: The *entry point for the driver's Unload routine*, if any, which is set by the `DriverEntry` routine when the driver initializes. If a driver has no Unload routine, this member is `NULL`.

```c
#include <ntddk.h>

void Unload(IN PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);
    KdPrint(("[+] Goodbye from Kernel\n"));
}



NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
   // initialization

    DeviceObject->DriverUnload = Unload;
    return STATUS;
}
```
- *Remarks*: The Unload routine must return `VOID` and by calling convention it receives the `DEVICE_OBJECT` object.

### References

- Windows Kernel Programming (author Pavel Yosifovich)
- [Ido Veltzman Blog](https://idov31.github.io/posts/lord-of-the-ring0-p1)
- [Kernel Access Please – BYOVD and Vulnerable Drivers](https://www.nsideattacklogic.de/en/kernel-access-please-byovd-and-vulnerable-drivers/)
