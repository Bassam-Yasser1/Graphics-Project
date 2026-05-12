#pragma once
#include <windows.h>

void PrefSetWhiteBackground(HWND hwnd);

void PrefSetCursor(HWND hwnd, LPCWSTR cursorId);

COLORREF PrefChooseColor(HWND hwnd, COLORREF currentColor);
