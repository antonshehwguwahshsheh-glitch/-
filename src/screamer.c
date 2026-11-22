#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>

int main(void) {
    HWND w = GetConsoleWindow();
    ShowWindow(w, SW_SHOWMAXIMIZED);
    PlaySoundA("C:\\Users\\Public\\scream.wav", NULL,
               SND_FILENAME | SND_ASYNC | SND_LOOP);
    HDC dc = GetDC(NULL);
    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);
    for (int i = 0; i < 200; i++) {
        HBRUSH b = CreateSolidBrush(RGB(rand()%255, 0, 0));
        RECT r = {rand()%sw, rand()%sh, rand()%sw, rand()%sh};
        FillRect(dc, &r, b);
        DeleteObject(b);
        Sleep(50);
    }
    return 0;
}
