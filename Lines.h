#pragma once
#include <windows.h>

// -------------------------------------------------------
// Lines.h
// Line drawing algorithms:
//   - DDA (Digital Differential Analyzer)
//   - Midpoint Line
//   - Parametric Line
// Each takes two endpoints and a colour.
// -------------------------------------------------------

void DrawLineDDA        (HDC hdc, int x1, int y1, int x2, int y2, COLORREF color);
void DrawLineMidpoint   (HDC hdc, int x1, int y1, int x2, int y2, COLORREF color);
void DrawLineParametric (HDC hdc, int x1, int y1, int x2, int y2, COLORREF color);
