// Preferences.cpp
#define UNICODE
#define _UNICODE
#include "Preferences.h"
#include <commdlg.h>   // ChooseColor

// -------------------------------------------------------
// a) White background
//    Fills the entire client area with white and requests
//    a repaint.  The WM_ERASEBKGND / WM_PAINT handlers
//    will keep it white afterwards because we also update
//    the class background brush (see PrefSetWhiteBackground).
// -------------------------------------------------------
void PrefSetWhiteBackground(HWND hwnd)
{
    // Force the class brush to WHITE_BRUSH so every
    // subsequent WM_ERASEBKGND uses white.
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND,
                    (LONG_PTR)GetStockObject(WHITE_BRUSH));

    // Immediately fill the current client rect with white.
    HDC  hdc  = GetDC(hwnd);
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
    ReleaseDC(hwnd, hdc);

    InvalidateRect(hwnd, NULL, TRUE);
}

// -------------------------------------------------------
// b) Change cursor
//    Loads one of the system stock cursors and sets it as
//    the class cursor so it applies everywhere in the window.
// -------------------------------------------------------
void PrefSetCursor(HWND hwnd, LPCWSTR cursorId)
{
    HCURSOR hCur = LoadCursor(NULL, cursorId);
    if (!hCur) return;

    SetClassLongPtr(hwnd, GCLP_HCURSOR, (LONG_PTR)hCur);

    // Force Windows to re-read the class cursor immediately.
    SetCursor(hCur);
}

// -------------------------------------------------------
// c) Colour picker
//    Opens the standard Windows ChooseColor dialog.
//    Returns the selected colour, or currentColor on cancel.
// -------------------------------------------------------

// Persistent custom-colour array (survives across calls within a session).
static COLORREF g_customColors[16] = {
    RGB(255,255,255), RGB(0,0,0),       RGB(255,0,0),   RGB(0,255,0),
    RGB(0,0,255),     RGB(255,255,0),   RGB(0,255,255), RGB(255,0,255),
    RGB(128,0,0),     RGB(0,128,0),     RGB(0,0,128),   RGB(128,128,0),
    RGB(0,128,128),   RGB(128,0,128),   RGB(128,128,128),RGB(192,192,192)
};

COLORREF PrefChooseColor(HWND hwnd, COLORREF currentColor)
{
    CHOOSECOLOR cc   = {};
    cc.lStructSize   = sizeof(cc);
    cc.hwndOwner     = hwnd;
    cc.rgbResult     = currentColor;
    cc.lpCustColors  = g_customColors;
    cc.Flags         = CC_FULLOPEN | CC_RGBINIT;

    if (ChooseColor(&cc))
        return cc.rgbResult;

    return currentColor;   // user cancelled – keep old colour
}