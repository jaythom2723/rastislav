#include "video.h"

#include "tty.h"

#include <stdio.h>

extern char tty_buffer[MAX_TTY_CHARS];
extern char *tty_buf_ptr;

static HWND hwnd;
static rastislav_t *console;

void displayCharacter(rastislav_t *rastislav, HDC hdc);
void displayPixel(rastislav_t *rastislav, HDC hdc);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

void videoInit(rastislav_t *rastislav)
{
    const char *CLASS_NAME = "Sample Window Class";
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = NULL;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);
    hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Rastislav Console Emulator",
        WS_OVERLAPPEDWINDOW,

        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,

        NULL,
        NULL,
        NULL,
        NULL
    );

    if(hwnd == NULL)
        exit(-1);

    ShowWindow(hwnd, 1);

    console = rastislav;
}

void videoProcess(rastislav_t *rastislav)
{
    MSG msg = { 0 };

    if(hwnd == NULL)
        exit(-1);

    if(GetMessage(&msg, hwnd, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void videoUpdateConsoleReference(rastislav_t *rastislav)
{
    console = rastislav;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    int width, height;

    switch(umsg)
    {
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);

            if(console->regs[RFX] == (VID_OUT | TTY_OUT))
            {
                displayCharacter(console, hdc);
                console->regs[RFX] = 0;
            }

            if(console->regs[RFX] == VID_OUT)
                displayPixel(console, hdc);

            EndPaint(hwnd, &ps);
        return 0;
        case WM_CLOSE:
            if(MessageBox(hwnd, "Really Quit?", "Rastislav Emulator", MB_OKCANCEL) == IDOK)
                DestroyWindow(hwnd);
        return 0;
        case WM_DESTROY:
        return 0;
        default:
        return DefWindowProc(hwnd, umsg, wparam, lparam);
    }
}

void displayCharacter(rastislav_t *rastislav, HDC hdc)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    rect.left = 40;
    rect.top = 10;

    SetTextColor(hdc, 0x00000000);
    DrawText(hdc, tty_buffer, -1, &rect, DT_SINGLELINE | DT_NOCLIP);
}

void displayPixel(rastislav_t *rastislav, HDC hdc)
{
    int x, y;
    x = rastislav->ports[VIDEO_PORT_X];
    y = rastislav->ports[VIDEO_PORT_Y];

    RECT rect;
    GetClientRect(hwnd, &rect);
    rect.left = x;
    rect.top = y;
    rect.right = x + 2;
    rect.bottom = y + 2;

    FillRect(hdc, &rect, (HBRUSH) (COLOR_WINDOW));

    SetPixel(hdc, rastislav->ports[VIDEO_PORT_X], rastislav->ports[VIDEO_PORT_Y], 0x000000FF);
}
