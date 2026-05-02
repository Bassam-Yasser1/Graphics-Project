#pragma once
#include <windows.h>

// -------------------------------------------------------
// Task 5 – Ellipse Drawing Algorithms
//   Three algorithms to draw an ellipse with centre (cx, cy)
//   and semi-axes a (horizontal) and b (vertical):
//     a) Direct / Cartesian equation method
//     b) Polar / parametric method
//     c) Midpoint (Bresenham-style) method
//
// Each function plots individual pixels – no GDI Ellipse() used.
// -------------------------------------------------------

// ---- a) Direct equation ---------------------------------
// Iterates x from -a to +a, computes y from the implicit
// equation  (x/a)^2 + (y/b)^2 = 1  and plots four symmetric points.
void DrawEllipseDirect(HDC hdc, int cx, int cy, int a, int b, COLORREF color);

// ---- b) Polar / parametric ------------------------------
// Steps the angle θ from 0 to 2π in small increments,
// evaluates  x = cx + a*cos(θ),  y = cy + b*sin(θ)  and sets each pixel.
void DrawEllipsePolar(HDC hdc, int cx, int cy, int a, int b, COLORREF color);

// ---- c) Midpoint ----------------------------------------
// Implements the integer midpoint algorithm for ellipses using
// two regions (where slope |dy/dx| < 1 and |dy/dx| > 1) with
// decision parameters to avoid floating-point operations.
void DrawEllipseMidpoint(HDC hdc, int cx, int cy, int a, int b, COLORREF color);

// Convenience wrapper: draws all three algorithms on the same DC
// with distinct colours so they can be compared side-by-side.
// Offset separates the three ellipses horizontally by `offset` px.
void DrawAllEllipses(HDC hdc, int cx, int cy, int a, int b, int offset);
