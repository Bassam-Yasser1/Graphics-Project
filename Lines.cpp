#include "Lines.h"
#include <cmath>
#include <algorithm>

// DDA Line Drawing Algo
void DrawLineDDA(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    // Calculate delta x and y
    int dx = x2 - x1;
    int dy = y2 - y1;
    // calculate the number of steps needed
    int steps = (std::abs(dx) > std::abs(dy)) ? std::abs(dx) : std::abs(dy);
    // if steps is 0 line is a single point
    if (steps == 0) { SetPixel(hdc, x1, y1, color); return; }

    // Calculate the increment in x and y for each step
    double xInc = (double)dx / steps;
    double yInc = (double)dy / steps;

    double x = x1;
    double y = y1;
    // Draw the line by incrementing x and y in each step
    for (int i = 0; i <= steps; ++i)
    {
        SetPixel(hdc, (int)round(x), (int)round(y), color);
        x += xInc;
        y += yInc;
    }
}

// Midpoint Line Drawing Algo
void DrawLineMidpoint(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    // Calculate deltas and steps
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        // Set the pixel for the current point
        SetPixel(hdc, x1, y1, color);
        // If we've reached the end point, break
        if (x1 == x2 && y1 == y2) break;
        // Calculate error and increment x and y accordingly
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 <  dx) { err += dx; y1 += sy; }
    }
}


// Parametric Line Drawing Algo
void DrawLineParametric(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    // Calculate deltas and steps
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = (std::abs(dx) > std::abs(dy)) ? std::abs(dx) : std::abs(dy);
    if (steps == 0) { SetPixel(hdc, x1, y1, color); return; } // single point case

    // Draw the line by varying t from 0 to 1
    double dt = 1.0 / steps;
    // calculate the corresponding point on the line and set the pixel
    for (int i = 0; i <= steps; ++i)
    {
        double t = i * dt;
        // Calculate the x and y coordinates using the parametric form of the line
        int x = (int)round(x1 + t * dx);
        int y = (int)round(y1 + t * dy);
        SetPixel(hdc, x, y, color);
    }
}
