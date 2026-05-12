#define UNICODE
#define _UNICODE
#include "Preferences.h"
#include <commdlg.h>   

// Change background to white
void PrefSetWhiteBackground(HWND hwnd)
{
    // Force the class brush to WHITE_BRUSH so every subsequent WM_ERASEBKGND uses white
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND,
                    (LONG_PTR)GetStockObject(WHITE_BRUSH));

    //  fill the current client rect with white
    HDC  hdc  = GetDC(hwnd);
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
    ReleaseDC(hwnd, hdc);

    InvalidateRect(hwnd, NULL, TRUE);
}

// Change cursor shape
void PrefSetCursor(HWND hwnd, LPCWSTR cursorId)
{
    // Load the specified cursor and set it as the class cursor
    HCURSOR hCur = LoadCursor(NULL, cursorId);
    if (!hCur) return;
    // Update the class cursor so it applies to the entire window
    SetClassLongPtr(hwnd, GCLP_HCURSOR, (LONG_PTR)hCur);

    // Force Windows to re-read the class cursor 
    SetCursor(hCur);
}


// open colour picker dialog and return chosen colour
static COLORREF g_customColors[16] = {
    RGB(255,255,255), RGB(0,0,0),       RGB(255,0,0),   RGB(0,255,0),
    RGB(0,0,255),     RGB(255,255,0),   RGB(0,255,255), RGB(255,0,255),
    RGB(128,0,0),     RGB(0,128,0),     RGB(0,0,128),   RGB(128,128,0),
    RGB(0,128,128),   RGB(128,0,128),   RGB(128,128,128),RGB(192,192,192)
};

// Open the standard Windows colour picker dialog and return the chosen colour.
COLORREF PrefChooseColor(HWND hwnd, COLORREF currentColor)
{
    
    CHOOSECOLOR cc   = {};
    cc.lStructSize   = sizeof(cc);
    // Set initial colour selection to the current drawing colour
    cc.hwndOwner     = hwnd;
    // Provide custom colours array for the dialog
    cc.rgbResult     = currentColor;
    cc.lpCustColors  = g_customColors;
    cc.Flags         = CC_FULLOPEN | CC_RGBINIT;

    // Display the dialog and return the chosen colour
    if (ChooseColor(&cc))
        return cc.rgbResult;

    return currentColor;   // keep old colour
}
