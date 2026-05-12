#pragma once
#include <windows.h>
#include <vector>

struct CurvePoint { int x, y; };

void DrawCardinalSpline(HDC hdc,
                        const std::vector<CurvePoint>& pts,
                        double tension,
                        int    steps,
                        COLORREF color);
