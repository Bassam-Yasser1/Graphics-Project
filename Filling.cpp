// Filling.cpp
// Implementations of all shape-filling algorithms.
#define NOMINMAX
#include "Filling.h"
#include <cmath>
#include <algorithm>
#include <stack>
#include <vector>

// =====================================================================
// HELPER: draw one pixel in the correct quarter of a circle.
// Quarter mapping (Cartesian convention, y grows downward in GDI):
//   1 = top-right  ( dx >= 0, dy <= 0 in Cartesian -> dy <= 0 on screen )
//   2 = top-left   ( dx <= 0, dy <= 0 )
//   3 = bottom-left( dx <= 0, dy >= 0 )
//   4 = bottom-right(dx >= 0, dy >= 0 )
// =====================================================================
static inline bool InQuarter(int dx, int dy, int quarter)
{
    switch (quarter)
    {
    case 1: return dx >= 0 && dy <= 0;
    case 2: return dx <= 0 && dy <= 0;
    case 3: return dx <= 0 && dy >= 0;
    case 4: return dx >= 0 && dy >= 0;
    default: return true; // all quarters
    }
}

// =====================================================================
// a) Fill a quarter of a circle with horizontal scan-lines.
//    For each y in [cy-r, cy+r], find the x span on the circle
//    boundary and draw a horizontal line, masked to the chosen quarter.
// =====================================================================
void FillCircleWithLines(HDC hdc, int cx, int cy, int r, int quarter, COLORREF color)
{
    for (int dy = -r; dy <= r; ++dy)
    {
        int dx_max = (int)sqrt((double)(r * r - dy * dy));
        for (int dx = -dx_max; dx <= dx_max; ++dx)
        {
            if (InQuarter(dx, dy, quarter))
                SetPixel(hdc, cx + dx, cy + dy, color);
        }
    }
}

// =====================================================================
// b) Fill a quarter of a circle with concentric circles.
//    Draws circles of radius 0..r step 1, masking to the chosen quarter.
// =====================================================================
void FillCircleWithCircles(HDC hdc, int cx, int cy, int r, int quarter, COLORREF color)
{
    for (int ri = 1; ri <= r; ++ri)
    {
        // Use midpoint circle algorithm, draw only pixels in chosen quarter
        int x = 0, y = ri;
        int d = 1 - ri;

        auto PlotOctants = [&](int px, int py)
        {
            // 8-way symmetry -> map to quarter filter
            struct { int dx; int dy; } pts[8] = {
                { px,  py}, {-px,  py}, { px, -py}, {-px, -py},
                { py,  px}, {-py,  px}, { py, -px}, {-py, -px}
            };
            for (auto& p : pts)
                if (InQuarter(p.dx, p.dy, quarter))
                    SetPixel(hdc, cx + p.dx, cy + p.dy, color);
        };

        PlotOctants(x, y);
        while (x < y)
        {
            if (d < 0) d += 2 * x + 3;
            else       { d += 2 * (x - y) + 5; --y; }
            ++x;
            PlotOctants(x, y);
        }
    }
}

// =====================================================================
// HELPER: evaluate a cubic Hermite curve at parameter t.
//   P(t) = h00*P0 + h10*T0 + h01*P1 + h11*T1
//   where T0, T1 are tangent vectors.
// =====================================================================
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

// =====================================================================
// c) Fill a square with vertical Hermite curves.
//    For each integer x column inside the square, draw a Hermite curve
//    from the top edge to the bottom edge with tangents that create a
//    gentle S-wave for visual interest.
// =====================================================================
void FillSquareHermite(HDC hdc, int x0, int y0, int side, COLORREF color)
{
    // Each column is a vertical Hermite curve top->bottom.
    // Tangents point straight down so curves stay inside the square.
    int steps = (side + 1)*2; // more steps for smoother curves

    for (int col = 0; col <= side; col++)
    {
        double p0x = (double)(x0 + col), p0y = (double)y0;
        double p1x = (double)(x0 + col), p1y = (double)(y0 + side);
        double t0x = 0, t0y = (double)side;
        double t1x = 0, t1y = (double)side;

        for (int s = 0; s <= steps; ++s)
        {
            double t = (double)s / steps;
            double ox, oy;
            HermitePoint(t, p0x, p0y, p1x, p1y, t0x, t0y, t1x, t1y, ox, oy);
            SetPixel(hdc, (int)round(ox), (int)round(oy), color);
        }
    }
}

// =====================================================================
// HELPER: evaluate a cubic Bezier curve at parameter t.
//   B(t) = (1-t)^3 P0 + 3(1-t)^2 t P1 + 3(1-t) t^2 P2 + t^3 P3
// =====================================================================
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

