#pragma once
#include <windows.h>
#include <vector>

struct FillPoint { int x, y; };

void FillCircleWithLines  (HDC hdc, int cx, int cy, int r, int quarter, COLORREF color);

void FillCircleWithCircles(HDC hdc, int cx, int cy, int r, int quarter, COLORREF color);

void FillSquareHermite    (HDC hdc, int x0, int y0, int side, COLORREF color);

void FillRectBezier       (HDC hdc, int x0, int y0, int w, int h, COLORREF color);

void FillConvexPolygon    (HDC hdc, const std::vector<FillPoint>& pts, COLORREF color);

void FillNonConvexPolygon (HDC hdc, const std::vector<FillPoint>& pts, COLORREF color);

void FloodFillRecursive   (HDC hdc, int x, int y, COLORREF fillColor, COLORREF borderColor);

void FloodFillIterative   (HDC hdc, int x, int y, COLORREF fillColor, COLORREF borderColor);
