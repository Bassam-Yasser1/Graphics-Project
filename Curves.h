#pragma once
#include <windows.h>
#include <vector>

// -------------------------------------------------------
// Curves.h
// Parametric curve algorithms:
//   - Cardinal Spline (Catmull-Rom is the special case s=0.5)
//     Tension parameter s: 0 = Catmull-Rom, 1 = linear segments
// -------------------------------------------------------

struct CurvePoint { int x, y; };

// Draw a Cardinal Spline through the given control points.
// tension: 0.0 (loose/Catmull-Rom) – 1.0 (tight/linear).
// steps:   number of parametric samples per segment.
void DrawCardinalSpline(HDC hdc,
                        const std::vector<CurvePoint>& pts,
                        double tension,
                        int    steps,
                        COLORREF color);