// =====================================================================
// d) Fill a rectangle with horizontal Bezier curves.
//    For each integer y row, draw a Bezier from the left to right edge
//    with control points that bow upward/downward for visual texture.
// =====================================================================
void FillRectBezier(HDC hdc, int x0, int y0, int w, int h, COLORREF color)
{
    // Each row is a horizontal Bezier curve left->right.
    // Control points on the same y keeps curves inside the rectangle.
    int steps = (w + 1)*2; // more steps for smoother curves

    for (int row = 0; row <= h; row++)
    {
        double ry  = (double)(y0 + row);
        double p0x = (double)x0,       p0y = ry;
        double p1x = (double)(x0+w/3), p1y = ry;
        double p2x = (double)(x0+2*w/3), p2y = ry;
        double p3x = (double)(x0+w),   p3y = ry;

        for (int s = 0; s <= steps; ++s)
        {
            double t = (double)s / steps;
            double ox, oy;
            BezierPoint(t, p0x, p0y, p1x, p1y, p2x, p2y, p3x, p3y, ox, oy);
            SetPixel(hdc, (int)round(ox), (int)round(oy), color);
        }
    }
}

// =====================================================================
// HELPER: generic scanline fill for any simple polygon.
//   Works for both convex and non-convex polygons via edge-table.
// =====================================================================
static void ScanlineFill(HDC hdc, const std::vector<FillPoint>& pts, COLORREF color)
{
    int n = (int)pts.size();
    if (n < 3) return;

    // Find y-extents
    int ymin = pts[0].y, ymax = pts[0].y;
    for (auto& p : pts) { ymin = std::min(ymin, p.y); ymax = std::max(ymax, p.y); }

    for (int y = ymin; y <= ymax; ++y)
    {
        std::vector<int> xIntersect;

        for (int i = 0; i < n; ++i)
        {
            int j = (i + 1) % n;
            int yi = pts[i].y, yj = pts[j].y;
            int xi = pts[i].x, xj = pts[j].x;

            if ((yi <= y && yj > y) || (yj <= y && yi > y))
            {
                // x at intersection
                int x = xi + (y - yi) * (xj - xi) / (yj - yi);
                xIntersect.push_back(x);
            }
        }

        std::sort(xIntersect.begin(), xIntersect.end());

        for (int k = 0; k + 1 < (int)xIntersect.size(); k += 2)
            for (int x = xIntersect[k]; x <= xIntersect[k+1]; ++x)
                SetPixel(hdc, x, y, color);
    }
}

// =====================================================================
// e) Convex polygon fill (delegates to generic scanline — convex is a
//    special case of the general scanline algorithm).
// =====================================================================
void FillConvexPolygon(HDC hdc, const std::vector<FillPoint>& pts, COLORREF color)
{
    ScanlineFill(hdc, pts, color);
}

// =====================================================================
// e) Non-Convex polygon fill (same scanline algorithm; it naturally
//    handles re-entrant polygons through the even-odd rule implicit in
//    the edge intersection sort).
// =====================================================================
void FillNonConvexPolygon(HDC hdc, const std::vector<FillPoint>& pts, COLORREF color)
{
    ScanlineFill(hdc, pts, color);
}

// =====================================================================
// f) Recursive flood fill.
//    Replaces targetColor (color at seed) with fillColor.
//    NOTE: for large regions the call stack may overflow; use iterative.
// =====================================================================
void FloodFillRecursive(HDC hdc, int x, int y, COLORREF fillColor, COLORREF targetColor)
{
    COLORREF cur = GetPixel(hdc, x, y);
    if (cur != targetColor) return;   // stop at anything that isn't the interior
    if (cur == fillColor)   return;   // already filled

    SetPixel(hdc, x, y, fillColor);
    FloodFillRecursive(hdc, x+1, y,   fillColor, targetColor);
    FloodFillRecursive(hdc, x-1, y,   fillColor, targetColor);
    FloodFillRecursive(hdc, x,   y+1, fillColor, targetColor);
    FloodFillRecursive(hdc, x,   y-1, fillColor, targetColor);
}

// =====================================================================
// f) Non-recursive (explicit stack) flood fill.
//    Replaces targetColor (color at seed) with fillColor.
// =====================================================================
void FloodFillIterative(HDC hdc, int x, int y, COLORREF fillColor, COLORREF targetColor)
{
    if (GetPixel(hdc, x, y) == fillColor) return; // nothing to do

    struct Pt { int x, y; };
    std::stack<Pt> stk;
    stk.push({x, y});

    while (!stk.empty())
    {
        Pt p = stk.top(); stk.pop();
        COLORREF cur = GetPixel(hdc, p.x, p.y);
        if (cur != targetColor) continue;  // border or already filled

        SetPixel(hdc, p.x, p.y, fillColor);
        stk.push({p.x+1, p.y});
        stk.push({p.x-1, p.y});
        stk.push({p.x,   p.y+1});
        stk.push({p.x,   p.y-1});
    }
}