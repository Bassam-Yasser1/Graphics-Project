#pragma once
#include <windows.h>

// Draws 8 symmetric points for every (x, y) coordinate to optimize circle rendering.
void Draw8Points(HDC hdc, int xc, int yc, int x, int y, COLORREF color);

// 1. Direct Circle Algorithm: Uses the Cartesian equation (x^2 + y^2 = R^2)
void CircleDirect(HDC hdc, int xc, int yc, int R, COLORREF color);

// 2. Polar Circle Algorithm: Uses trigonometric functions (R*cos(theta), R*sin(theta))
void CirclePolar(HDC hdc, int xc, int yc, int R, COLORREF color);

// 3. Iterative Polar Algorithm: Optimizes Polar by using incremental angle calculations
void CircleIterativePolar(HDC hdc, int xc, int yc, int R, COLORREF color);

// 4. Midpoint Circle Algorithm: Uses a decision variable to choose the next pixel
void CircleMidpoint(HDC hdc, int xc, int yc, int R, COLORREF color);

// 5. Modified Midpoint: An optimized version of Midpoint using second-order differences
void CircleModifiedMidpoint(HDC hdc, int xc, int yc, int R, COLORREF color);
