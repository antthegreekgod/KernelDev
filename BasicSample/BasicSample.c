#include <ntddk.h>


// unload callback
void Unload(IN PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);

	KdPrint(("Goodbye From Kernel!\n"));
}


// initialisation routine (entry point)
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	NTSTATUS STATUS = STATUS_SUCCESS;

	DriverObject->DriverUnload = Unload; // unload routine when driver is unloaded

	KdPrint(("Hello From Kernel!\n")); // greetindgs from kernel XD

	// initialising struct and filling required params!
	RTL_OSVERSIONINFOW RtlOsVerInfo;
	RtlSecureZeroMemory(&RtlOsVerInfo, sizeof(RtlOsVerInfo));
	RtlOsVerInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);

	STATUS = RtlGetVersion(&RtlOsVerInfo);

	if (!NT_SUCCESS(STATUS))
	{
		KdPrint(("RtlGetVersion failed with error code: 0x%0.8X\n", STATUS));
		return STATUS;
	}

	KdPrint(("[i] Windows Version %d.%d (Build %d)\n", (DWORD64)RtlOsVerInfo.dwMajorVersion, (DWORD64)RtlOsVerInfo.dwMinorVersion, (DWORD64)RtlOsVerInfo.dwBuildNumber));

	return STATUS;
}