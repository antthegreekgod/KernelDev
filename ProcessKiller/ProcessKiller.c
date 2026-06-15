#include <ntifs.h>
#include <ntddk.h>
#include "ProcessKillerCommon.h"

VOID Unload(IN PDRIVER_OBJECT DriverObject);

NTSTATUS DeviceCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

NTSTATUS DeviceControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS STATUS = STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(RegistryPath);
	
	DriverObject->DriverUnload = Unload;

	DriverObject->MajorFunction[IRP_MJ_CREATE] = DeviceCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DeviceCreateClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControl;
	

	UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\ProcessKiller");
	PDEVICE_OBJECT DeviceObject;


	STATUS = IoCreateDevice(
		DriverObject,
		0,
		&DeviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&DeviceObject);

	if (!NT_SUCCESS(STATUS))
	{
		KdPrint(("[!] IoCreateDevice failed with error code: 0x%0.8X\n", STATUS));
		return STATUS;
	}

	UNICODE_STRING SymLink = RTL_CONSTANT_STRING(L"\\??\\ProcessKiller");

	STATUS = IoCreateSymbolicLink(
		&SymLink,
		&DeviceName);

	if (!NT_SUCCESS(STATUS))
	{
		KdPrint(("[!] IoCreateSymbolicLink failed with error code: 0x%0.8X\n", STATUS));
		IoDeleteDevice(DeviceObject);
		return STATUS;
	}

	KdPrint(("[+] Created Device with symlink: \\??\\ProcessKiller\n"));

	return STATUS;
}


NTSTATUS DeviceCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;

}

NTSTATUS DeviceControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	IO_STACK_LOCATION* stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS STATUS = STATUS_SUCCESS;

	

	switch (stack->Parameters.DeviceIoControl.IoControlCode)
	{
		case IOCTL_PROCESS_KILLER:
		{
			if (stack->Parameters.DeviceIoControl.InputBufferLength != sizeof(ULONG))
			{
				// we are expecting as input a ULONG representing the PID of the target process
				STATUS = STATUS_INVALID_BUFFER_SIZE;
				break;
			}

			ULONG* PID = (PULONG)(stack->Parameters.DeviceIoControl.Type3InputBuffer);

			KdPrint(("[+] Target PID: %lu\n", *PID));

			OBJECT_ATTRIBUTES ObjectAttr;
			ObjectAttr.RootDirectory = 0;
			ObjectAttr.Attributes = 0;
			ObjectAttr.ObjectName = NULL;
			ObjectAttr.SecurityDescriptor = NULL;
			ObjectAttr.SecurityQualityOfService = NULL;
			ObjectAttr.Length = sizeof(OBJECT_ATTRIBUTES);
			CLIENT_ID ClientId;
			ClientId.UniqueThread = 0;
			ClientId.UniqueProcess = ULongToHandle(*PID);
			HANDLE hProcess = NULL;

			// we need to close the opened handle with ZwClose later on.
			STATUS = ZwOpenProcess(
				&hProcess,
				1, // access mask to kill process
				&ObjectAttr,
				&ClientId);

			if (!NT_SUCCESS(STATUS))
			{
				ZwClose(hProcess); // closing the handle
				KdPrint(("ZwOpenProcess failed with error code: 0x%0.8X\n", STATUS));
				break;
			}

			STATUS = ZwTerminateProcess(
				hProcess,
				STATUS_SUCCESS
			);

			if (!NT_SUCCESS(STATUS))
			{
				KdPrint(("[!] ZwTerminateProcess failed with error code: 0x%0.8X\n", STATUS));
			}
			else {
				KdPrint(("[+] Killed Target Process ;)\n"));
			}

			ZwClose(hProcess); // closing the handle

			break;
		}

		default:
			STATUS = STATUS_INVALID_DEVICE_REQUEST;
			break;
	}


	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS;
}

VOID Unload(IN PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING SymLink = RTL_CONSTANT_STRING(L"\\??\\ProcessKiller");
	IoDeleteSymbolicLink(&SymLink);

	IoDeleteDevice(DriverObject->DeviceObject);
}

