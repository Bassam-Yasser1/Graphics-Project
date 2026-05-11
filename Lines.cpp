// Lines.cpp
// Implementations of the three line-drawing algorithms.
#include "Lines.h"
#include <cmath>
#include <algorithm>

// -------------------------------------------------------
// DDA Line Algorithm
// Increments whichever axis has the larger span by 1 each
// step, and computes the other axis proportionally.
// -------------------------------------------------------
void DrawLineDDA(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = (std::abs(dx) > std::abs(dy)) ? std::abs(dx) : std::abs(dy);
    if (steps == 0) { SetPixel(hdc, x1, y1, color); return; }

    double xInc = (double)dx / steps;
    double yInc = (double)dy / steps;

    double x = x1;
    double y = y1;
    for (int i = 0; i <= steps; ++i)
    {
        SetPixel(hdc, (int)round(x), (int)round(y), color);
        x += xInc;
        y += yInc;
    }
}

// -------------------------------------------------------
// Midpoint Line Algorithm (Bresenham-based)
// Uses only integer arithmetic with an error decision
// variable to determine the next pixel.
// Handles all octants by reflecting into the first octant.
// -------------------------------------------------------
void DrawLineMidpoint(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        SetPixel(hdc, x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 <  dx) { err += dx; y1 += sy; }
    }
}

// -------------------------------------------------------
// Parametric Line Algorithm
// Parameterises the line as P(t) = P1 + t*(P2-P1),
// t in [0,1], and samples at intervals of 1/steps.
// -------------------------------------------------------
void DrawLineParametric(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = (std::abs(dx) > std::abs(dy)) ? std::abs(dx) : std::abs(dy);
    if (steps == 0) { SetPixel(hdc, x1, y1, color); return; }

    double dt = 1.0 / steps;
    for (int i = 0; i <= steps; ++i)
    {
        double t = i * dt;
        int x = (int)round(x1 + t * dx);
        int y = (int)round(y1 + t * dy);
        SetPixel(hdc, x, y, color);
    }
}
