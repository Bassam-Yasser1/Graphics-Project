#include "Curves.h"
#include <cmath>

// Draw a Cardinal Spline through the given control points
// tension: 0.0 (loose) – 1.0 (tight)
// steps: number of segments between points
void DrawCardinalSpline(HDC hdc,
                        const std::vector<CurvePoint>& pts,
                        double tension,
                        int    steps,
                        COLORREF color)
{
    int n = (int)pts.size();
    if (n < 2) return; // at least two points needed to define a path

    // Create extended point list with phantom start/end for endpoint conditions
    std::vector<CurvePoint> ep;
    ep.push_back({ 2*pts[0].x - pts[1].x, 2*pts[0].y - pts[1].y }); // phantom start
    for (auto& p : pts) ep.push_back(p);
    ep.push_back({ 2*pts[n-1].x - pts[n-2].x, 2*pts[n-1].y - pts[n-2].y }); // phantom end

    // tension factor
    double s = (1.0 - tension) * 0.5;

    // For each segment between pts[i] and pts[i+1], use pts[i-1] and pts[i+2] as control points
    int en = (int)ep.size();
    for (int i = 1; i + 2 < en; ++i) 
    {
        double p0x = ep[i-1].x, p0y = ep[i-1].y;
        double p1x = ep[i  ].x, p1y = ep[i  ].y;
        double p2x = ep[i+1].x, p2y = ep[i+1].y;
        double p3x = ep[i+2].x, p3y = ep[i+2].y;

        // Tangent vectors at p1 and p2
        double t1x = s * (p2x - p0x), t1y = s * (p2y - p0y);
        double t2x = s * (p3x - p1x), t2y = s * (p3y - p1y);

        // Draw the cubic Hermite curve segment from p1 to p2
        for (int k = 0; k <= steps; ++k)
        {
            // Cubic Hermite basis functions
            double t  = (double)k / steps;
            double t2 = t * t, t3 = t2 * t;

            double h00 =  2*t3 - 3*t2 + 1;
            double h10 =    t3 - 2*t2 + t;
            double h01 = -2*t3 + 3*t2;
            double h11 =    t3 -   t2;

            // Compute the point on the curve for parameter t
            double x = h00*p1x + h10*t1x + h01*p2x + h11*t2x;
            double y = h00*p1y + h10*t1y + h01*p2y + h11*t2y;

            SetPixel(hdc, (int)round(x), (int)round(y), color);
        }
    }
}
