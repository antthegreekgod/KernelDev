#include <ntddk.h>


void Unload(PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);
	KdPrint(("Goodbye from Kernel\n"));
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	NTSTATUS STATUS = STATUS_SUCCESS;

	DriverObject->DriverUnload = Unload;

	KdPrint(("HelloWorld from Kernel\n"));

	return STATUS;
}