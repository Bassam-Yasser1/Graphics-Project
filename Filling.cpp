#define NOMINMAX
#include "Filling.h"
#include <cmath>
#include <algorithm>
#include <stack>
#include <vector>

// checks if the point (dx, dy) relative to the center is in the specified quarter
static inline bool InQuarter(int dx, int dy, int quarter)
{
    switch (quarter)
    {
    case 3: return dx >= 0 && dy <= 0;
    case 4: return dx <= 0 && dy <= 0;
    case 1: return dx <= 0 && dy >= 0;
    case 2: return dx >= 0 && dy >= 0;
    default: return true; 
    }
}

//scanline fill algo (both convex and non-convex)
void FillCircleWithLines(HDC hdc, int cx, int cy, int r, int quarter, COLORREF color)
{
    // find intersection points with the circle
    for (int dy = -r; dy <= r; ++dy)
    {
        int dx_max = (int)sqrt((double)(r * r - dy * dy));

        // For each dy, find the max dx that satisfies the circle equation
        for (int dx = -dx_max; dx <= dx_max; ++dx)
        { 
            // Check if the point (cx+dx, cy+dy) is in the specified quarter and fill it
            if (InQuarter(dx, dy, quarter))
                SetPixel(hdc, cx + dx, cy + dy, color);
        }
    }
}

// draws concentric circles instead of lines
void FillCircleWithCircles(HDC hdc, int cx, int cy, int r, int quarter, COLORREF color)
{
    for (int ri = 1; ri <= r; ++ri)
    {
        // Use midpoint circle algorithm, draw only pixels in chosen quarter
        int x = 0, y = ri;
        int d = 1 - ri;

        auto PlotOctants = [&](int px, int py)
        {
            // 8-point symmetry map to quarter filter
            struct { int dx; int dy; } pts[8] = {
                { px,  py}, {-px,  py}, { px, -py}, {-px, -py},
                { py,  px}, {-py,  px}, { py, -px}, {-py, -px}
            };
            // Put points in the correct quarter
            for (auto& p : pts)
                if (InQuarter(p.dx, p.dy, quarter))
                    SetPixel(hdc, cx + p.dx, cy + p.dy, color);
        };

        // Initial points on the circle
        PlotOctants(x, y);
        // Midpoint circle algorithm loop
        while (x < y)
        {
            if (d < 0) d += 2 * x + 3;
            else       { d += 2 * (x - y) + 5; --y; }
            ++x;
            PlotOctants(x, y);
        }
    }
}

// Hermite curve point calculation
static void HermitePoint(double t,
    double p0x, double p0y,
    double p1x, double p1y,
    double t0x, double t0y,
    double t1x, double t1y,
    double& ox, double& oy)
{
    double t2 = t * t, t3 = t2 * t;
    double h00 =  2*t3 - 3*t2 + 1;
    double h10 =    t3 - 2*t2 + t;
    double h01 = -2*t3 + 3*t2;
    double h11 =    t3 -   t2;

    ox = h00*p0x + h10*t0x + h01*p1x + h11*t1x;
    oy = h00*p0y + h10*t0y + h01*p1y + h11*t1y;
}

// fills a square with vertical Hermite curves 
void FillSquareHermite(HDC hdc, int x0, int y0, int side, COLORREF color)
{
    // more steps for smoother curves
    int steps = (side + 1)*2; 

    // For each column, define a Hermite curve from top to bottom and fill it
    for (int col = 0; col <= side; col++)
    {
        double p0x = (double)(x0 + col), p0y = (double)y0;
        double p1x = (double)(x0 + col), p1y = (double)(y0 + side);
        double t0x = 0, t0y = (double)side;
        double t1x = 0, t1y = (double)side;

        // points along the Hermite curve filling them
        for (int s = 0; s <= steps; ++s)
        {
            // t parameter from 0 to 1
            double t = (double)s / steps;
            double ox, oy;
            HermitePoint(t, p0x, p0y, p1x, p1y, t0x, t0y, t1x, t1y, ox, oy);
            SetPixel(hdc, (int)round(ox), (int)round(oy), color);
        }
    }
}

// Bezier curve point calculation
static void BezierPoint(double t,
    double p0x, double p0y,
    double p1x, double p1y,
    double p2x, double p2y,
    double p3x, double p3y,
    double& ox, double& oy)
{
    double u  = 1.0 - t;
    double u2 = u * u, u3 = u2 * u;
    double t2 = t * t, t3 = t2 * t;

    ox = u3*p0x + 3*u2*t*p1x + 3*u*t2*p2x + t3*p3x;
    oy = u3*p0y + 3*u2*t*p1y + 3*u*t2*p2y + t3*p3y;
}

