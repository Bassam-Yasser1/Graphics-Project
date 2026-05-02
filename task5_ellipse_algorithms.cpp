// task5_ellipse_algorithms.cpp
// Task 5 – Three ellipse-drawing algorithms implemented from scratch
// (no GDI Ellipse() is used; every pixel is placed with SetPixel).
#include "task5_ellipse_algorithms.h"
#include <cmath>

// -------------------------------------------------------
// Internal helper – plot four symmetric points of an ellipse
// around centre (cx, cy) given the offset pair (dx, dy).
// -------------------------------------------------------
static inline void Draw4Points(HDC hdc, int cx, int cy,
                                     int dx, int dy, COLORREF color)
{
    SetPixel(hdc, cx + dx, cy + dy, color);
    SetPixel(hdc, cx - dx, cy + dy, color);
    SetPixel(hdc, cx + dx, cy - dy, color);
    SetPixel(hdc, cx - dx, cy - dy, color);
}

// =======================================================
// a) Direct (Cartesian equation) method
//    Iterates x from -a to a, computes y from the ellipse
//    equation  y = b * sqrt(1 - (x/a)^2)  and plots symmetrically.
// =======================================================
void DrawEllipseDirect(HDC hdc, int cx, int cy, int a, int b, COLORREF color)
{
    if (a == 0 || b == 0) return;

    double a2 = static_cast<double>(a) * a;
    double b2 = static_cast<double>(b) * b;

    // Region 1: iterate over x-axis (-a … +a)
    for (int dx = -a; dx <= a; ++dx)
    {
        double inner = 1.0 - (static_cast<double>(dx) * dx) / a2;
        if (inner < 0.0) inner = 0.0;
        int dy = static_cast<int>(std::round(b * std::sqrt(inner)));
        Draw4Points(hdc, cx, cy, dx, dy, color);
    }

    // Region 2: iterate over y-axis (-b … +b) to fill in the steep parts
    for (int dy = -b; dy <= b; ++dy)
    {
        double inner = 1.0 - (static_cast<double>(dy) * dy) / b2;
        if (inner < 0.0) inner = 0.0;
        int dx = static_cast<int>(std::round(a * std::sqrt(inner)));
        Draw4Points(hdc, cx, cy, dx, dy, color);
    }
}

// =======================================================
// b) Polar / parametric method
//    Steps angle θ from 0 to 2π using a step small enough
//    that no pixels are skipped:  step = 1 / max(a, b).
// =======================================================
void DrawEllipsePolar(HDC hdc, int cx, int cy, int a, int b, COLORREF color)
{
    if (a == 0 || b == 0) return;

    // Choose step size so that the arc length per step ≈ 1 pixel
    int r = (a > b) ? a : b;
    double step = 1.0 / r;

    const double TWO_PI = 2.0 * 3.14159265358979323846;
    for (double theta = 0.0; theta < TWO_PI; theta += step)
    {
        int dx = static_cast<int>(std::round(a * std::cos(theta)));
        int dy = static_cast<int>(std::round(b * std::sin(theta)));
        SetPixel(hdc, cx + dx, cy + dy, color);
    }
}

// =======================================================
// c) Midpoint (Bresenham-style) method
//    Uses integer arithmetic with two decision parameters
//    (one for each region).  Based on the implicit form
//    F(x, y) = b²x² + a²y² – a²b²
// =======================================================
void DrawEllipseMidpoint(HDC hdc, int cx, int cy, int a, int b, COLORREF color)
{
    if (a == 0 || b == 0) return;

    long long a2 = static_cast<long long>(a) * a;
    long long b2 = static_cast<long long>(b) * b;

    int x = 0;
    int y = b;

    // ---- Region 1: |slope| < 1  (from top to the slope = -1 point) ----
    // Initial decision parameter p1 = b² - a²b + a²/4
    // (using integer approximation for a²/4)
    long long p1 = b2 - a2 * b + (a2 + 2) / 4;  // round(a²/4)

    while (2LL * b2 * x < 2LL * a2 * y)
    {
        Draw4Points(hdc, cx, cy, x, y, color);

        if (p1 < 0)
        {
            // Move East
            ++x;
            p1 += 2LL * b2 * x + b2;
        }
        else
        {
            // Move South-East
            ++x;
            --y;
            p1 += 2LL * b2 * x - 2LL * a2 * y + b2;
        }
    }

    // ---- Region 2: |slope| > 1  (from the slope = -1 point to rightmost) ----
    // Initial decision parameter p2 = b²(x+1/2)² + a²(y-1)² - a²b²
    long long p2 = b2 * (2LL * x + 1) * (2LL * x + 1) / 4
                 + a2 * (y - 1LL) * (y - 1LL)
                 - a2 * b2;

    while (y >= 0)
    {
        Draw4Points(hdc, cx, cy, x, y, color);

        if (p2 > 0)
        {
            // Move South
            --y;
            p2 -= 2LL * a2 * y + a2;
        }
        else
        {
            // Move South-East
            ++x;
            --y;
            p2 += 2LL * b2 * x - 2LL * a2 * y + a2;
        }
    }
}

// =======================================================
// Convenience wrapper – draws all three algorithms with
// different colours, offset horizontally so they are visible.
//   Blue   = Direct
//   Green  = Polar
//   Red    = Midpoint
// =======================================================
void DrawAllEllipses(HDC hdc, int cx, int cy, int a, int b, int offset)
{
    // Direct  – centred at (cx - offset, cy)
    DrawEllipseDirect  (hdc, cx - offset,cy, a, b, RGB(0,0,220));

    // Polar   – centred at (cx, cy)
    DrawEllipsePolar   (hdc, cx,cy, a, b, RGB(0,180,0));

    // Midpoint – centred at (cx + offset, cy)
    DrawEllipseMidpoint(hdc, cx + offset,cy, a, b, RGB(220,0,0));
}
