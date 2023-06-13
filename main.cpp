#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

DWORD getProcessID(const std::wstring& processName)
{
	DWORD processID = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	PROCESSENTRY32 processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hSnapshot, &processEntry))
		return 0;

	do
	{
		if (processName == processEntry.szExeFile)
		{
			processID = processEntry.th32ProcessID;
			break;
		}
	} while (Process32Next(hSnapshot, &processEntry));

	CloseHandle(hSnapshot);
	return processID;
}

void LeakMemoryLOL(int healthChange)
{
	for (int i = 1; i <= healthChange; ++i)
	{
		for (int j = 1; j <= 100; ++j)
			malloc((size_t)1000000); // One megabyte
	}
	std::cout << "Leaked " << 100 * healthChange << " megabytes, LOL!\n";
}

int main()
{
	// Modify this variable with the address to your HP
	LPVOID memory = (LPVOID)0x0;

	HANDLE minecraftHandle = OpenProcess(PROCESS_VM_READ, 0, getProcessID(L"javaw.exe"));
	if (!minecraftHandle)
	{
		std::cerr << GetLastError() << '\n';
		return 0;
	}

	int oldHP = 0, newHP = 0;
	while (true)
	{
		if (!ReadProcessMemory(minecraftHandle, memory, &newHP, sizeof(int), 0))
		{
			std::cerr << GetLastError();
			return 0;
		}

		int healthChange = oldHP - newHP;
		if (healthChange > 0)
			LeakMemoryLOL(healthChange);

		oldHP = newHP;

		Sleep(1000);
	}

	return 0;
}