void FillRectBezier(HDC hdc, int x0, int y0, int w, int h, COLORREF color)
{
    
    int steps = (w + 1)*2; // more steps for smoother curves

    // For each row, define a Bezier curve from left to right and fill it
    for (int row = 0; row <= h; row++)
    {
        double ry  = (double)(y0 + row);
        double p0x = (double)x0,       p0y = ry;
        double p1x = (double)(x0+w/3), p1y = ry;
        double p2x = (double)(x0+2*w/3), p2y = ry;
        double p3x = (double)(x0+w),   p3y = ry;

        // points along the Bezier curve filling them
        for (int s = 0; s <= steps; ++s)
        {
            // t parameter from 0 to 1
            double t = (double)s / steps;
            double ox, oy;
            BezierPoint(t, p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y, ox, oy);
            SetPixel(hdc, (int)round(ox), (int)round(oy), color);
        }
    }
}

// scanline fill function used by both convex and non-convex polygon fills
static void ScanlineFill(HDC hdc, const std::vector<FillPoint>& pts, COLORREF color)
{
    // Get the number of vertices and return if less than 3
    int n = (int)pts.size();
    if (n < 3) return;

    // Find the min and max y to determine the range of scanlines
    int ymin = pts[0].y, ymax = pts[0].y;
    for (auto& p : pts) { ymin = std::min(ymin, p.y); ymax = std::max(ymax, p.y); }

    // For each scanline, find intersections with polygon edges and fill between pairs of intersections
    for (int y = ymin; y <= ymax; ++y)
    {
        std::vector<int> xIntersect;

        // Check each edge of the polygon for intersection with the scanline
        for (int i = 0; i < n; ++i)
        {
            // Edge from pts[i] to pts[j]
            int j = (i + 1) % n;
            int yi = pts[i].y, yj = pts[j].y;
            int xi = pts[i].x, xj = pts[j].x;

            // Check if the edge intersects with the scanline y
            if ((yi <= y && yj > y) || (yj <= y && yi > y))
            {
                // Calculate the x-coordinate of the intersection
                int x = xi + (y - yi) * (xj - xi) / (yj - yi);
                xIntersect.push_back(x);
            }
        }

        std::sort(xIntersect.begin(), xIntersect.end());
// Fill between pairs of intersections
        for (int k = 0; k + 1 < (int)xIntersect.size(); k += 2)
            for (int x = xIntersect[k]; x <= xIntersect[k+1]; ++x)
                SetPixel(hdc, x, y, color);
    }
}

// Convex polygon fill just calls the scanline fill
void FillConvexPolygon(HDC hdc, const std::vector<FillPoint>& pts, COLORREF color)
{
    ScanlineFill(hdc, pts, color);
}

// Non-convex polygon fill just calls the scanline fill 
void FillNonConvexPolygon(HDC hdc, const std::vector<FillPoint>& pts, COLORREF color)
{
    ScanlineFill(hdc, pts, color);
}

// Recursive flood fill algo
void FloodFillRecursive(HDC hdc, int x, int y, COLORREF fillColor, COLORREF targetColor)
{
    // Check the current pixel color
    COLORREF cur = GetPixel(hdc, x, y);
    if (cur != targetColor) return;   // stop at anything that isn't the interior
    if (cur == fillColor)   return;   // already filled

    // Fill the pixel and recursively fill neighbors
    SetPixel(hdc, x, y, fillColor);
    FloodFillRecursive(hdc, x+1, y,   fillColor, targetColor);
    FloodFillRecursive(hdc, x-1, y,   fillColor, targetColor);
    FloodFillRecursive(hdc, x,   y+1, fillColor, targetColor);
    FloodFillRecursive(hdc, x,   y-1, fillColor, targetColor);
}

// Iterative flood fill using a stack
void FloodFillIterative(HDC hdc, int x, int y, COLORREF fillColor, COLORREF targetColor)
{
    if (GetPixel(hdc, x, y) == fillColor) return; // already filled
    // Stack for points to fill
    struct Pt { int x, y; };
    std::stack<Pt> stk;
    stk.push({x, y});

    // fill points until stack is empty
    while (!stk.empty())
    {
        Pt p = stk.top(); stk.pop();
        COLORREF cur = GetPixel(hdc, p.x, p.y);
        if (cur != targetColor) continue;  // border or already filled

        // Fill the pixel and add neighbors to stack
        SetPixel(hdc, p.x, p.y, fillColor);
        stk.push({p.x+1, p.y});
        stk.push({p.x-1, p.y});
        stk.push({p.x,   p.y+1});
        stk.push({p.x,   p.y-1});
    }
}
