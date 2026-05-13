#include "task5_ellipse_algorithms.h"
#include <cmath>

//draws the 4 symmetric points of the ellipse for a given (dx, dy) offset
static inline void Draw4Points(HDC hdc, int cx, int cy,
                                     int dx, int dy, COLORREF color)
{
    SetPixel(hdc, cx + dx, cy + dy, color);
    SetPixel(hdc, cx - dx, cy + dy, color);
    SetPixel(hdc, cx + dx, cy - dy, color);
    SetPixel(hdc, cx - dx, cy - dy, color);
}

// direct algo
// a is the horizontal radius, b is the vertical radius, (cx, cy) is the center of the ellipse
void DrawEllipseDirect(HDC hdc, int cx, int cy, int a, int b, COLORREF color)
{
    // case where the ellipse collapses to a line or point
    if (a == 0 || b == 0) return;

    // Precompute squares to avoid repeated multiplication
    double a2 = static_cast<double>(a) * a;
    double b2 = static_cast<double>(b) * b;

    // iterate over x-axis to ensure no gaps
    for (int dx = -a; dx <= a; ++dx)
    {
        // Compute corresponding y using the ellipse equation: (x^2/a^2) + (y^2/b^2) = 1
        double inner = 1.0 - (static_cast<double>(dx) * dx) / a2;
        // Due to rounding errors, inner might become slightly negative, make it zero
        if (inner < 0.0) inner = 0.0;
        // Compute corresponding y using the ellipse equation: (x^2/a^2) + (y^2/b^2) = 1
        int dy = static_cast<int>(std::round(b * std::sqrt(inner)));
        Draw4Points(hdc, cx, cy, dx, dy, color);
    }

    // iterate over y-axis to ensure no gaps 
    for (int dy = -b; dy <= b; ++dy)
    {
        //same steps for y-axis
        double inner = 1.0 - (static_cast<double>(dy) * dy) / b2;
        if (inner < 0.0) inner = 0.0;
        int dx = static_cast<int>(std::round(a * std::sqrt(inner)));
        Draw4Points(hdc, cx, cy, dx, dy, color);
    }
}

// polar algo
void DrawEllipsePolar(HDC hdc, int cx, int cy, int a, int b, COLORREF color)
{
    if (a == 0 || b == 0) return;     // case where the ellipse collapses to a line or point

    // Choose step size so that the arc length per step approximates 1 pixel
    int r = (a > b) ? a : b;
    double step = 1.0 / r;

    // Iterate over angle from 0 to 2π, compute (x,y) using parametric form:
    // x = cx + a*cos(theta), y = cy + b*sin(theta)
    // const double TWO_PI = 2.0 * 3.14159265358979323846;
    // for (double theta = 0.0; theta < TWO_PI; theta += step)
    // {
    //     int dx = static_cast<int>(std::round(a * std::cos(theta)));
    //     int dy = static_cast<int>(std::round(b * std::sin(theta)));
    //     SetPixel(hdc, cx + dx, cy + dy, color);
    // }

    // Iterate over angle from 0 to π/2 and use symmetry to draw all 4 points, which is more efficient and avoids gaps at the poles
    const double HALF_PI = 3.14159265358979323846 / 2.0;

    for (double theta = 0.0; theta <= HALF_PI; theta += step)
    {
        // Parametric ellipse equations
        int dx = static_cast<int>(std::round(a * std::cos(theta)));
        int dy = static_cast<int>(std::round(b * std::sin(theta)));

        // Draw the 4 symmetric points
        Draw4Points(hdc, cx, cy, dx, dy, color);
    }
    
}

// midpoint algo
void DrawEllipseMidpoint(HDC hdc, int cx, int cy, int a, int b, COLORREF color)
{
    if (a == 0 || b == 0) return; // case where the ellipse collapses to a line or point

    // Precompute squares to avoid repeated multiplication
    long long a2 = static_cast<long long>(a) * a;
    long long b2 = static_cast<long long>(b) * b;

    int x = 0;
    int y = b;

    // case1 |slope| <= 1  
    // Initial decision parameter p1 = b^2 - a^2*b + a^2/4
    long long p1 = b2 - a2 * b + (a2 + 2) / 4;  

    //while we are in the first region, we step through x and decide whether to move vertically based on p1
    while (2LL * b2 * x < 2LL * a2 * y)
    {
        Draw4Points(hdc, cx, cy, x, y, color);

        if (p1 < 0)
        {
            // Move vertically only
            ++x;
            p1 += 2LL * b2 * x + b2;
        }
        else
        {
            // Move diagonally (x+1, y-1)
            ++x;
            --y;
            p1 += 2LL * b2 * x - 2LL * a2 * y + b2;
        }
    }

    // case2 |slope| > 1
    // Initial decision parameter p2 = b^2*(x+1/2)^2 + a^2*(y-1)^2 - a^2*b^2
    long long p2 = b2 * (2LL * x + 1) * (2LL * x + 1) / 4
                 + a2 * (y - 1LL) * (y - 1LL)
                 - a2 * b2;

    // step through y and decide whether to move horizontally based on p2
    while (y >= 0)
    {
        Draw4Points(hdc, cx, cy, x, y, color);

        if (p2 > 0)
        {
            // Move vertically only
            --y;
            p2 -= 2LL * a2 * y + a2;
        }
        else
        {
            // Move diagonally (x+1, y-1)
            ++x;
            --y;
            p2 += 2LL * b2 * x - 2LL * a2 * y + a2;
        }
    }
}
