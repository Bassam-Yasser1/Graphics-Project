#pragma once
#include <windows.h>

// -------------------------------------------------------
// Preferences.h
// User-preference helpers:
//   a) Set window background to white
//   b) Change the window cursor shape
//   c) Let the user pick a drawing colour via ColorDlg
// -------------------------------------------------------

// a) Repaints the client area with a solid white background.
//    Call after WM_COMMAND for IDM_PREF_BG_WHITE.
void PrefSetWhiteBackground(HWND hwnd);

// b) Changes the class cursor to one of the built-in IDC_ shapes.
//    cursorId: one of IDC_ARROW, IDC_CROSS, IDC_IBEAM, IDC_HAND, etc.
void PrefSetCursor(HWND hwnd, LPCWSTR cursorId);

// c) Opens the system Color-picker dialog.
//    Returns the chosen COLORREF, or the current colour if cancelled.
COLORREF PrefChooseColor(HWND hwnd, COLORREF currentColor);
