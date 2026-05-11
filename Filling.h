#pragma once
#include <windows.h>
#include <vector>

// -------------------------------------------------------
// Filling.h
// Shape-filling algorithms:
//   a) Fill Circle with horizontal lines  (by quarter)
//   b) Fill Circle with concentric circles (by quarter)
//   c) Fill Square  with Hermite curves   (vertical)
//   d) Fill Rectangle with Bezier curves  (horizontal)
//   e) Convex & Non-Convex polygon fill   (scanline)
//   f) Recursive & Non-Recursive Flood Fill
// -------------------------------------------------------

struct FillPoint { int x, y; };

// a) Fill a quarter of a circle using horizontal lines
//    quarter: 1=top-right, 2=top-left, 3=bottom-left, 4=bottom-right
void FillCircleWithLines  (HDC hdc, int cx, int cy, int r, int quarter, COLORREF color);

// b) Fill a quarter of a circle using concentric circles
void FillCircleWithCircles(HDC hdc, int cx, int cy, int r, int quarter, COLORREF color);

// c) Fill a square with vertical Hermite curves
void FillSquareHermite    (HDC hdc, int x0, int y0, int side, COLORREF color);

// d) Fill a rectangle with horizontal Bezier curves
void FillRectBezier       (HDC hdc, int x0, int y0, int w, int h, COLORREF color);

// e) Convex polygon fill (scanline)
void FillConvexPolygon    (HDC hdc, const std::vector<FillPoint>& pts, COLORREF color);

// e) Non-Convex polygon fill (scanline, handles re-entrant polygons)
void FillNonConvexPolygon (HDC hdc, const std::vector<FillPoint>& pts, COLORREF color);

// f) Recursive flood fill
void FloodFillRecursive   (HDC hdc, int x, int y, COLORREF fillColor, COLORREF borderColor);

// f) Non-recursive (stack-based) flood fill
void FloodFillIterative   (HDC hdc, int x, int y, COLORREF fillColor, COLORREF borderColor);
