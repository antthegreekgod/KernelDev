#include <windows.h>
#include <stdio.h>
#include "..\ProcessKiller\ProcessKillerCommon.h"


int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("[!] Usage: %s <PID 2 Kill>\n", argv[0]);
		return EXIT_FAILURE;
	}

	ULONG PID = (ULONG)atoi(argv[1]);

	printf("[+] Tasked to kill Process with PID: %lu\n", PID);

	HANDLE hDevice = CreateFileA(
		"\\\\.\\ProcessKiller",
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL
	);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("[!] CreateFileA failed with error code: %d\n", GetLastError());
		return EXIT_FAILURE;
	}

	DWORD dwBytesReturned;

	if (!DeviceIoControl(
		hDevice,
		IOCTL_PROCESS_KILLER,
		&PID,
		sizeof(ULONG),
		NULL,
		0,
		&dwBytesReturned,
		NULL))
	{
		printf("[!] DeviceIoControl failed with error code: %d\n", GetLastError());
		CloseHandle(hDevice);
		return EXIT_FAILURE;
	}

	CloseHandle(hDevice);
	printf("[+] SUCCESS!\n");
	return EXIT_SUCCESS;

}