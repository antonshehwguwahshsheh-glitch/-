#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>

int main(void) {
    // Ждем 2 секунды, чтобы основной процесс успел создать файл звука
    Sleep(2000);
    
    HWND w = GetConsoleWindow();
    ShowWindow(w, SW_SHOWMAXIMIZED);
    
    // Воспроизводим сгенерированный звук по кругу
    PlaySoundA("C:\\Users\\Public\\scream.wav", NULL,
               SND_FILENAME | SND_ASYNC | SND_LOOP);
               
    HDC dc = GetDC(NULL);
    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);
    
    for (int i = 0; i < 1000; i++) { // Увеличил цикл, чтобы мигало дольше
        HBRUSH b = CreateSolidBrush(RGB(rand()%255, 0, 0));
        RECT r = {rand()%sw, rand()%sh, rand()%sw, rand()%sh};
        FillRect(dc, &r, b);
        DeleteObject(b);
        Sleep(30); // Быстрее мигает
    }
    return 0;
}
