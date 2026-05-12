#pragma once
#include <windows.h>

// Draw 8 symmetric points of the circle using circle symmetry around the center (xc, yc)
void Draw8Points(HDC hdc, int xc, int yc, int x, int y, COLORREF color);

// Direct circle drawing algo using the circle equation: x² + y² = R²
void CircleDirect(HDC hdc, int xc, int yc, int R, COLORREF color);

// Polar circle drawing algo using the polar equation of a circle: x = R * cos(theta), y = R * sin(theta)
void CirclePolar(HDC hdc, int xc, int yc, int R, COLORREF color);

// Iterative polar circle algo avoiding repeated sin/cos calculations
void CircleIterativePolar(HDC hdc, int xc, int yc, int R, COLORREF color);

// Midpoint circle algo using decision parameter to choose next pixel
void CircleMidpoint(HDC hdc, int xc, int yc, int R, COLORREF color);

// Modified midpoint circle algo using incremental updates
void CircleModifiedMidpoint(HDC hdc, int xc, int yc, int R, COLORREF color);
