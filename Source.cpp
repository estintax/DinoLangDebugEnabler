#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
	if (argc > 2) {
		vector<DWORD> pids;
		wstring targetProcessName = L"dinolang.exe";
		// Converting strings to integers
		DWORD offset;
		offset = (DWORD)strtol(argv[1], NULL, 16);
		if (offset == 0) {
			cout << "Offset must be an integer!" << endl;
			return 1;
		}
		int state = atoi(argv[2]);

		// Working on it
		HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
		PROCESSENTRY32W entry;
		entry.dwSize = sizeof entry;
		if (!Process32FirstW(snap, &entry)) {
			return 0;
		}

		do {
			if (wstring(entry.szExeFile) == targetProcessName) {
				pids.emplace_back(entry.th32ProcessID);
			}
		} while (Process32NextW(snap, &entry));

		for (int i(0); i < pids.size(); ++i) {
			HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pids[i]);
			if (proc == NULL) {
				cout << "Failed to open Dinolang process: " << GetLastError() << endl;
				return 1;
			}

			BYTE data = 0x0;
			if (state == 1) {
				data = 0x1;
			}
			LPVOID address = (LPVOID)offset;
			BOOL result = WriteProcessMemory(proc, address, &data, sizeof(BYTE), NULL);
			if (result) cout << "Wrote " << state << " to offset 0x" << hex << offset << " on PID: " << dec << pids[i] << endl;
			else {
				cout << "Failed to write to process memory: " << GetLastError() << endl;
			}
			CloseHandle(proc);
		}

	}
	else {
		cout << "Usage: <debug offset> <state (1 - enable, 0 - disable)>" << endl;
		return 1;
	}
	return 0;
}
